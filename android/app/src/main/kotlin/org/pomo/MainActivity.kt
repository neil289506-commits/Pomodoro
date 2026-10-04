package org.pomo
import android.app.Activity
import android.app.AlertDialog
import android.app.AppOpsManager
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Bundle
import android.os.Process
import android.provider.Settings
import android.view.View
import android.widget.FrameLayout
import android.widget.LinearLayout
import android.widget.ScrollView
import android.widget.Toast
import org.json.JSONObject
class MainActivity : Activity(), SessionListener {
    private lateinit var announcer: Announcer
    private lateinit var history: History
    private lateinit var focus: FocusMode
    private lateinit var guardian: Guardian
    private lateinit var watcher: ForegroundWatcher
    private val session = Session(this)
    private val prefs by lazy { getSharedPreferences("pomo", Context.MODE_PRIVATE) }
    private var tab = 0
    private var phase = Phase.IDLE
    private var count = Config.DEFAULT_ROUNDS
    private var planned = 0
    private var done = 0
    private var remaining = Config.WORK_SEC
    private var total = Config.WORK_SEC
    private var leaves = 0
    private var paused = false
    private var ring: RingView? = null

    override fun attachBaseContext(base: Context) = super.attachBaseContext(I18n.wrap(base))

    override fun onCreate(b: Bundle?) {
        super.onCreate(b)
        announcer = Announcer(this); history = History(this); focus = FocusMode(this)
        guardian = Guardian(allowed(), packageName) { session.voidCurrent(it) }
        guardian.onLeave = { n -> leaves = n; build() }
        watcher = ForegroundWatcher(this, guardian)
        count = prefs.getInt("rounds", Config.DEFAULT_ROUNDS).coerceIn(1, Config.MAX_ROUNDS)
        build()
    }

    override fun onResume() {
        super.onResume()
        if (paused && phase == Phase.WORK) { paused = false; focus.pin(this) }   // 從放行 App 回來：重新固定螢幕
        build()
    }

    // ---------- 狀態
    private fun allowed(): Set<String> = prefs.getStringSet("allowedApps", emptySet()) ?: emptySet()
    private fun saveAllowed(s: Set<String>) { prefs.edit().putStringSet("allowedApps", s).apply(); guardian.allowed = s }
    private fun appLabel(pkg: String): String = try { packageManager.getApplicationLabel(packageManager.getApplicationInfo(pkg, 0)).toString() } catch (_: Exception) { pkg }
    private fun phaseName(p: Phase) = getString(when (p) {
        Phase.PREP -> R.string.phase_prep; Phase.WORK -> R.string.phase_work; Phase.REST -> R.string.phase_rest
        Phase.FINISHED -> R.string.phase_finished; Phase.VOIDED -> R.string.phase_voided; else -> R.string.phase_idle })
    private val running get() = phase == Phase.PREP || phase == Phase.WORK || phase == Phase.REST

    // ---------- Session 回呼
    override fun onTick(remainingSec: Int) {
        remaining = remainingSec
        ring?.set(phase, remaining, total, Theme.palette(this, phase), phaseName(phase))
    }
    override fun onDone(index: Int) { done = index; history.record(index, true) }
    override fun onVoided(index: Int, reason: String) { history.record(index, false, reason) }
    override fun onPhase(p: Phase) {
        phase = p; paused = false
        guardian.active = p == Phase.WORK
        if (p == Phase.WORK) { leaves = 0; focus.enable(this); watcher.start() } else { focus.disable(this); watcher.stop() }
        total = when (p) {
            Phase.PREP -> Config.PREP_SEC
            Phase.WORK -> Config.WORK_SEC
            Phase.REST -> if (done % Config.LONG_REST_EVERY == 0) Config.LONG_REST_SEC else Config.SHORT_REST_SEC
            else -> 1
        }
        remaining = if (running) total else if (p == Phase.IDLE) Config.WORK_SEC else 0
        build()
        when (p) {
            Phase.PREP -> announcer.say(getString(R.string.speech_prep))
            Phase.WORK -> announcer.say(getString(R.string.speech_work))
            Phase.REST -> announcer.say(getString(R.string.speech_rest))
            Phase.FINISHED -> announcer.say(getString(R.string.speech_finished))
            Phase.VOIDED -> announcer.say(getString(R.string.speech_voided))
            else -> {}
        }
    }

