package org.pomo;
import java.util.Set;
import java.util.function.Consumer;
/** 專注守護：離開超過 MAX_LEAVES 次即觸發 onViolated。iOS 只能偵測本 App 進入背景。 */
public class Guardian {
    private final Consumer<String> onViolated;
    private Set<String> allowed = Set.of();
    private boolean active, away;
    private int leaves;
    private java.util.function.IntConsumer onLeave = n -> {};
    public void setOnLeave(java.util.function.IntConsumer c) { onLeave = c; }
    public int leaves() { return leaves; }
    public Guardian(Consumer<String> onViolated) { this.onViolated = onViolated; }
    public void setAllowed(Set<String> ids) { allowed = ids; }
    public void setActive(boolean on) { active = on; leaves = 0; away = false; }
    public void onForeground(String appId, boolean isSelf) {
        if (!active) return;
        if (isSelf || allowed.contains(appId)) { away = false; return; }
        if (away) return;
        away = true; leaves++; onLeave.accept(leaves);
        if (leaves > Config.MAX_LEAVES) onViolated.accept("leaves");
    }
}
