#pragma once
#include <QObject>
#include <QSet>
#include <QString>
namespace pomo {
// 專注守護：專注期間離開視窗（前景換成非放行 App）超過 kMaxLeaves 次即觸發 violated。
class Guardian : public QObject {
    Q_OBJECT
public:
    explicit Guardian(QObject* parent = nullptr) : QObject(parent) {}
    void setAllowedApps(const QSet<QString>& ids);
    void setActive(bool on) { active_ = on; leaves_ = 0; away_ = false; }
    int leaves() const { return leaves_; }
public slots:
    void onForegroundChanged(const QString& appId, bool isSelf);
signals:
    void leaveCounted(int n);
    void violated(const QString& reason);
private:
    QSet<QString> allowed_;
    bool active_ = false, away_ = false;
    int leaves_ = 0;
};
}
