#include "pomo/Session.h"
#include "pomo/Config.h"
namespace pomo {
Session::Session(QObject* p) : QObject(p) { t_.setInterval(250); connect(&t_, &QTimer::timeout, this, &Session::onTick); }
void Session::begin(Phase ph, int sec) {
    phase_ = ph; end_ = QDateTime::currentDateTime().addSecs(sec); t_.start();
    emit phaseChanged(ph); emit ticked(sec);
}
void Session::start(int total) { total_ = total; done_ = 0; begin(Phase::Prep, kPrepSec); }
void Session::voidCurrent(const QString& reason) {
    if (phase_ != Phase::Prep && phase_ != Phase::Work) return;
    t_.stop(); phase_ = Phase::Voided;
    emit pomodoroVoided(done_ + 1, reason); emit phaseChanged(phase_);
}
void Session::stop() {
    if (phase_ == Phase::Prep || phase_ == Phase::Work) { voidCurrent(QStringLiteral("manual")); return; }
    if (phase_ == Phase::Rest) { t_.stop(); phase_ = Phase::Finished; emit phaseChanged(phase_); }
}
void Session::onTick() {
    const int left = QDateTime::currentDateTime().secsTo(end_);
    if (left > 0) { emit ticked(left); return; }
    switch (phase_) {
    case Phase::Prep: begin(Phase::Work, kWorkSec); break;
    case Phase::Work:
        emit pomodoroDone(++done_);
        if (done_ >= total_) { t_.stop(); phase_ = Phase::Finished; emit phaseChanged(phase_); }
        else begin(Phase::Rest, done_ % kLongRestEvery == 0 ? kLongRestSec : kShortRestSec);
        break;
    case Phase::Rest: begin(Phase::Work, kWorkSec); break;
    default: break;
    }
}
}
