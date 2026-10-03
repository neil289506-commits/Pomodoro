#pragma once
#include <QObject>
#include <QTimer>
#include <QDateTime>
namespace pomo {
enum class Phase { Idle, Prep, Work, Rest, Finished, Voided };
// 番茄鐘流程：準備 3 分 -> (專注 -> 休息) x N。每個番茄鐘不可切割。
class Session : public QObject {
    Q_OBJECT
public:
    explicit Session(QObject* parent = nullptr);
    Phase phase() const { return phase_; }
    int completed() const { return done_; }
    int total() const { return total_; }
public slots:
    void start(int total);
    void stop();                              // 停止 = 作廢
    void voidCurrent(const QString& reason);  // 守護觸發作廢
signals:
    void phaseChanged(pomo::Phase p);
    void ticked(int remainingSec);
    void pomodoroDone(int index);
    void pomodoroVoided(int index, const QString& reason);
private:
    void begin(Phase p, int sec);
    void onTick();
    QTimer t_;
    Phase phase_ = Phase::Idle;
    int total_ = 0, done_ = 0;
    QDateTime end_;
};
}
