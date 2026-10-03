#include "pomo/TimerRing.h"
#include "pomo/Theme.h"
#include <QPainter>
namespace pomo {
void TimerRing::paintEvent(QPaintEvent*) {
    QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
    const int side = qMin(width(), height()) - 32;
    const QRectF r((width() - side) / 2.0, (height() - side) / 2.0, side, side);
    const QColor accent = theme::phaseColor(ph_);
    p.setPen(QPen(QColor(128, 128, 128, 60), 14, Qt::SolidLine, Qt::RoundCap)); p.drawEllipse(r);
    p.setPen(QPen(accent, 14, Qt::SolidLine, Qt::RoundCap));
    p.drawArc(r, 90 * 16, -int(360 * 16.0 * rem_ / tot_));
    QFont f = font(); f.setPixelSize(side / 5); f.setWeight(QFont::Light); p.setFont(f);
    p.setPen(ph_ == Phase::Work ? QColor("#EEEEEE") : QColor("#222222"));
    p.drawText(r.adjusted(0, -side / 12.0, 0, 0), Qt::AlignCenter, QString("%1:%2").arg(rem_ / 60, 2, 10, QChar('0')).arg(rem_ % 60, 2, 10, QChar('0')));
    f.setPixelSize(side / 14); f.setWeight(QFont::Normal); p.setFont(f); p.setPen(accent);
    p.drawText(r.adjusted(0, side / 4.0, 0, 0), Qt::AlignCenter, theme::phaseName(ph_));
}
}
