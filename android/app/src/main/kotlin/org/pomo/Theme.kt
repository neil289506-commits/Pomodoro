package org.pomo
import android.content.Context
import android.content.res.Configuration
/** 與桌面版、iOS 相同的色票：專注中一律深色，其餘時段跟隨系統深色模式。 */
object Theme {
    class Palette(val bg: Int, val surface: Int, val text: Int, val muted: Int, val border: Int, val isDark: Boolean)
    val light = Palette(0xFFFAF7F2.toInt(), 0xFFFFFFFF.toInt(), 0xFF1F1F1F.toInt(), 0xFF6B6B6B.toInt(), 0xFFE6E0D6.toInt(), false)
    val dark = Palette(0xFF121214.toInt(), 0xFF1C1C20.toInt(), 0xFFF2F2F3.toInt(), 0xFF9A9AA2.toInt(), 0xFF2E2E34.toInt(), true)
    fun phaseColor(p: Phase): Int = when (p) {
        Phase.PREP -> 0xFFF5A623
        Phase.WORK -> 0xFFE5484D
        Phase.REST -> 0xFF30A46C
        Phase.FINISHED -> 0xFF3E63DD
        Phase.VOIDED -> 0xFF8B8D98
        else -> 0xFFE5484D
    }.toInt()
    fun palette(ctx: Context, p: Phase): Palette {
        val night = (ctx.resources.configuration.uiMode and Configuration.UI_MODE_NIGHT_MASK) == Configuration.UI_MODE_NIGHT_YES
        return if (p == Phase.WORK || night) dark else light
    }
}
