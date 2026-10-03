#include "pomo/Lockdown.h"
#include <QGuiApplication>
#include <QTimer>
#include <X11/Xlib.h>
#include <X11/keysym.h>
namespace pomo {
// X11：XGrabKeyboard 讓所有按鍵只送給本視窗，Alt+Tab、Super、工作列快捷鍵都不會傳到視窗管理員。
// 暫停時改用 XGrabKey 只攔截「開啟放行 App」熱鍵。Wayland 不允許這種攔截，只能退回全螢幕置頂。
class LinuxLockdown final : public ILockdown {
public:
    explicit LinuxLockdown(QObject* parent) : ILockdown(parent) {
        poll_.setInterval(50);
        connect(&poll_, &QTimer::timeout, this, &LinuxLockdown::drain);
    }
    ~LinuxLockdown() override { release(); }
    bool engage(WId w) override {
        if (QGuiApplication::platformName() != QStringLiteral("xcb")) {
            emit failed(QStringLiteral("Wayland 不允許攔截系統快捷鍵：無法硬鎖，只能使用全螢幕置頂"));
            return false;
        }
        if (!display_) display_ = XOpenDisplay(nullptr);
        if (!display_) { emit failed(QStringLiteral("無法連線 X11，無法硬鎖")); return false; }
        poll_.stop(); grabHotkey(false);   // 從暫停回來時清掉熱鍵抓取
        window_ = static_cast<Window>(w);
        paused_ = false;
        return grab();
    }
    void release() override {
        poll_.stop();
        if (!display_) return;
        XUngrabKeyboard(display_, CurrentTime);
        grabHotkey(false);
        XCloseDisplay(display_); display_ = nullptr;
    }
    void setPaused(bool p) override {
        if (!display_ || p == paused_) return;
        paused_ = p;
        if (p) { XUngrabKeyboard(display_, CurrentTime); grabHotkey(true); poll_.start(); }
        else { poll_.stop(); grabHotkey(false); grab(); }
    }
    QString describe() const override {
        return QStringLiteral("硬鎖（X11 鍵盤獨佔）：Alt+Tab、Super 等切換快捷鍵已攔截。無法攔截 Ctrl+Alt+F1~F7 切換終端機與電源鍵。");
    }
private:
    bool grab() {
        const int r = XGrabKeyboard(display_, window_, True, GrabModeAsync, GrabModeAsync, CurrentTime);
        XFlush(display_);
        if (r != GrabSuccess) {
            emit failed(QStringLiteral("無法取得鍵盤獨佔（X11 錯誤碼 %1，可能有其他程式已獨佔鍵盤）").arg(r));
            return false;
        }
        return true;
    }
    void grabHotkey(bool on) {
        if (!display_) return;
        const KeyCode code = XKeysymToKeycode(display_, XK_a);
        const unsigned base = ControlMask | Mod1Mask | ShiftMask;
        for (unsigned extra : {0u, (unsigned)LockMask, (unsigned)Mod2Mask, (unsigned)(LockMask | Mod2Mask)}) {  // 含 CapsLock / NumLock
            if (on) XGrabKey(display_, code, base | extra, DefaultRootWindow(display_), True, GrabModeAsync, GrabModeAsync);
            else XUngrabKey(display_, code, base | extra, DefaultRootWindow(display_));
        }
        XFlush(display_);
    }
    void drain() {
        while (display_ && XPending(display_)) {
            XEvent ev; XNextEvent(display_, &ev);
            if (ev.type == KeyPress) emit hotkeyPressed();
        }
    }
    QTimer poll_;
    Display* display_ = nullptr;
    Window window_ = 0;
    bool paused_ = false;
};
std::unique_ptr<ILockdown> createPlatformLockdown(QObject* parent) { return std::make_unique<LinuxLockdown>(parent); }
}
