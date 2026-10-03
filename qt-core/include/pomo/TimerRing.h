#pragma once
#include <QWidget>
#include "pomo/Session.h"
namespace pomo {
// 圓環倒數：顏色隨階段變化，中央顯示剩餘時間與階段名稱
class TimerRing : public QWidget {
    Q_OBJECT
public:
    explicit TimerRing(QWidget* parent = nullptr) : QWidget(parent) { setMinimumSize(260, 260); }
    void setState(Phase p, int remainingSec, int totalSec) { ph_ = p; rem_ = remainingSec; tot_ = totalSec > 0 ? totalSec : 1; update(); }
    QSize sizeHint() const override { return {340, 340}; }
protected:
    void paintEvent(QPaintEvent*) override;
private:
    Phase ph_ = Phase::Idle;
    int rem_ = 0, tot_ = 1;
};
}
