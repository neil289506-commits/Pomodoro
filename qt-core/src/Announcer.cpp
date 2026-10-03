#include "pomo/Announcer.h"
#include "pomo/Config.h"
#ifdef POMO_HAS_TTS
#include <QTextToSpeech>
#else
class QTextToSpeech {};  // 沒有 Qt Speech 模組時的佔位型別，讓 unique_ptr 能完整解構
#endif
namespace pomo {
Announcer::Announcer(QObject* p) : QObject(p), vol_(createPlatformVolume()) {
#ifdef POMO_HAS_TTS
    tts_ = std::make_unique<QTextToSpeech>();
#endif
}
Announcer::~Announcer() = default;
void Announcer::say(const QString& text) {
    if (vol_) vol_->setPercent(kVolumePercent);
    if (tts_) tts_->say(text);
}
}
