package org.pomo
import android.content.Context
import android.content.ContextWrapper
import android.content.res.Configuration
import java.util.Locale
/** 介面語言：預設跟隨系統；使用者在設定頁選擇後存起來，下次啟動套用。字串來自 i18n/strings.json。 */
object I18n {
    private const val KEY = "language"
    fun preference(ctx: Context): String = ctx.getSharedPreferences("pomo", Context.MODE_PRIVATE).getString(KEY, "auto") ?: "auto"
    fun setPreference(ctx: Context, value: String) { ctx.getSharedPreferences("pomo", Context.MODE_PRIVATE).edit().putString(KEY, value).apply() }
    fun wrap(base: Context): Context {
        val pref = preference(base)
        if (pref == "auto") return base
        val cfg = Configuration(base.resources.configuration)
        cfg.setLocale(Locale.forLanguageTag(pref))
        return ContextWrapper(base.createConfigurationContext(cfg))
    }
    fun languages(ctx: Context): List<Pair<String, String>> {
        val codes = ctx.resources.getStringArray(R.array.language_codes)
        val names = ctx.resources.getStringArray(R.array.language_names)
        return codes.indices.map { codes[it] to names[it] }
    }
}
