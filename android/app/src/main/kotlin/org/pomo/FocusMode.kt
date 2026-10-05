package org.pomo
import android.app.Activity
import android.app.NotificationManager
import android.content.Context
/** 專注時段：勿擾 + 螢幕固定（Android 的硬鎖，使用者可長按返回與概覽鍵退出）。 */
class FocusMode(private val ctx: Context) {
    private val nm = ctx.getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
    fun dndGranted() = nm.isNotificationPolicyAccessGranted
    fun enable(activity: Activity) {
        if (dndGranted()) nm.setInterruptionFilter(NotificationManager.INTERRUPTION_FILTER_NONE)
        pin(activity)
    }
    fun disable(activity: Activity) {
        if (dndGranted()) nm.setInterruptionFilter(NotificationManager.INTERRUPTION_FILTER_ALL)
        unpin(activity)
    }
    /** 螢幕固定：系統會跳出確認提示；停用 Home 與最近使用的 App。 */
    fun pin(activity: Activity) { try { activity.startLockTask() } catch (_: Exception) {} }
    fun unpin(activity: Activity) { try { activity.stopLockTask() } catch (_: Exception) {} }
}
