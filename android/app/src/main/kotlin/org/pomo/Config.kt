package org.pomo
object Config {
    const val PREP_SEC = 180; const val WORK_SEC = 1500; const val SHORT_REST_SEC = 300; const val LONG_REST_SEC = 1200
    const val LONG_REST_EVERY = 4   // 每 4 個番茄鐘後長休息
    const val MAX_LEAVES = 3        // 專注守護：離開超過 3 次即作廢
    const val VOLUME_PERCENT = 50   // 提醒時的音量
}
