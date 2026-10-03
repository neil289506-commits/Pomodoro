package org.pomo
import android.content.Context
import org.json.JSONObject
import java.io.File
import java.time.LocalDateTime
/** 紀錄每個番茄鐘成功 / 失敗（JSON Lines）。 */
class History(ctx: Context) {
    private val f = File(ctx.filesDir, "history.jsonl")
    fun record(index: Int, success: Boolean, reason: String = "") {
        val o = JSONObject().put("time", LocalDateTime.now().toString()).put("index", index)
            .put("result", if (success) "success" else "fail").put("reason", reason)
        f.appendText(o.toString() + "\n")
    }
    fun load(): List<JSONObject> = if (f.exists()) f.readLines().map { JSONObject(it) } else emptyList()
}
