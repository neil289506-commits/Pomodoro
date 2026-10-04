package org.pomo;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
/** 紀錄每個番茄鐘成功 / 失敗（JSON Lines），原因存代碼（manual / leaves），顯示時再翻譯。 */
public class History {
    public static final class Entry { public String time, result, reason; public int index; public boolean success() { return "success".equals(result); } }
    private static final Pattern LINE = Pattern.compile("\"time\":\"([^\"]*)\",\"index\":(\\d+),\"result\":\"(\\w+)\",\"reason\":\"([^\"]*)\"");
    private final Path file;
    public History(Path dir) { this.file = dir.resolve("history.jsonl"); try { Files.createDirectories(dir); } catch (IOException ignored) {} }
    public void record(int index, boolean success, String reason) {
        String line = String.format("{\"time\":\"%s\",\"index\":%d,\"result\":\"%s\",\"reason\":\"%s\"}%n",
            LocalDateTime.now(), index, success ? "success" : "fail", reason.replace("\"", "'"));
        try { Files.write(file, line.getBytes(StandardCharsets.UTF_8), StandardOpenOption.CREATE, StandardOpenOption.APPEND); }
        catch (IOException ignored) {}
    }
    public List<Entry> load() {
        List<Entry> out = new ArrayList<>();
        try {
            if (!Files.exists(file)) return out;
            for (String l : Files.readAllLines(file, StandardCharsets.UTF_8)) {
                Matcher m = LINE.matcher(l);
                if (m.find()) { Entry e = new Entry(); e.time = m.group(1); e.index = Integer.parseInt(m.group(2)); e.result = m.group(3); e.reason = m.group(4); out.add(e); }
            }
        } catch (IOException ignored) {}
        return out;
    }
}
