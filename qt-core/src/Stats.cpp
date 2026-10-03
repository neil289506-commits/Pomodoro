#include "pomo/Stats.h"
#include <QJsonObject>
#include <algorithm>
namespace pomo {
Stats computeStats(const QJsonArray& h) {
    Stats s;
    for (const auto& v : h) {
        ++s.total;
        if (v.toObject().value("result").toString() == "success") { ++s.success; s.bestStreak = std::max(s.bestStreak, ++s.streak); }
        else { ++s.fail; s.streak = 0; }
    }
    return s;
}
}
