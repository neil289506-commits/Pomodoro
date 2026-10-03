package org.pomo
import android.content.Context
import android.media.AudioManager
import android.speech.tts.TextToSpeech
import java.util.Locale
/** 語音提醒前先把音量拉到 50%。 */
class Announcer(ctx: Context) {
    private val am = ctx.getSystemService(Context.AUDIO_SERVICE) as AudioManager
    private val tts: TextToSpeech
    private var ttsReady = false
    init {
        tts = TextToSpeech(ctx) { status ->
            if (status != TextToSpeech.SUCCESS) return@TextToSpeech
            val r = tts.setLanguage(Locale.TRADITIONAL_CHINESE)
            if (r == TextToSpeech.LANG_MISSING_DATA || r == TextToSpeech.LANG_NOT_SUPPORTED)
                tts.setLanguage(Locale.getDefault())
            ttsReady = true
        }
    }
    fun say(text: String) {
        if (!ttsReady) return
        val max = am.getStreamMaxVolume(AudioManager.STREAM_MUSIC)
        am.setStreamVolume(AudioManager.STREAM_MUSIC, max * Config.VOLUME_PERCENT / 100, 0)
        tts.speak(text, TextToSpeech.QUEUE_FLUSH, null, "pomo")
    }
}
