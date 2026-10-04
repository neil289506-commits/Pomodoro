#include "pomo/MainWindow.h"
#include "pomo/AppEntry.h"
#include "pomo/AppInfo.h"
#include "pomo/Config.h"
#include "pomo/History.h"
#include "pomo/I18n.h"
#include "pomo/LauncherDialog.h"
#include "pomo/Settings.h"
#include "pomo/SettingsPage.h"
#include "pomo/StatsPage.h"
#include "pomo/Theme.h"
#include "pomo/TimerRing.h"
#include <QApplication>
#include <QCloseEvent>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <QStyleHints>
#include <QTabBar>
#include <QTabWidget>
#include <QVBoxLayout>
namespace pomo {
namespace {
QLabel* makeLabel(const char* prop, QWidget* parent = nullptr) {
    auto* l = new QLabel(parent); l->setProperty(prop, true); l->setAlignment(Qt::AlignCenter); l->setWordWrap(true); return l;
}
}
MainWindow::MainWindow() {
    setWindowTitle(app::name()); setWindowIcon(QIcon(QStringLiteral(":/assets/tomato.png")));
    setMinimumSize(440, 680); resize(480, 760);
    tabs_ = new QTabWidget; tabs_->setTabPosition(QTabWidget::South); stats_ = new StatsPage; settings_ = new SettingsPage;
    tabs_->addTab(buildTimerTab(), QString()); tabs_->addTab(stats_, QString()); tabs_->addTab(settings_, QString());
    auto* root = new QVBoxLayout(this); root->setContentsMargins(0, 0, 0, 0); root->addWidget(tabs_);
    refreshAllowed();
    connect(settings_, &SettingsPage::allowedAppsChanged, this, [this](const QSet<QString>&) { refreshAllowed(); });
    connect(&I18n::instance(), &I18n::languageChanged, this, [this] { updateTexts(); updateGuard(); updateLock(); stats_->refresh(); ring_->update(); });
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this, [this] { applyTheme(); });
#endif

