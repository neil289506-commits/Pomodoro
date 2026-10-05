package org.pomo
import android.content.Context
import android.graphics.Typeface
import android.graphics.drawable.GradientDrawable
import android.view.Gravity
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.LinearLayout
import android.widget.TextView
const val MATCH = ViewGroup.LayoutParams.MATCH_PARENT
const val WRAP = ViewGroup.LayoutParams.WRAP_CONTENT
fun Context.dp(v: Int): Int = (v * resources.displayMetrics.density + 0.5f).toInt()
fun Context.shape(color: Int, radiusDp: Float, strokeColor: Int = 0, strokeDp: Int = 0) = GradientDrawable().apply {
    setColor(color); cornerRadius = radiusDp * resources.displayMetrics.density
    if (strokeDp > 0) setStroke(dp(strokeDp), strokeColor)
}
fun Context.label(content: String, sizeSp: Float, color: Int, bold: Boolean = false, center: Boolean = false) = TextView(this).apply {
    text = content; textSize = sizeSp; setTextColor(color)
    if (bold) setTypeface(typeface, Typeface.BOLD)
    if (center) gravity = Gravity.CENTER
}
fun Context.button(content: String, pal: Theme.Palette, accent: Int, primary: Boolean, onClick: () -> Unit) = Button(this).apply {
    text = content; isAllCaps = false; textSize = if (primary) 17f else 15f
    setTypeface(typeface, if (primary) Typeface.BOLD else Typeface.NORMAL)
    setTextColor(if (primary) 0xFFFFFFFF.toInt() else pal.text)
    background = shape(if (primary) accent else 0, 12f, pal.border, if (primary) 0 else 1)
    stateListAnimator = null; minHeight = dp(52); minimumHeight = dp(52)
    setOnClickListener { onClick() }
}
fun Context.card(pal: Theme.Palette): LinearLayout = LinearLayout(this).apply {
    orientation = LinearLayout.VERTICAL; setPadding(dp(16), dp(14), dp(16), dp(14))
    background = shape(pal.surface, 14f, pal.border, 1)
}
fun LinearLayout.add(v: View, topDp: Int = 0, w: Int = MATCH, h: Int = WRAP, weight: Float = 0f) {
    addView(v, LinearLayout.LayoutParams(w, h, weight).apply { topMargin = context.dp(topDp) })
}
