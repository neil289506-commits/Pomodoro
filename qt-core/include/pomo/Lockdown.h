#pragma once
#include <QObject>
#include <QString>
#include <QtGui/qwindowdefs.h>
#include <memory>
namespace pomo {
// 硬鎖：在系統層攔截切換視窗 / 工作列 / 結束程式的快捷鍵（各平台可攔截的範圍不同，見 describe()）。
class ILockdown : public QObject {
    Q_OBJECT
public:
    explicit ILockdown(QObject* parent = nullptr) : QObject(parent) {}
    virtual bool engage(WId window) = 0;      // 啟用；失敗時回傳 false 並發出 failed
    virtual void release() = 0;               // 完全解除
    virtual void setPaused(bool paused) = 0;  // 暫停：放行 App 使用中，開放切換快捷鍵，但保留「開啟放行 App」熱鍵
    virtual QString describe() const = 0;     // 這個平台實際能擋什麼、擋不了什麼
signals:
    void hotkeyPressed();                     // 使用者按了 Config.h 的 kLauncherHotkey
    void failed(const QString& reason);
};
// 由各桌面平台目錄實作（Windows / macOS / Linux）
std::unique_ptr<ILockdown> createPlatformLockdown(QObject* parent = nullptr);
}
