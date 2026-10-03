#pragma once
#include <QObject>
#include <memory>
namespace pomo {
class IForegroundWatcher : public QObject {
    Q_OBJECT
public:
    explicit IForegroundWatcher(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~IForegroundWatcher() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
signals:
    void foregroundChanged(const QString& appId, bool isSelf);
    void unavailable(const QString& reason);
};
// 由各桌面平台目錄實作（Windows / macOS / Linux）
std::unique_ptr<IForegroundWatcher> createPlatformForegroundWatcher(QObject* parent = nullptr);
}
