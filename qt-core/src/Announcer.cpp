#include "pomo/Announcer.h"
#include "pomo/Config.h"
#ifdef POMO_HAS_TTS
#include <QTextToSpeech>
#endif
namespace pomo {
Announcer::Announcer(QObject* p) : QObject(p), vol_(createPlatformVolume()) {
#ifdef POMO_HAS_TTS
    tts_ = std::make_unique<QTextToSpeech>();
#endif
}
void Announcer::say(const QString& text) {
    if (vol_) vol_->setPercent(kVolumePercent);
    if (tts_) tts_->say(text);
}
}
