#include "pomo/Announcer.h"
#include "pomo/Config.h"
namespace pomo {
Announcer::Announcer(QObject* p) : QObject(p), vol_(createPlatformVolume()) {}
void Announcer::say(const QString& text) {
    if (vol_) vol_->setPercent(kVolumePercent);
    tts_.say(text);
}
}
