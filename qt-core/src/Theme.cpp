#include "pomo/Theme.h"
#include "pomo/I18n.h"
#include <QGuiApplication>
#include <QStyleHints>
namespace pomo::theme {
bool isDark(Phase p) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    return p == Phase::Work || QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#else
    return p == Phase::Work;   // 6.5 以前沒有系統深色模式 API
#endif
}
Palette palette(Phase p) {
    if (isDark(p)) return {QColor("#121214"), QColor("#1C1C20"), QColor("#F2F2F3"), QColor("#9A9AA2"), QColor("#2E2E34")};
    return {QColor("#FAF7F2"), QColor("#FFFFFF"), QColor("#1F1F1F"), QColor("#6B6B6B"), QColor("#E6E0D6")};
}
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
    case Phase::Prep: return T("phase.prep");
    case Phase::Work: return T("phase.work");
    case Phase::Rest: return T("phase.rest");
    case Phase::Finished: return T("phase.finished");
    case Phase::Voided: return T("phase.voided");
    default: return T("phase.idle");
    }
}
QString styleSheet(Phase p) {
    const Palette c = palette(p);
    const QString acc = phaseColor(p).name();
    return QString(
        "QWidget{background:%1;color:%2;font-size:15px;}"
        "QLabel{background:transparent;}"
        "QLabel[muted=\"true\"]{color:%3;font-size:13px;}"
        "QLabel[title=\"true\"]{font-size:20px;font-weight:600;}"
        "QLabel[bignum=\"true\"]{font-size:28px;font-weight:600;}"
        "QPushButton{background:transparent;border:1px solid %4;border-radius:12px;padding:12px 20px;color:%2;}"
        "QPushButton:hover{background:%5;}"
        "QPushButton:disabled{color:%3;border-color:%4;}"
        "QPushButton[primary=\"true\"]{background:%6;border:none;color:#FFFFFF;font-size:17px;font-weight:600;padding:15px 28px;}"
        "QPushButton[primary=\"true\"]:hover{background:%6;}"
        "QPushButton[primary=\"true\"]:disabled{background:%4;color:%3;}"
        "QPushButton[round=\"true\"]{border-radius:22px;min-width:44px;max-width:44px;min-height:44px;max-height:44px;padding:0;font-size:22px;}"
        "QTabWidget::pane{border:none;}"
        "QTabBar{background:%1;}"
        "QTabBar::tab{padding:14px 26px;color:%3;border-top:3px solid transparent;}"
        "QTabBar::tab:selected{color:%6;border-top:3px solid %6;}"
        "QSpinBox,QLineEdit,QComboBox,QListWidget{background:%5;border:1px solid %4;border-radius:10px;padding:8px;}"
        "QListWidget::item{padding:10px;border-bottom:1px solid %4;}"
        "QListWidget::item:selected{background:%4;color:%2;}"
        "QGroupBox{background:%5;border:1px solid %4;border-radius:14px;margin-top:14px;padding:14px;font-weight:600;}"
        "QGroupBox::title{subcontrol-origin:margin;left:14px;padding:0 6px;}"
        "QFrame[card=\"true\"]{background:%5;border:1px solid %4;border-radius:14px;}"
        "QScrollArea{border:none;}"
        "QScrollBar:vertical{width:8px;background:transparent;}"
        "QScrollBar::handle:vertical{background:%4;border-radius:4px;}"
        "QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{height:0;}"
    ).arg(c.bg.name(), c.text.name(), c.muted.name(), c.border.name(), c.surface.name(), acc);
}
}
