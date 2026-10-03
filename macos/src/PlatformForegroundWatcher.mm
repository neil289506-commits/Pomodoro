#include "pomo/ForegroundWatcher.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QTimer>
#import <AppKit/AppKit.h>  // NSWorkspace / NSRunningApplication（含 executableURL）
namespace pomo {
class MacForegroundWatcher final : public IForegroundWatcher {
public:
    explicit MacForegroundWatcher(QObject* parent = nullptr) : IForegroundWatcher(parent) {
        timer_.setInterval(1000);
        connect(&timer_, &QTimer::timeout, this, &MacForegroundWatcher::poll);
        selfExe_ = QFileInfo(QCoreApplication::applicationFilePath()).fileName().toLower();
    }
    void start() override { timer_.start(); poll(); }
    void stop() override { timer_.stop(); }
private:
    void poll() {
        NSRunningApplication* app = [[NSWorkspace sharedWorkspace] frontmostApplication];
        if (!app) return;
        NSString* path = app.executableURL.path;
        const QString exe = QFileInfo(QString::fromNSString(path)).fileName().toLower();
        if (exe.isEmpty() || exe == last_) return;
        last_ = exe;
        emit foregroundChanged(exe, exe == selfExe_);
    }
    QTimer timer_;
    QString selfExe_;
    QString last_;
};
std::unique_ptr<IForegroundWatcher> createPlatformForegroundWatcher(QObject* parent) {
    return std::make_unique<MacForegroundWatcher>(parent);
}
}
