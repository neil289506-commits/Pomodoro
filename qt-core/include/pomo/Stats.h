#pragma once
#include <QJsonArray>
namespace pomo {
struct Stats { int total = 0, success = 0, fail = 0, streak = 0, bestStreak = 0; };
Stats computeStats(const QJsonArray& history);  // 連勝 = 連續成功的番茄鐘，失敗即歸零
}