    // ---------- 畫面
    private fun build() {
        val pal = Theme.palette(this, phase)
        window.statusBarColor = pal.bg; window.navigationBarColor = pal.bg
        @Suppress("DEPRECATION")
        window.decorView.systemUiVisibility = if (pal.isDark) 0 else (View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR or View.SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR)
        val root = LinearLayout(this).apply { orientation = LinearLayout.VERTICAL; setBackgroundColor(pal.bg) }
        val content = FrameLayout(this)
        content.addView(when (if (phase == Phase.WORK) 0 else tab) { 0 -> timerPage(pal); 1 -> historyPage(pal); else -> settingsPage(pal) })
        root.add(content, 0, MATCH, 0, 1f)
        if (phase != Phase.WORK) root.add(tabBar(pal))   // 專注時隱藏分頁，只留圓環與停止鍵
        setContentView(root)
    }
    private fun tabBar(pal: Theme.Palette): View {
        val accent = Theme.phaseColor(Phase.IDLE)
        val names = listOf(R.string.tab_timer, R.string.tab_history, R.string.tab_settings)
        return LinearLayout(this).apply {
            orientation = LinearLayout.HORIZONTAL; setBackgroundColor(pal.bg)
            names.forEachIndexed { i, res ->
                val sel = i == tab
                val t = label(getString(res), 14f, if (sel) accent else pal.muted, sel, true)
                t.setPadding(0, dp(14), 0, dp(14))
                t.background = shape(0, 0f, 0, 0)
                t.setOnClickListener { tab = i; build() }
                add(t, 0, 0, WRAP, 1f)
            }
        }
    }
    private fun timerPage(pal: Theme.Palette): View {
        val accent = Theme.phaseColor(phase)
        val page = LinearLayout(this).apply { orientation = LinearLayout.VERTICAL; setPadding(dp(24), dp(20), dp(24), dp(12)) }
        if (running) {
            val idx = if (phase == Phase.REST) done else minOf(done + 1, planned)
            page.add(label(getString(R.string.timer_round, maxOf(1, idx), planned), 20f, pal.text, true, true))
        }
        page.add(label(when (phase) {
            Phase.PREP -> getString(R.string.timer_prep_hint); Phase.REST -> getString(R.string.timer_rest_hint)
            Phase.FINISHED -> getString(R.string.timer_done_hint); Phase.VOIDED -> getString(R.string.timer_voided_hint)
            Phase.IDLE -> getString(R.string.timer_idle_hint); else -> "" }, 13f, pal.muted, false, true), 4)
        val r = RingView(this); ring = r
        r.set(phase, remaining, total, pal, phaseName(phase))
        page.add(r, 8, MATCH, 0, 1f)
        if (!running) {
            page.add(label(getString(R.string.timer_pomodoros), 13f, pal.muted, false, true), 8)
            val row = LinearLayout(this).apply { orientation = LinearLayout.HORIZONTAL; gravity = android.view.Gravity.CENTER }
            fun round(sign: String, delta: Int, enabled: Boolean) = button(sign, pal, accent, false) {
                count = (count + delta).coerceIn(1, Config.MAX_ROUNDS); build()
            }.apply { isEnabled = enabled; textSize = 22f; minWidth = dp(52); minimumWidth = dp(52); minHeight = dp(52); setPadding(0, 0, 0, 0) }
            row.addView(round("−", -1, count > 1), LinearLayout.LayoutParams(dp(52), dp(52)))
            val num = label(count.toString(), 28f, pal.text, true, true); num.minWidth = dp(80)
            row.addView(num)
            row.addView(round("+", 1, count < Config.MAX_ROUNDS), LinearLayout.LayoutParams(dp(52), dp(52)))
            page.add(row, 4)
            page.add(label(getString(R.string.timer_start_hint), 13f, pal.muted, false, true), 8)
            page.add(button(getString(R.string.timer_start), pal, accent, true) { startSession() }, 10)
        } else {
            page.add(button(getString(R.string.timer_stop), pal, accent, false) { confirmStop() }, 10)
            if (phase == Phase.WORK) page.add(button(getString(R.string.timer_launch_plain), pal, accent, false) { openAllowed() }, 8)
        }
        page.add(label(if (phase == Phase.WORK) getString(R.string.guard_leaves, leaves, Config.MAX_LEAVES) else getString(R.string.guard_idle), 13f, pal.muted, false, true), 10)
        return page
    }
    private fun startSession() {
        prefs.edit().putInt("rounds", count).apply()
        planned = count; done = 0
        session.start(count)
    }
    private fun confirmStop() {
        AlertDialog.Builder(this).setTitle(R.string.confirm_title)
            .setMessage(if (phase == Phase.REST) R.string.confirm_rest else R.string.confirm_work)
            .setNegativeButton(R.string.confirm_keep, null)
            .setPositiveButton(R.string.confirm_stop) { _, _ -> session.stop() }.show()
    }
    private fun openAllowed() {
        val pkgs = allowed().toList()
        if (pkgs.isEmpty()) { Toast.makeText(this, R.string.launcher_empty, Toast.LENGTH_LONG).show(); return }
        AlertDialog.Builder(this).setTitle(R.string.launcher_title)
            .setItems(pkgs.map { appLabel(it) }.toTypedArray()) { _, i ->
                val intent = packageManager.getLaunchIntentForPackage(pkgs[i])
                if (intent == null) Toast.makeText(this, R.string.launcher_failed, Toast.LENGTH_LONG).show()
                else { paused = true; focus.unpin(this); startActivity(intent) }   // 暫停硬鎖，離開後回到本 App 會重新固定
            }.show()
    }

