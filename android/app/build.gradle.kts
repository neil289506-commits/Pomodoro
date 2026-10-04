import groovy.json.JsonSlurper

plugins { id("com.android.application"); id("org.jetbrains.kotlin.android") }

// 由 repo 根目錄的 i18n/strings.json 產生 Android 的 values-<語言>/strings.xml（與桌面版、iOS 共用同一份字串）
abstract class GenerateI18n : DefaultTask() {
    @get:InputFile abstract val catalog: RegularFileProperty
    @get:OutputDirectory abstract val outDir: DirectoryProperty

    private fun esc(s: String): String {
        var r = s.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
            .replace("'", "\\'").replace("\"", "\\\"").replace("\n", "\\n")
        if (r.startsWith("@") || r.startsWith("?")) r = "\\" + r
        return Regex("%(\\d)").replace(r) { "%" + it.groupValues[1] + "\$s" }   // %1 -> %1$s
    }

    @TaskAction
    fun generate() {
        @Suppress("UNCHECKED_CAST")
        val root = JsonSlurper().parse(catalog.get().asFile, "UTF-8") as Map<String, Any>
        @Suppress("UNCHECKED_CAST")
        val langs = root["languages"] as List<Map<String, String>>
        @Suppress("UNCHECKED_CAST")
        val strings = root["strings"] as Map<String, Map<String, String>>
        val out = outDir.get().asFile
        out.deleteRecursively()
        for (lang in langs) {
            val code = lang["code"]!!
            // zh-TW -> values-zh-rTW；繁體中文也提供給香港、澳門
            val qualifiers = when (code) {
                "en" -> listOf("values")
                "zh-TW" -> listOf("values-zh-rTW", "values-zh-rHK", "values-zh-rMO")
                "zh-CN" -> listOf("values-zh-rCN")
                else -> listOf("values-$code")
            }
            val xml = StringBuilder("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<resources>\n")
            if (code == "en") {
                xml.append("  <string name=\"app_name\">TomatoGuard</string>\n")
                xml.append("  <string-array name=\"language_codes\">\n")
                langs.forEach { xml.append("    <item>${it["code"]}</item>\n") }
                xml.append("  </string-array>\n  <string-array name=\"language_names\">\n")
                langs.forEach { xml.append("    <item>${esc(it["name"]!!)}</item>\n") }
                xml.append("  </string-array>\n")
            }
            for ((key, tr) in strings) {
                val text = tr[code] ?: tr["en"]!!
                xml.append("  <string name=\"${key.replace('.', '_')}\">${esc(text)}</string>\n")
            }
            xml.append("</resources>\n")
            for (q in qualifiers) {
                File(out, q).mkdirs()
                File(out, "$q/strings.xml").writeText(xml.toString(), Charsets.UTF_8)
            }
        }
    }
}

val generateI18n = tasks.register<GenerateI18n>("generateI18n") {
    catalog.set(rootProject.layout.projectDirectory.file("../i18n/strings.json"))
}

android {
    namespace = "org.pomo"
    compileSdk = 34
    defaultConfig {
        applicationId = "org.pomo.pomodoro"; minSdk = 26; targetSdk = 34; versionCode = 2; versionName = "0.3.0"
        ndk { abiFilters += listOf("arm64-v8a", "armeabi-v7a", "x86_64", "x86") }  // 日後加入原生程式碼時生效
    }
    compileOptions { sourceCompatibility = JavaVersion.VERSION_17; targetCompatibility = JavaVersion.VERSION_17 }
    kotlinOptions { jvmTarget = "17" }
}

androidComponents {
    onVariants { variant ->
        variant.sources.res?.addGeneratedSourceDirectory(generateI18n, GenerateI18n::outDir)
    }
}
