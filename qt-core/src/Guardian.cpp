#include "pomo/Guardian.h"
#include "pomo/Config.h"
namespace pomo {
void Guardian::setAllowedApps(const QSet<QString>& ids) {
    allowed_.clear();
    for (const QString& id : ids) allowed_.insert(id.trimmed().toLower());
}
void Guardian::onForegroundChanged(const QString& appId, bool isSelf) {
    if (!active_) return;
    const bool ok = isSelf || allowed_.contains(appId.trimmed().toLower());
    if (ok) { away_ = false; return; }
    if (away_) return;  // 同一次離開只算一次
    away_ = true; emit leaveCounted(++leaves_);
    if (leaves_ > kMaxLeaves) emit violated(QStringLiteral("離開視窗超過 %1 次").arg(kMaxLeaves));
}
}
