package org.pomo
import android.os.Handler
import android.os.Looper
enum class Phase { IDLE, PREP, WORK, REST, FINISHED, VOIDED }
interface SessionListener {
    fun onPhase(p: Phase) {}
    fun onTick(remainingSec: Int) {}
    fun onDone(index: Int) {}
    fun onVoided(index: Int, reason: String) {}
}
/** 番茄鐘流程：準備 3 分 -> (專注 -> 休息) x N，每個番茄鐘不可切割。 */
class Session(private val l: SessionListener) {
    var phase = Phase.IDLE; private set
    private var total = 0; private var done = 0; private var endAt = 0L
    private val h = Handler(Looper.getMainLooper())
    private val tick = object : Runnable { override fun run() { onTick(); if (phase.isRunning()) h.postDelayed(this, 250) } }
    private fun Phase.isRunning() = this == Phase.PREP || this == Phase.WORK || this == Phase.REST
    fun start(n: Int) { total = n; done = 0; begin(Phase.PREP, Config.PREP_SEC) }
    private fun begin(p: Phase, sec: Int) { phase = p; endAt = System.currentTimeMillis() + sec * 1000L; l.onPhase(p); h.removeCallbacks(tick); h.post(tick) }
    fun voidCurrent(reason: String) {
        if (phase != Phase.PREP && phase != Phase.WORK) return
        phase = Phase.VOIDED; l.onVoided(done + 1, reason); l.onPhase(phase)
    }
    fun stop() { if (phase == Phase.REST) { phase = Phase.FINISHED; l.onPhase(phase) } else voidCurrent("手動停止") }
    private fun onTick() {
        val left = ((endAt - System.currentTimeMillis()) / 1000).toInt()
        if (left > 0) { l.onTick(left); return }
        when (phase) {
            Phase.PREP, Phase.REST -> begin(Phase.WORK, Config.WORK_SEC)
            Phase.WORK -> { l.onDone(++done)
                if (done >= total) { phase = Phase.FINISHED; l.onPhase(phase) }
                else begin(Phase.REST, if (done % Config.LONG_REST_EVERY == 0) Config.LONG_REST_SEC else Config.SHORT_REST_SEC) }
            else -> {}
        }
    }
}
