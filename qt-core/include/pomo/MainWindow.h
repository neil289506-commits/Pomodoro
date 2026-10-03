#pragma once
#include <QWidget>
#include "pomo/Announcer.h"
#include "pomo/Guardian.h"
#include "pomo/Session.h"
class QLabel; class QPushButton; class QSpinBox; class QTabWidget;
namespace pomo {
class TimerRing; class StatsPage; class SettingsPage;
class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow();
private:
    QWidget* buildTimerTab();
    void onPhase(Phase p);
    Session session_; Guardian guardian_; Announcer announcer_;
    TimerRing* ring_; StatsPage* stats_; SettingsPage* settings_;
    QTabWidget* tabs_; QSpinBox* count_; QPushButton *go_, *stop_; QLabel* guard_;
    int phaseTotal_ = 1;
};
}
