package org.pomo
import android.app.NotificationManager
import android.content.Context
import android.content.Intent
import android.provider.Settings
/** 專注時段開啟勿擾（需使用者授權勿擾權限）。 */
class FocusMode(private val ctx: Context) {
    private val nm = ctx.getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
    fun enable() {
        if (nm.isNotificationPolicyAccessGranted) nm.setInterruptionFilter(NotificationManager.INTERRUPTION_FILTER_NONE)
        else ctx.startActivity(Intent(Settings.ACTION_NOTIFICATION_POLICY_ACCESS_SETTINGS).addFlags(Intent.FLAG_ACTIVITY_NEW_TASK))
    }
    fun disable() { if (nm.isNotificationPolicyAccessGranted) nm.setInterruptionFilter(NotificationManager.INTERRUPTION_FILTER_ALL) }
}
