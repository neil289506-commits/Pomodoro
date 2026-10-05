#include "pomo/Lockdown.h"
#include "pomo/I18n.h"
#include <QMetaObject>
#import <AppKit/AppKit.h>
#include <Carbon/Carbon.h>
namespace pomo {
// macOS：用 Kiosk 展示選項停用 Cmd+Tab、強制結束、Dock 與選單列、登出對話框；熱鍵用 Carbon 註冊（不需要輔助使用權限）。
class MacLockdown final : public ILockdown {
public:
    explicit MacLockdown(QObject* parent) : ILockdown(parent) { registerHotkey(); }
    ~MacLockdown() override { release(); unregisterHotkey(); }
    bool engage(WId) override { paused_ = false; apply(); [NSApp activateIgnoringOtherApps:YES]; return true; }
    void release() override { NSApp.presentationOptions = NSApplicationPresentationDefault; }
    void setPaused(bool p) override { paused_ = p; apply(); }
    QString describe() const override {
        return T("lock.desc.macos");
    }
private:
    void apply() {
        NSApp.presentationOptions = paused_ ? NSApplicationPresentationDefault
            : (NSApplicationPresentationHideDock | NSApplicationPresentationHideMenuBar |
               NSApplicationPresentationDisableProcessSwitching | NSApplicationPresentationDisableForceQuit |
               NSApplicationPresentationDisableSessionTermination | NSApplicationPresentationDisableHideApplication);
    }
    static OSStatus onHotkey(EventHandlerCallRef, EventRef, void* self) {
        QMetaObject::invokeMethod(static_cast<MacLockdown*>(self), "hotkeyPressed", Qt::QueuedConnection);
        return noErr;
    }
    void registerHotkey() {
        EventTypeSpec spec = {kEventClassKeyboard, kEventHotKeyPressed};
        InstallApplicationEventHandler(&MacLockdown::onHotkey, 1, &spec, this, &handler_);
        EventHotKeyID id = {'pomo', 1};
        RegisterEventHotKey(kVK_ANSI_A, controlKey | optionKey | shiftKey, id, GetApplicationEventTarget(), 0, &hotkey_);
    }
    void unregisterHotkey() {
        if (hotkey_) UnregisterEventHotKey(hotkey_);
        if (handler_) RemoveEventHandler(handler_);
    }
    EventHandlerRef handler_ = nullptr;
    EventHotKeyRef hotkey_ = nullptr;
    bool paused_ = false;
};
std::unique_ptr<ILockdown> createPlatformLockdown(QObject* parent) { return std::make_unique<MacLockdown>(parent); }
}