    // ---------- 紀錄頁
    private fun historyPage(pal: Theme.Palette): View {
        val rows = history.load()
        val ok = rows.count { it.optString("result") == "success" }
        var streak = 0; var best = 0
        rows.forEach { if (it.optString("result") == "success") { streak++; best = maxOf(best, streak) } else streak = 0 }
        val page = LinearLayout(this).apply { orientation = LinearLayout.VERTICAL; setPadding(dp(20), dp(20), dp(20), dp(12)) }
        val cards = LinearLayout(this).apply { orientation = LinearLayout.HORIZONTAL }
        listOf(ok to R.string.history_completed, (rows.size - ok) to R.string.history_voided, best to R.string.history_best_streak).forEachIndexed { i, (n, cap) ->
            val c = card(pal)
            c.add(label(n.toString(), 28f, pal.text, true, true)); c.add(label(getString(cap), 12f, pal.muted, false, true), 2)
            cards.addView(c, LinearLayout.LayoutParams(0, WRAP, 1f).apply { if (i > 0) leftMargin = dp(10) })
        }
        page.add(cards)
        if (rows.isEmpty()) {
            page.add(label(getString(R.string.history_empty), 14f, pal.muted, false, true), 40)
        } else {
            val list = LinearLayout(this).apply { orientation = LinearLayout.VERTICAL }
            rows.reversed().forEach { o -> list.add(historyRow(o, pal), 8) }
            page.add(ScrollView(this).apply { addView(list) }, 14, MATCH, 0, 1f)
        }
        return page
    }
    private fun historyRow(o: JSONObject, pal: Theme.Palette): View {
        val success = o.optString("result") == "success"
        val reason = when (val r = o.optString("reason")) {
            "manual" -> getString(R.string.reason_manual)
            "leaves" -> getString(R.string.reason_leaves, Config.MAX_LEAVES)
            else -> r
        }
        val c = card(pal)
        c.add(label((if (success) "✓  " else "✕  ") + getString(R.string.history_index, o.optInt("index")), 15f, if (success) 0xFF30A46C.toInt() else 0xFFE5484D.toInt(), true))
        val detail = o.optString("time").replace('T', ' ').take(16) + (if (!success && reason.isNotEmpty()) "  ·  $reason" else "")
        c.add(label(detail, 12f, pal.muted), 2)
        return c
    }

