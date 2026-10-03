#include "pomo/ForegroundWatcher.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QTimer>
#include <windows.h>
namespace pomo {
class WinForegroundWatcher final : public IForegroundWatcher {
public:
    explicit WinForegroundWatcher(QObject* parent = nullptr) : IForegroundWatcher(parent) {
        timer_.setInterval(1000);
        connect(&timer_, &QTimer::timeout, this, &WinForegroundWatcher::poll);
        selfExe_ = QFileInfo(QCoreApplication::applicationFilePath()).fileName().toLower();
    }
    void start() override { timer_.start(); poll(); }
    void stop() override { timer_.stop(); }
private:
    void poll() {
        HWND hwnd = GetForegroundWindow();
        if (!hwnd) return;
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (!pid) return;
        HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!process) return;
        wchar_t path[MAX_PATH] = {};
        DWORD size = MAX_PATH;
        if (!QueryFullProcessImageNameW(process, 0, path, &size)) { CloseHandle(process); return; }
        CloseHandle(process);
        const QString exe = QFileInfo(QString::fromWCharArray(path)).fileName().toLower();
        if (exe == last_) return;
        last_ = exe;
        emit foregroundChanged(exe, exe == selfExe_);
    }
    QTimer timer_;
    QString selfExe_;
    QString last_;
};
std::unique_ptr<IForegroundWatcher> createPlatformForegroundWatcher(QObject* parent) {
    return std::make_unique<WinForegroundWatcher>(parent);
}
}
