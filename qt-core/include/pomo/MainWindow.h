#pragma once
#include <QElapsedTimer>
#include <QSet>
#include <QTimer>
#include <QWidget>
#include "pomo/Announcer.h"
#include "pomo/ForegroundWatcher.h"
#include "pomo/Guardian.h"
#include "pomo/Lockdown.h"
#include "pomo/Session.h"
class QLabel; class QPushButton; class QTabWidget;
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
    void applyTheme();
    void updateTexts();
    void updateGuard();
    void updateLock();
    void setCount(int n);
    void confirmStop();
    void refreshAllowed();
    void engageLock(int attempt);
    void openLauncher();
    void pauseForApp();
    void resumeLock();
    void onPhase(Phase p);
    enum class LockUi { None, Active, Paused, Failed };
    Session session_; Guardian guardian_; Announcer announcer_;
    std::unique_ptr<IForegroundWatcher> watcher_;
    std::unique_ptr<ILockdown> lock_;
    TimerRing* ring_; StatsPage* stats_; SettingsPage* settings_;
    QTabWidget* tabs_;
    QLabel *roundLabel_, *hint_, *countCaption_, *countValue_, *startHint_, *guard_, *lockInfo_;
    QPushButton *go_, *stop_, *launch_, *minus_, *plus_;
    QWidget* stepper_;
    QTimer raise_;
    QElapsedTimer launchedAt_;
    QSet<QString> allowedIds_;
    LockUi lockUi_ = LockUi::None;
    QString lockMsg_, guardLimitKey_;
    bool paused_ = false, running_ = false;
    int count_ = 1, phaseTotal_ = 1;
};
}
