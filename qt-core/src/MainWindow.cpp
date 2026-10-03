#include "pomo/MainWindow.h"
#include "pomo/AppEntry.h"
#include "pomo/AppInfo.h"
#include "pomo/Config.h"
#include "pomo/History.h"
#include "pomo/LauncherDialog.h"
#include "pomo/Settings.h"
#include "pomo/SettingsPage.h"
#include "pomo/StatsPage.h"
#include "pomo/Theme.h"
#include "pomo/TimerRing.h"
#include <QApplication>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QShortcut>
#include <QSpinBox>
#include <QTabBar>
#include <QTabWidget>
#include <QVBoxLayout>
namespace pomo {
MainWindow::MainWindow() {
    setWindowTitle(app::name()); resize(520, 700);
    tabs_ = new QTabWidget; stats_ = new StatsPage; settings_ = new SettingsPage;
    tabs_->addTab(buildTimerTab(), "計時"); tabs_->addTab(stats_, "紀錄"); tabs_->addTab(settings_, "設定");
    auto* root = new QVBoxLayout(this); root->addWidget(tabs_);
    refreshAllowed();
    connect(settings_, &SettingsPage::allowedAppsChanged, this, [this](const QSet<QString>&) { refreshAllowed(); });
    lock_ = createPlatformLockdown(this);
    connect(lock_.get(), &ILockdown::hotkeyPressed, this, &MainWindow::openLauncher);
    connect(lock_.get(), &ILockdown::failed, this, [this](const QString& why) { lockInfo_->setText(QStringLiteral("硬鎖失敗：%1").arg(why)); });
    auto* hotkey = new QShortcut(QKeySequence(QString::fromLatin1(kLauncherHotkey)), this);  // 本視窗在前景時的熱鍵
    hotkey->setContext(Qt::ApplicationShortcut);
    connect(hotkey, &QShortcut::activated, this, &MainWindow::openLauncher);
    connect(launch_, &QPushButton::clicked, this, &MainWindow::openLauncher);
    raise_.setInterval(400);   // 專注中視窗若被搶走焦點，立刻拉回最上層
    connect(&raise_, &QTimer::timeout, this, [this] { if (!paused_ && !isActiveWindow()) { raise(); activateWindow(); } });
    watcher_ = createPlatformForegroundWatcher(this);
    if (watcher_) {
        connect(watcher_.get(), &IForegroundWatcher::foregroundChanged, &guardian_, &Guardian::onForegroundChanged);
        // 放行 App 使用中（硬鎖暫停）：換到不在清單的 App，或回到本視窗，就立刻重新鎖上
        connect(watcher_.get(), &IForegroundWatcher::foregroundChanged, this, [this](const QString& id, bool isSelf) {
            if (!paused_ || !launchedAt_.hasExpired(5000)) return;
            if (isSelf || !allowedIds_.contains(id)) resumeLock();
        });
        connect(watcher_.get(), &IForegroundWatcher::unavailable, this, [this](const QString& why) {
            guard_->setText(QString("專注守護限制：%1").arg(why));
        });
        watcher_->start();
    } else {
        connect(qApp, &QApplication::applicationStateChanged, this,
                [this](Qt::ApplicationState s) { guardian_.onForegroundChanged("other", s == Qt::ApplicationActive); });
    }
    connect(go_, &QPushButton::clicked, this, [this] { Settings::instance().setRounds(count_->value()); session_.start(count_->value()); });
    connect(stop_, &QPushButton::clicked, &session_, &Session::stop);
    connect(&session_, &Session::ticked, this, [this](int s) { ring_->setState(session_.phase(), s, phaseTotal_); });
    connect(&session_, &Session::phaseChanged, this, &MainWindow::onPhase);
    connect(&session_, &Session::pomodoroDone, this, [this](int i) { History::record(i, true); stats_->refresh(); });
    connect(&session_, &Session::pomodoroVoided, this, [this](int i, const QString& r) { History::record(i, false, r); stats_->refresh(); });
    connect(&guardian_, &Guardian::leaveCounted, this, [this](int n) { guard_->setText(QString("專注守護：已離開 %1 / %2 次").arg(n).arg(kMaxLeaves)); });
    connect(&guardian_, &Guardian::violated, &session_, &Session::voidCurrent);
    onPhase(Phase::Idle);
}
QWidget* MainWindow::buildTimerTab() {
    auto* w = new QWidget; auto* l = new QVBoxLayout(w);
    ring_ = new TimerRing; ring_->setState(Phase::Idle, kWorkSec, kWorkSec);
    count_ = new QSpinBox; count_->setRange(1, 12); count_->setValue(Settings::instance().rounds()); count_->setSuffix(" 個番茄鐘");
    go_ = new QPushButton("開始（先準備 3 分鐘）"); stop_ = new QPushButton("停止（番茄鐘作廢）");
    guard_ = new QLabel("專注守護：待命"); guard_->setAlignment(Qt::AlignCenter);
    launch_ = new QPushButton(QStringLiteral("開啟放行 App（%1）").arg(QString::fromLatin1(kLauncherHotkey))); launch_->setEnabled(false);
    lockInfo_ = new QLabel; lockInfo_->setWordWrap(true); lockInfo_->setAlignment(Qt::AlignCenter);
    l->addWidget(ring_, 1); l->addWidget(count_); l->addWidget(go_); l->addWidget(stop_); l->addWidget(launch_); l->addWidget(guard_); l->addWidget(lockInfo_);
    return w;
}
void MainWindow::onPhase(Phase p) {
    const bool running = p == Phase::Prep || p == Phase::Work || p == Phase::Rest;
    running_ = running;
    guardian_.setActive(p == Phase::Work);
    if (p == Phase::Work) guard_->setText(QString("專注守護：已離開 0 / %1 次").arg(kMaxLeaves));
    else if (!running) guard_->setText("專注守護：待命");
    switch (p) {
    case Phase::Prep: phaseTotal_ = kPrepSec; break;
    case Phase::Work: phaseTotal_ = kWorkSec; break;
    case Phase::Rest: phaseTotal_ = session_.completed() % kLongRestEvery == 0 ? kLongRestSec : kShortRestSec; break;
    default: phaseTotal_ = 1; break;
    }
    ring_->setState(p, running ? phaseTotal_ : (p == Phase::Idle ? kWorkSec : 0), running ? phaseTotal_ : (p == Phase::Idle ? kWorkSec : 1));
    setStyleSheet(theme::styleSheet(p));
    count_->setEnabled(!running); go_->setEnabled(!running); stop_->setEnabled(running);
    tabs_->tabBar()->setVisible(p != Phase::Work);   // 專注時隱藏分頁，只留圓環與停止鍵
    setWindowFlag(Qt::WindowStaysOnTopHint, p == Phase::Work);
    show();
    if (p == Phase::Work) { tabs_->setCurrentIndex(0); showFullScreen(); } else showNormal();
    paused_ = false;
    launch_->setEnabled(p == Phase::Work);
    if (p == Phase::Work) { raise_.start(); QTimer::singleShot(300, this, [this] { engageLock(0); }); }   // 等視窗真的顯示後再鎖
    else { raise_.stop(); lock_->release(); lockInfo_->clear(); }
    switch (p) {
    case Phase::Prep: announcer_.say("準備開始，請整理好工作環境"); break;
    case Phase::Work: announcer_.say("開始專注"); break;
    case Phase::Rest: announcer_.say("番茄鐘完成，請休息"); break;
    case Phase::Finished: announcer_.say("全部完成"); break;
    case Phase::Voided: announcer_.say("番茄鐘已作廢"); break;
    default: break;
    }
}
void MainWindow::refreshAllowed() {
    QSet<QString> ids;
    for (const QString& e : Settings::instance().allowedApps()) ids.insert(appentry::idOf(e));
    allowedIds_ = ids;
    guardian_.setAllowedApps(ids);   // 與前景偵測的 ID 格式一致（小寫檔名）
}
void MainWindow::engageLock(int attempt) {
    if (session_.phase() != Phase::Work || paused_) return;
    if (lock_->engage(winId())) lockInfo_->setText(QStringLiteral("%1\n熱鍵：%2").arg(lock_->describe(), QString::fromLatin1(kLauncherHotkey)));
    else if (attempt < 2) QTimer::singleShot(400, this, [this, attempt] { engageLock(attempt + 1); });
}
void MainWindow::openLauncher() {
    if (session_.phase() != Phase::Work) return;
    if (paused_) { resumeLock(); return; }   // 暫停中再按一次熱鍵：回到鎖定
    LauncherDialog dlg(Settings::instance().allowedApps(), this);
    dlg.setWindowFlag(Qt::WindowStaysOnTopHint);
    connect(&dlg, &LauncherDialog::launched, this, [this](const QString&) { pauseForApp(); });
    dlg.exec();
}
void MainWindow::pauseForApp() {
    paused_ = true; launchedAt_.start();
    lock_->setPaused(true);
    raise_.stop();
    lockInfo_->setText(QStringLiteral("已暫停硬鎖：放行 App 使用中。離開該 App 或再按 %1 就會重新鎖上。").arg(QString::fromLatin1(kLauncherHotkey)));
    showMinimized();   // 讓出畫面給放行的 App
}
void MainWindow::resumeLock() {
    if (!paused_) return;
    paused_ = false;
    showFullScreen(); raise(); activateWindow();
    raise_.start();
    QTimer::singleShot(300, this, [this] { engageLock(0); });
}
void MainWindow::closeEvent(QCloseEvent* e) {
    if (running_) { e->ignore(); return; }
    QWidget::closeEvent(e);
}
}