    lock_ = createPlatformLockdown(this);
    connect(lock_.get(), &ILockdown::hotkeyPressed, this, &MainWindow::openLauncher);
    connect(lock_.get(), &ILockdown::failed, this, [this](const QString& why) { lockUi_ = LockUi::Failed; lockMsg_ = why; updateLock(); });
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
        connect(watcher_.get(), &IForegroundWatcher::unavailable, this, [this](const QString& why) { guardLimitKey_ = why; updateGuard(); });
        watcher_->start();
    } else {
        connect(qApp, &QApplication::applicationStateChanged, this,
                [this](Qt::ApplicationState s) { guardian_.onForegroundChanged("other", s == Qt::ApplicationActive); });
    }
    connect(go_, &QPushButton::clicked, this, [this] { Settings::instance().setRounds(count_); session_.start(count_); });
    connect(stop_, &QPushButton::clicked, this, &MainWindow::confirmStop);
    connect(minus_, &QPushButton::clicked, this, [this] { setCount(count_ - 1); });
    connect(plus_, &QPushButton::clicked, this, [this] { setCount(count_ + 1); });
    connect(&session_, &Session::ticked, this, [this](int s) { ring_->setState(session_.phase(), s, phaseTotal_); });
    connect(&session_, &Session::phaseChanged, this, &MainWindow::onPhase);
    connect(&session_, &Session::pomodoroDone, this, [this](int i) { History::record(i, true); stats_->refresh(); });
    connect(&session_, &Session::pomodoroVoided, this, [this](int i, const QString& r) { History::record(i, false, r); stats_->refresh(); });
    connect(&guardian_, &Guardian::leaveCounted, this, [this](int) { updateGuard(); });
    connect(&guardian_, &Guardian::violated, &session_, &Session::voidCurrent);
    setCount(Settings::instance().rounds());
    onPhase(Phase::Idle);
}
QWidget* MainWindow::buildTimerTab() {
    auto* w = new QWidget; auto* l = new QVBoxLayout(w); l->setContentsMargins(28, 22, 28, 12); l->setSpacing(10);
    roundLabel_ = makeLabel("title"); hint_ = makeLabel("muted");
    ring_ = new TimerRing; ring_->setState(Phase::Idle, kWorkSec, kWorkSec);
    stepper_ = new QWidget; auto* sl = new QVBoxLayout(stepper_); sl->setContentsMargins(0, 0, 0, 0); sl->setSpacing(6);
    countCaption_ = makeLabel("muted");
    minus_ = new QPushButton(QStringLiteral("−")); plus_ = new QPushButton(QStringLiteral("+"));
    for (auto* b : {minus_, plus_}) { b->setProperty("round", true); b->setCursor(Qt::PointingHandCursor); }
    countValue_ = makeLabel("bignum"); countValue_->setMinimumWidth(70);
    auto* row = new QHBoxLayout; row->addStretch(1); row->addWidget(minus_); row->addWidget(countValue_); row->addWidget(plus_); row->addStretch(1);
    sl->addWidget(countCaption_); sl->addLayout(row);
    startHint_ = makeLabel("muted");
    go_ = new QPushButton; go_->setProperty("primary", true); go_->setCursor(Qt::PointingHandCursor);
    stop_ = new QPushButton; stop_->setCursor(Qt::PointingHandCursor);
    launch_ = new QPushButton; launch_->setCursor(Qt::PointingHandCursor);
    guard_ = makeLabel("muted"); lockInfo_ = makeLabel("muted");
    l->addWidget(roundLabel_); l->addWidget(hint_); l->addWidget(ring_, 1); l->addWidget(stepper_); l->addWidget(startHint_);
    l->addWidget(go_); l->addWidget(stop_); l->addWidget(launch_); l->addWidget(guard_); l->addWidget(lockInfo_);
    return w;
}
void MainWindow::setCount(int n) {
    count_ = qBound(1, n, 12);
    countValue_->setText(QString::number(count_));
    minus_->setEnabled(count_ > 1); plus_->setEnabled(count_ < 12);
}
void MainWindow::applyTheme() { setStyleSheet(theme::styleSheet(session_.phase())); ring_->update(); }
void MainWindow::updateTexts() {
    tabs_->setTabText(0, T("tab.timer")); tabs_->setTabText(1, T("tab.history")); tabs_->setTabText(2, T("tab.settings"));
    const Phase p = session_.phase();
    const bool run = p == Phase::Prep || p == Phase::Work || p == Phase::Rest;
    int idx = p == Phase::Rest ? session_.completed() : qMin(session_.completed() + 1, session_.total());
    roundLabel_->setText(run ? T("timer.round").arg(qMax(1, idx)).arg(session_.total()) : QString());
    switch (p) {
    case Phase::Prep: hint_->setText(T("timer.prep_hint")); break;
    case Phase::Rest: hint_->setText(T("timer.rest_hint")); break;
    case Phase::Finished: hint_->setText(T("timer.done_hint")); break;
    case Phase::Voided: hint_->setText(T("timer.voided_hint")); break;
    case Phase::Idle: hint_->setText(T("timer.idle_hint")); break;
    default: hint_->clear(); break;
    }
    countCaption_->setText(T("timer.pomodoros")); startHint_->setText(T("timer.start_hint"));
    go_->setText(T("timer.start")); stop_->setText(T("timer.stop")); launch_->setText(T("timer.launch").arg(app::hotkeyText()));
}
void MainWindow::updateGuard() {
    QString t = session_.phase() == Phase::Work ? T("guard.leaves").arg(guardian_.leaves()).arg(kMaxLeaves) : T("guard.idle");
    if (!guardLimitKey_.isEmpty()) t += "\n" + T("guard.limited").arg(I18n::instance().t(guardLimitKey_));
    guard_->setText(t);
}
void MainWindow::updateLock() {
    switch (lockUi_) {
    case LockUi::Active: lockInfo_->setText(lock_->describe() + "\n" + T("lock.hotkey_line").arg(app::hotkeyText())); break;
    case LockUi::Paused: lockInfo_->setText(T("lock.paused").arg(app::hotkeyText())); break;
    case LockUi::Failed: lockInfo_->setText(T("lock.failed").arg(lockMsg_)); break;
    default: lockInfo_->clear(); break;
    }
}
void MainWindow::confirmStop() {
    const Phase p = session_.phase();
    QMessageBox box(this); box.setIcon(QMessageBox::Warning);
    box.setWindowTitle(T("confirm.title")); box.setText(T("confirm.title"));
    box.setInformativeText(p == Phase::Prep || p == Phase::Work ? T("confirm.work") : T("confirm.rest"));
    auto* keep = box.addButton(T("confirm.keep"), QMessageBox::RejectRole);
    auto* stop = box.addButton(T("confirm.stop"), QMessageBox::DestructiveRole);
    box.setDefaultButton(keep); box.exec();
    if (box.clickedButton() == stop) session_.stop();
}
void MainWindow::onPhase(Phase p) {
    const bool running = p == Phase::Prep || p == Phase::Work || p == Phase::Rest;
    running_ = running;
    guardian_.setActive(p == Phase::Work);
    switch (p) {
    case Phase::Prep: phaseTotal_ = kPrepSec; break;
    case Phase::Work: phaseTotal_ = kWorkSec; break;
    case Phase::Rest: phaseTotal_ = session_.completed() % kLongRestEvery == 0 ? kLongRestSec : kShortRestSec; break;
    default: phaseTotal_ = 1; break;
    }
    ring_->setState(p, running ? phaseTotal_ : (p == Phase::Idle ? kWorkSec : 0), running ? phaseTotal_ : (p == Phase::Idle ? kWorkSec : 1));
    applyTheme();
    stepper_->setVisible(!running); startHint_->setVisible(!running); go_->setVisible(!running);
    stop_->setVisible(running); roundLabel_->setVisible(running); launch_->setVisible(p == Phase::Work);
    tabs_->tabBar()->setVisible(p != Phase::Work);   // 專注時隱藏分頁，只留圓環與停止鍵
    setWindowFlag(Qt::WindowStaysOnTopHint, p == Phase::Work);
    show();
    if (p == Phase::Work) { tabs_->setCurrentIndex(0); showFullScreen(); } else showNormal();
    paused_ = false; lockUi_ = LockUi::None;
    if (p == Phase::Work) { raise_.start(); QTimer::singleShot(300, this, [this] { engageLock(0); }); }   // 等視窗真的顯示後再鎖
    else { raise_.stop(); lock_->release(); }
    updateTexts(); updateGuard(); updateLock();
    switch (p) {
    case Phase::Prep: announcer_.say(T("speech.prep")); break;
    case Phase::Work: announcer_.say(T("speech.work")); break;
    case Phase::Rest: announcer_.say(T("speech.rest")); break;
    case Phase::Finished: announcer_.say(T("speech.finished")); break;
    case Phase::Voided: announcer_.say(T("speech.voided")); break;
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
    if (lock_->engage(winId())) { lockUi_ = LockUi::Active; updateLock(); }
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
    lockUi_ = LockUi::Paused; updateLock();
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
