package org.pomo
import android.content.Context
import android.graphics.Canvas
import android.graphics.Paint
import android.graphics.RectF
import android.graphics.Typeface
import android.view.View
/** 圓環倒數：顏色隨階段變化，中央顯示剩餘時間與階段名稱。 */
class RingView(context: Context) : View(context) {
    private var phase = Phase.IDLE
    private var rem = 0
    private var tot = 1
    private var pal = Theme.light
    private var name = ""
    private fun stroke() = Paint(Paint.ANTI_ALIAS_FLAG).apply { style = Paint.Style.STROKE; strokeCap = Paint.Cap.ROUND }
    private val track = stroke()
    private val arc = stroke()
    private val timePaint = Paint(Paint.ANTI_ALIAS_FLAG).apply { textAlign = Paint.Align.CENTER; typeface = Typeface.create("sans-serif-light", Typeface.NORMAL) }
    private val namePaint = Paint(Paint.ANTI_ALIAS_FLAG).apply { textAlign = Paint.Align.CENTER; typeface = Typeface.create("sans-serif-medium", Typeface.NORMAL) }
    fun set(p: Phase, remaining: Int, total: Int, palette: Theme.Palette, label: String) {
        phase = p; rem = remaining; tot = if (total > 0) total else 1; pal = palette; name = label; invalidate()
    }
    override fun onDraw(c: Canvas) {
        val side = minOf(width, height) - context.dp(40)
        val w = maxOf(context.dp(10).toFloat(), side / 22f)
        val l = (width - side) / 2f
        val t = (height - side) / 2f
        val rect = RectF(l + w / 2, t + w / 2, l + side - w / 2, t + side - w / 2)
        track.strokeWidth = w; track.color = pal.border
        c.drawArc(rect, 0f, 360f, false, track)
        if (rem > 0) { arc.strokeWidth = w; arc.color = Theme.phaseColor(phase); c.drawArc(rect, -90f, 360f * rem / tot, false, arc) }
        val cy = height / 2f
        timePaint.textSize = side / 4f; timePaint.color = pal.text
        c.drawText("%02d:%02d".format(rem / 60, rem % 60), width / 2f, cy + side * 0.03f, timePaint)
        namePaint.textSize = side / 13f; namePaint.color = Theme.phaseColor(phase)
        c.drawText(name, width / 2f, cy + side * 0.25f, namePaint)
    }
}
