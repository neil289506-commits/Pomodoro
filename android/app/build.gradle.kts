plugins { id("com.android.application"); id("org.jetbrains.kotlin.android") }
android {
    namespace = "org.pomo"
    compileSdk = 34
    defaultConfig {
        applicationId = "org.pomo.pomodoro"; minSdk = 26; targetSdk = 34; versionCode = 1; versionName = "0.2.0"
        ndk { abiFilters += listOf("arm64-v8a", "armeabi-v7a", "x86_64", "x86") }  // 日後加入原生程式碼時生效
    }
    compileOptions { sourceCompatibility = JavaVersion.VERSION_17; targetCompatibility = JavaVersion.VERSION_17 }
    kotlinOptions { jvmTarget = "17" }
}
