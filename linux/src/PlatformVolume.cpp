#include <QProcess>
#include "pomo/Announcer.h"
namespace pomo {
struct LinuxVolume : IVolume {
    bool setPercent(int p) override {
        const QString percent = QString::number(p) + "%";
        if (QProcess::execute("pactl", {"set-sink-volume", "@DEFAULT_SINK@", percent}) == 0) return true;
        if (QProcess::execute("amixer", {"sset", "Master", percent}) == 0) return true;
        return QProcess::execute("wpctl", {"set-volume", "@DEFAULT_AUDIO_SINK@", QString::number(p / 100.0, 'f', 2)}) == 0;
    }
};
std::unique_ptr<IVolume> createPlatformVolume() { return std::make_unique<LinuxVolume>(); }
}
