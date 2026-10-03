package org.pomo
import android.app.Activity
import android.os.Bundle
import android.widget.Button
import android.widget.LinearLayout
import android.widget.TextView
class MainActivity : Activity(), SessionListener {
    private lateinit var label: TextView
    private lateinit var announcer: Announcer
    private lateinit var history: History
    private lateinit var focus: FocusMode
    private lateinit var guardian: Guardian
    private lateinit var watcher: ForegroundWatcher
    private val session = Session(this)
    override fun onCreate(b: Bundle?) {
        super.onCreate(b)
        announcer = Announcer(this); history = History(this); focus = FocusMode(this)
        guardian = Guardian(emptySet(), packageName) { session.voidCurrent(it) }
        watcher = ForegroundWatcher(this, guardian)
        label = TextView(this).apply { textSize = 48f; text = "準備好了嗎？" }
        val go = Button(this).apply { text = "開始 4 個番茄鐘（先準備 3 分鐘）"; setOnClickListener { session.start(4) } }
        val stop = Button(this).apply { text = "停止（番茄鐘作廢）"; setOnClickListener { session.stop() } }
        setContentView(LinearLayout(this).apply { orientation = LinearLayout.VERTICAL; addView(label); addView(go); addView(stop) })
    }
    override fun onTick(remainingSec: Int) { label.text = "%02d:%02d".format(remainingSec / 60, remainingSec % 60) }
    override fun onDone(index: Int) = history.record(index, true)
    override fun onVoided(index: Int, reason: String) = history.record(index, false, reason)
    override fun onPhase(p: Phase) {
        guardian.active = p == Phase.WORK
        if (p == Phase.WORK) { focus.enable(); watcher.start() } else { focus.disable(); watcher.stop() }
        when (p) {
            Phase.PREP -> announcer.say("準備開始，請整理好工作環境")
            Phase.WORK -> announcer.say("開始專注")
            Phase.REST -> announcer.say("番茄鐘完成，請休息")
            Phase.FINISHED -> announcer.say("全部完成")
            Phase.VOIDED -> announcer.say("番茄鐘已作廢")
            else -> {}
        }
    }
}