    // ---------- 設定頁
    private fun usageGranted(): Boolean {
        val ops = getSystemService(Context.APP_OPS_SERVICE) as AppOpsManager
        @Suppress("DEPRECATION")
        return ops.checkOpNoThrow(AppOpsManager.OPSTR_GET_USAGE_STATS, Process.myUid(), packageName) == AppOpsManager.MODE_ALLOWED
    }
    private fun settingsPage(pal: Theme.Palette): View {
        val accent = Theme.phaseColor(Phase.IDLE)
        val page = LinearLayout(this).apply { orientation = LinearLayout.VERTICAL; setPadding(dp(20), dp(20), dp(20), dp(20)) }
        // 語言
        val langs = I18n.languages(this)
        val current = I18n.preference(this)
        val langCard = card(pal)
        langCard.add(label(getString(R.string.settings_language), 16f, pal.text, true))
        val currentName = if (current == "auto") getString(R.string.settings_language_auto) else langs.firstOrNull { it.first == current }?.second ?: current
        langCard.add(button(currentName, pal, accent, false) {
            val names = listOf(getString(R.string.settings_language_auto)) + langs.map { it.second }
            AlertDialog.Builder(this).setTitle(R.string.settings_language).setItems(names.toTypedArray()) { _, i ->
                I18n.setPreference(this, if (i == 0) "auto" else langs[i - 1].first); recreate()
            }.show()
        }, 10)
        page.add(langCard)
        // 放行 App
        val apps = card(pal)
        apps.add(label(getString(R.string.settings_apps_title), 16f, pal.text, true))
        apps.add(label(getString(R.string.settings_apps_hint), 12f, pal.muted), 4)
        allowed().sorted().forEach { pkg ->
            val row = LinearLayout(this).apply { orientation = LinearLayout.HORIZONTAL; gravity = android.view.Gravity.CENTER_VERTICAL }
            row.add(label(appLabel(pkg), 15f, pal.text), 0, 0, WRAP, 1f)
            row.add(button("✕", pal, accent, false) { saveAllowed(allowed() - pkg); build() }.apply { minWidth = dp(48); minimumWidth = dp(48); minHeight = dp(40); minimumHeight = dp(40) }, 0, WRAP, WRAP)
            apps.add(row, 6)
        }
        apps.add(button(getString(R.string.settings_add), pal, accent, false) { pickApp() }, 10)
        page.add(apps, 14)
        // 權限
        val perms = card(pal)
        perms.add(label(getString(R.string.settings_permissions), 16f, pal.text, true))
        fun permRow(text: Int, granted: Boolean, action: String) {
            perms.add(label((if (granted) "✓  " else "•  ") + getString(text), 13f, if (granted) 0xFF30A46C.toInt() else pal.muted), 10)
            if (!granted) perms.add(button(getString(R.string.settings_perm_open), pal, accent, false) { startActivity(Intent(action)) }, 6)
        }
        permRow(R.string.settings_perm_usage, usageGranted(), Settings.ACTION_USAGE_ACCESS_SETTINGS)
        permRow(R.string.settings_perm_dnd, focus.dndGranted(), Settings.ACTION_NOTIFICATION_POLICY_ACCESS_SETTINGS)
        page.add(perms, 14)
        // 關於
        val about = card(pal)
        about.add(label(getString(R.string.settings_about), 16f, pal.text, true))
        about.add(label(getString(R.string.app_name), 20f, pal.text, true), 8)
        about.add(label(getString(R.string.about_tagline), 13f, pal.muted), 2)
        about.add(label(getString(R.string.settings_version, packageManager.getPackageInfo(packageName, 0).versionName), 13f, pal.muted), 2)
        page.add(about, 14)
        return ScrollView(this).apply { addView(page) }
    }
    private fun pickApp() {
        val pm = packageManager
        val launcher = Intent(Intent.ACTION_MAIN).addCategory(Intent.CATEGORY_LAUNCHER)
        val apps = pm.queryIntentActivities(launcher, 0).map { it.activityInfo.packageName to it.loadLabel(pm).toString() }
            .filter { it.first != packageName && it.first !in allowed() }.distinctBy { it.first }.sortedBy { it.second.lowercase() }
        AlertDialog.Builder(this).setTitle(R.string.settings_apps_title)
            .setItems(apps.map { it.second }.toTypedArray()) { _, i -> saveAllowed(allowed() + apps[i].first); build() }.show()
    }
}
