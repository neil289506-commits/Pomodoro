#include "pomo/ForegroundWatcher.h"
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QTimer>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
namespace pomo {
class LinuxForegroundWatcher final : public IForegroundWatcher {
public:
    explicit LinuxForegroundWatcher(QObject* parent = nullptr) : IForegroundWatcher(parent) {
        timer_.setInterval(1000);
        connect(&timer_, &QTimer::timeout, this, &LinuxForegroundWatcher::poll);
        selfExe_ = QFileInfo(QCoreApplication::applicationFilePath()).fileName().toLower();
    }
    void start() override {
        if (!qEnvironmentVariableIsSet("DISPLAY")) {
            emit unavailable(QStringLiteral("Wayland/無 DISPLAY：僅支援 X11 前景偵測"));
            return;
        }
        display_ = XOpenDisplay(nullptr);
        if (!display_) {
            emit unavailable(QStringLiteral("無法連線 X11；Wayland 需額外 compositor/portal 支援"));
            return;
        }
        activeAtom_ = XInternAtom(display_, "_NET_ACTIVE_WINDOW", True);
        pidAtom_ = XInternAtom(display_, "_NET_WM_PID", True);
        if (activeAtom_ == None || pidAtom_ == None) {
            emit unavailable(QStringLiteral("X11 不支援 _NET_ACTIVE_WINDOW"));
            return;
        }
        timer_.start();
        poll();
    }
    void stop() override {
        timer_.stop();
        if (display_) { XCloseDisplay(display_); display_ = nullptr; }
    }
    ~LinuxForegroundWatcher() override { stop(); }
private:
    void poll() {
        if (!display_) return;
        const Window root = DefaultRootWindow(display_);
        Atom actualType = None;
        int actualFormat = 0;
        unsigned long nItems = 0, bytesAfter = 0;
        unsigned char* prop = nullptr;
        if (XGetWindowProperty(display_, root, activeAtom_, 0, 1, False, AnyPropertyType,
                               &actualType, &actualFormat, &nItems, &bytesAfter, &prop) != Success || !prop) return;
        const Window active = *reinterpret_cast<Window*>(prop);
        XFree(prop);
        if (!active) return;
        unsigned char* pidProp = nullptr;
        if (XGetWindowProperty(display_, active, pidAtom_, 0, 1, False, XA_CARDINAL,
                               &actualType, &actualFormat, &nItems, &bytesAfter, &pidProp) != Success || !pidProp) return;
        const unsigned long pid = *reinterpret_cast<unsigned long*>(pidProp);
        XFree(pidProp);
        QFile f(QStringLiteral("/proc/%1/comm").arg(pid));
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
        const QString exe = QString::fromUtf8(f.readLine()).trimmed().toLower();
        if (exe.isEmpty() || exe == last_) return;
        last_ = exe;
        emit foregroundChanged(exe, exe == selfExe_);
    }
    QTimer timer_;
    Display* display_ = nullptr;
    Atom activeAtom_ = None;
    Atom pidAtom_ = None;
    QString selfExe_;
    QString last_;
};
std::unique_ptr<IForegroundWatcher> createPlatformForegroundWatcher(QObject* parent) {
    return std::make_unique<LinuxForegroundWatcher>(parent);
}
}
