package org.pomo;
import java.util.Timer;
import java.util.TimerTask;
/** 番茄鐘流程：準備 3 分 -> (專注 -> 休息) x N，每個番茄鐘不可切割。（回呼在 Timer 執行緒，UI 請自行切回主執行緒） */
public class Session {
    private final SessionListener l;
    private Phase phase = Phase.IDLE;
    private int total, done;
    private long endAt;
    private Timer timer;
    public Session(SessionListener l) { this.l = l; }
    public synchronized Phase phase() { return phase; }
    public synchronized void start(int n) { total = n; done = 0; begin(Phase.PREP, Config.PREP_SEC); }
    private void begin(Phase p, int sec) {
        phase = p; endAt = System.currentTimeMillis() + sec * 1000L; l.onPhase(p);
        if (timer != null) timer.cancel();
        timer = new Timer(true);
        timer.schedule(new TimerTask() { public void run() { tick(); } }, 0, 250);
    }
    public synchronized void voidCurrent(String reason) {
        if (phase != Phase.PREP && phase != Phase.WORK) return;
        timer.cancel(); phase = Phase.VOIDED; l.onVoided(done + 1, reason); l.onPhase(phase);
    }
    public synchronized void stop() {
        if (phase == Phase.REST) { timer.cancel(); phase = Phase.FINISHED; l.onPhase(phase); } else voidCurrent("manual");
    }
    private synchronized void tick() {
        int left = (int) ((endAt - System.currentTimeMillis()) / 1000);
        if (left > 0) { l.onTick(left); return; }
        switch (phase) {
            case PREP: case REST: begin(Phase.WORK, Config.WORK_SEC); break;
            case WORK:
                l.onDone(++done);
                if (done >= total) { timer.cancel(); phase = Phase.FINISHED; l.onPhase(phase); }
                else begin(Phase.REST, done % Config.LONG_REST_EVERY == 0 ? Config.LONG_REST_SEC : Config.SHORT_REST_SEC);
                break;
            default: break;
        }
    }
}
