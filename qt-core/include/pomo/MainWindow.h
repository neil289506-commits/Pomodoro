#pragma once
#include <QWidget>
#include "pomo/Announcer.h"
#include "pomo/ForegroundWatcher.h"
#include "pomo/Guardian.h"
#include "pomo/Session.h"
class QLabel; class QPushButton; class QSpinBox; class QTabWidget;
class QCloseEvent;
namespace pomo {
class TimerRing; class StatsPage; class SettingsPage;
class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow();
private:
    void closeEvent(QCloseEvent* e) override;
    QWidget* buildTimerTab();
    void onPhase(Phase p);
    Session session_; Guardian guardian_; Announcer announcer_;
    std::unique_ptr<IForegroundWatcher> watcher_;
    TimerRing* ring_; StatsPage* stats_; SettingsPage* settings_;
    QTabWidget* tabs_; QSpinBox* count_; QPushButton *go_, *stop_; QLabel* guard_;
    int phaseTotal_ = 1;
    bool running_ = false;
};
}
