package org.pomo
import android.content.Context
import android.media.AudioManager
import android.speech.tts.TextToSpeech
import java.util.Locale
/** 語音提醒前先把音量拉到 50%。 */
class Announcer(ctx: Context) {
    private val am = ctx.getSystemService(Context.AUDIO_SERVICE) as AudioManager
    private val tts = TextToSpeech(ctx) { if (it == TextToSpeech.SUCCESS) tts.language = Locale.TRADITIONAL_CHINESE }
    fun say(text: String) {
        val max = am.getStreamMaxVolume(AudioManager.STREAM_MUSIC)
        am.setStreamVolume(AudioManager.STREAM_MUSIC, max * Config.VOLUME_PERCENT / 100, 0)
        tts.speak(text, TextToSpeech.QUEUE_FLUSH, null, "pomo")
    }
}
