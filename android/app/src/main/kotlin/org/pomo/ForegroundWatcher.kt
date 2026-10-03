package org.pomo
import android.app.usage.UsageStatsManager
import android.content.Context
import android.os.Handler
import android.os.Looper
/** 每秒用 UsageStats 查前景 App，餵給 Guardian（需使用者授權「使用情況存取」）。 */
class ForegroundWatcher(private val ctx: Context, private val g: Guardian) {
    private val h = Handler(Looper.getMainLooper())
    private val r = object : Runnable { override fun run() { poll(); h.postDelayed(this, 1000) } }
    fun start() = h.post(r)
    fun stop() = h.removeCallbacks(r)
    private fun poll() {
        val m = ctx.getSystemService(Context.USAGE_STATS_SERVICE) as UsageStatsManager
        val now = System.currentTimeMillis()
        m.queryUsageStats(UsageStatsManager.INTERVAL_DAILY, now - 10_000, now)
            .maxByOrNull { it.lastTimeUsed }?.let { g.onForeground(it.packageName) }
    }
}
