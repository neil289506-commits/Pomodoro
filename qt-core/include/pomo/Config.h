#pragma once
namespace pomo {
constexpr int kPrepSec = 180, kWorkSec = 1500, kShortRestSec = 300, kLongRestSec = 1200;
constexpr int kLongRestEvery = 4;   // 每 4 個番茄鐘後長休息
constexpr int kMaxLeaves = 3;       // 專注守護：離開超過 3 次即作廢
constexpr int kVolumePercent = 50;  // 提醒時的系統音量
}
