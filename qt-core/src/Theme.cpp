#include "pomo/Theme.h"
namespace pomo::theme {
QColor phaseColor(Phase p) {
    switch (p) {
    case Phase::Prep: return QColor("#F5A623");
    case Phase::Work: return QColor("#E5484D");
    case Phase::Rest: return QColor("#30A46C");
    case Phase::Finished: return QColor("#3E63DD");
    case Phase::Voided: return QColor("#8B8D98");
    default: return QColor("#E5484D");
    }
}
QString phaseName(Phase p) {
    switch (p) {
    case Phase::Prep: return QStringLiteral("準備中");
    case Phase::Work: return QStringLiteral("專注中");
    case Phase::Rest: return QStringLiteral("休息中");
    case Phase::Finished: return QStringLiteral("全部完成");
    case Phase::Voided: return QStringLiteral("已作廢");
    default: return QStringLiteral("待命");
    }
}
QString styleSheet(Phase p) {
    const bool dark = p == Phase::Work;
    const QString bg = dark ? "#111111" : "#FAF7F2", fg = dark ? "#EEEEEE" : "#222222", field = dark ? "#1B1B1B" : "#FFFFFF";
    const QString acc = phaseColor(p).name();
    return QString("QWidget{background:%1;color:%2;font-size:15px;}"
                   "QPushButton{background:%3;color:white;border:none;border-radius:10px;padding:10px 18px;}"
                   "QPushButton:disabled{background:#999999;}"
                   "QTabBar::tab{padding:8px 18px;}QTabBar::tab:selected{border-bottom:3px solid %3;}"
                   "QSpinBox,QLineEdit,QListWidget,QTableWidget{background:%4;border:1px solid #88888844;border-radius:6px;padding:4px;}"
                   "QGroupBox{border:1px solid #88888844;border-radius:8px;margin-top:12px;padding:8px;}"
                   "QGroupBox::title{subcontrol-origin:margin;left:10px;}").arg(bg, fg, acc, field);
}
}
