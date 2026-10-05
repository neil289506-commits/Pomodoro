#pragma once
#include <QColor>
#include <QString>
#include "pomo/Session.h"
namespace pomo::theme {
struct Palette { QColor bg, surface, text, muted, border; };
bool isDark(Phase p);          // 專注中一律深色低干擾；其餘時段跟隨系統深色模式
Palette palette(Phase p);
QColor phaseColor(Phase p);
QString phaseName(Phase p);    // 已翻譯
QString styleSheet(Phase p);
}
