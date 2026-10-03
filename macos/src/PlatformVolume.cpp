#include <QProcess>
#include "pomo/Announcer.h"
namespace pomo {
struct MacVolume : IVolume {
    bool setPercent(int p) override { return QProcess::execute("osascript", {"-e", QString("set volume output volume %1").arg(p)}) == 0; }
};
std::unique_ptr<IVolume> createPlatformVolume() { return std::make_unique<MacVolume>(); }
}
