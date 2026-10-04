package org.pomo;
import java.util.List;
import org.robovm.apple.foundation.NSLocale;
import org.robovm.apple.foundation.NSUserDefaults;
/** 介面語言：預設跟隨系統；設定頁選擇後存在 NSUserDefaults。字串來自 i18n/strings.json（Catalog.java 由 Gradle 產生）。 */
public final class I18n {
    private I18n() {}
    public static String[][] languages() { return Catalog.LANGS; }
    public static String preference() {
        String s = NSUserDefaults.getStandardUserDefaults().getString("language");
        return s == null || s.isEmpty() ? "auto" : s;
    }
    public static void setPreference(String p) { NSUserDefaults.getStandardUserDefaults().put("language", p); }
    private static boolean has(String code) { for (String[] l : Catalog.LANGS) if (l[0].equals(code)) return true; return false; }
    /** 實際使用的語言代碼。 */
    public static String language() {
        String pref = preference();
        if (!pref.equals("auto") && has(pref)) return pref;
        List<String> prefs = NSLocale.getPreferredLanguages();
        String tag = prefs != null && !prefs.isEmpty() ? prefs.get(0) : "en";   // 例如 zh-Hant-TW、ja-JP
        String lang = tag.split("[-_]")[0];
        if (lang.equals("zh")) {
            boolean trad = tag.contains("Hant") || tag.endsWith("-TW") || tag.endsWith("-HK") || tag.endsWith("-MO");
            return trad ? "zh-TW" : "zh-CN";
        }
        return has(lang) ? lang : "en";
    }
    /** 語音用的 BCP 47 語言標籤。 */
    public static String speechTag() {
        switch (language()) {
            case "zh-TW": return "zh-TW"; case "zh-CN": return "zh-CN"; case "ja": return "ja-JP";
            case "ko": return "ko-KR"; case "es": return "es-ES"; default: return "en-US";
        }
    }
    public static String t(String key, Object... args) {
        String s = Catalog.lookup(language(), key);
        for (int i = 0; i < args.length; i++) s = s.replace("%" + (i + 1), String.valueOf(args[i]));
        return s;
    }
}
