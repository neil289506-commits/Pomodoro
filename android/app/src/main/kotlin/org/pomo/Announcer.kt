package org.pomo
import android.content.Context
import android.media.AudioManager
import android.speech.tts.TextToSpeech
import java.util.Locale
/** 語音提醒：先把音量拉到 50%，語音語言跟隨介面語言。 */
class Announcer(private val ctx: Context) {
    private val am = ctx.getSystemService(Context.AUDIO_SERVICE) as AudioManager
    private val tts: TextToSpeech
    private var ttsReady = false
    init {
        tts = TextToSpeech(ctx) { status ->
            ttsReady = status == TextToSpeech.SUCCESS
            if (ttsReady) applyLocale()
        }
    }
    private fun applyLocale() {
        val locale = ctx.resources.configuration.locales[0]
        val r = tts.setLanguage(locale)
        if (r == TextToSpeech.LANG_MISSING_DATA || r == TextToSpeech.LANG_NOT_SUPPORTED) tts.setLanguage(Locale.getDefault())
    }
    fun say(text: String) {
        if (!ttsReady) return
        applyLocale()   // 使用者可能剛切換過語言
        val max = am.getStreamMaxVolume(AudioManager.STREAM_MUSIC)
        am.setStreamVolume(AudioManager.STREAM_MUSIC, max * Config.VOLUME_PERCENT / 100, 0)
        tts.speak(text, TextToSpeech.QUEUE_FLUSH, null, "pomo")
    }
}
