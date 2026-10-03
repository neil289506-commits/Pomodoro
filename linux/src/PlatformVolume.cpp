#include <QProcess>
#include "pomo/Announcer.h"
namespace pomo {
struct LinuxVolume : IVolume {  // 使用 PulseAudio / PipeWire 的 pactl
    bool setPercent(int p) override { return QProcess::execute("pactl", {"set-sink-volume", "@DEFAULT_SINK@", QString::number(p) + "%"}) == 0; }
};
std::unique_ptr<IVolume> createPlatformVolume() { return std::make_unique<LinuxVolume>(); }
}
