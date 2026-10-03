package org.pomo;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.time.LocalDateTime;
/** 紀錄每個番茄鐘成功 / 失敗（JSON Lines）。 */
public class History {
    private final Path file;
    public History(Path dir) { this.file = dir.resolve("history.jsonl"); }
    public void record(int index, boolean success, String reason) {
        String line = String.format("{\"time\":\"%s\",\"index\":%d,\"result\":\"%s\",\"reason\":\"%s\"}%n",
            LocalDateTime.now(), index, success ? "success" : "fail", reason.replace("\"", "'"));
        try { Files.write(file, line.getBytes(StandardCharsets.UTF_8), StandardOpenOption.CREATE, StandardOpenOption.APPEND); }
        catch (IOException ignored) {}
    }
}
