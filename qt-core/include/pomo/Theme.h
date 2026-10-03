#pragma once
#include <QColor>
#include <QString>
#include "pomo/Session.h"
namespace pomo::theme {
QColor phaseColor(Phase p);
QString phaseName(Phase p);
QString styleSheet(Phase p);  // 專注時深色低干擾，其餘時段暖色淺底
}
