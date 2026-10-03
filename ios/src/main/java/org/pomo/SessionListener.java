package org.pomo;
public interface SessionListener {
    void onPhase(Phase p);
    void onTick(int remainingSec);
    void onDone(int index);
    void onVoided(int index, String reason);
}
