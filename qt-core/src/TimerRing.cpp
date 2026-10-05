#include "pomo/TimerRing.h"
#include "pomo/Theme.h"
#include <QPainter>
namespace pomo {
void TimerRing::paintEvent(QPaintEvent*) {
    QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
    const theme::Palette pal = theme::palette(ph_);
    const int side = qMin(width(), height()) - 40;
    const QRectF r((width() - side) / 2.0, (height() - side) / 2.0, side, side);
    const qreal w = qMax(10.0, side / 22.0);
    const QColor accent = theme::phaseColor(ph_);
    p.setPen(QPen(pal.border, w, Qt::SolidLine, Qt::RoundCap)); p.drawArc(r, 0, 360 * 16);                 // 軌道
    if (rem_ > 0) { p.setPen(QPen(accent, w, Qt::SolidLine, Qt::RoundCap)); p.drawArc(r, 90 * 16, -int(360 * 16.0 * rem_ / tot_)); }
    QFont f = font(); f.setPixelSize(side / 4); f.setWeight(QFont::Light); 
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    f.setFeature("tnum", 1);   // 等寬數字，倒數時數字不會左右跳動
#endif
    p.setFont(f);
    p.setPen(pal.text);
    p.drawText(r.adjusted(0, -side / 14.0, 0, 0), Qt::AlignCenter, QString("%1:%2").arg(rem_ / 60, 2, 10, QChar('0')).arg(rem_ % 60, 2, 10, QChar('0')));
    f.setPixelSize(side / 13); f.setWeight(QFont::DemiBold); p.setFont(f); p.setPen(accent);
    p.drawText(r.adjusted(0, side / 3.4, 0, 0), Qt::AlignCenter, theme::phaseName(ph_));
}
}
