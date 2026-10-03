package org.pomo
/** 專注守護：離開（前景為非放行 App）超過 MAX_LEAVES 次即觸發 onViolated。 */
class Guardian(var allowed: Set<String>, private val selfPkg: String, private val onViolated: (String) -> Unit) {
    var active = false; set(v) { field = v; leaves = 0; away = false }
    var leaves = 0; private set
    private var away = false
    fun onForeground(pkg: String) {
        if (!active) return
        if (pkg == selfPkg || pkg in allowed) { away = false; return }
        if (away) return
        away = true; leaves++
        if (leaves > Config.MAX_LEAVES) onViolated("離開視窗超過 ${Config.MAX_LEAVES} 次")
    }
}
