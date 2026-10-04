package org.pomo;
import java.nio.file.Paths;
import java.util.List;
import java.util.function.IntConsumer;
import org.robovm.apple.coregraphics.CGRect;
import org.robovm.apple.coregraphics.CGSize;
import org.robovm.apple.dispatch.DispatchQueue;
import org.robovm.apple.foundation.NSUserDefaults;
import org.robovm.apple.uikit.*;
/** 單一控制器：三個分頁（計時、紀錄、設定）、圓環、步進器與確認視窗，版面與桌面版、Android 相同。 */
public class RootViewController extends UIViewController implements SessionListener {
    private final Session session = new Session(this);
    private final Guardian guardian = new Guardian(reason -> ui(() -> session.voidCurrent(reason)));
    private final Announcer announcer = new Announcer();
    private History history;
    private RingView ring;
    private int tab = 0, count = Config.DEFAULT_ROUNDS, planned = 0, done = 0, remaining = Config.WORK_SEC, total = Config.WORK_SEC, leaves = 0;
    private Phase phase = Phase.IDLE;
    private double lastW = -1, lastH = -1;

    // ---------- 生命週期
    @Override public void viewDidLoad() {
        super.viewDidLoad();
        history = new History(Paths.get(System.getProperty("user.home"), "Documents"));
        int saved = NSUserDefaults.getStandardUserDefaults().getInt("rounds");
        count = saved >= 1 ? Math.min(saved, Config.MAX_ROUNDS) : Config.DEFAULT_ROUNDS;
        guardian.setOnLeave(n -> ui(() -> { leaves = n; rebuild(); }));
        rebuild();
    }
    @Override public void viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews();
        CGRect b = getView().getBounds();
        if (b.getWidth() != lastW || b.getHeight() != lastH) rebuild();   // 旋轉、iPad 分割畫面
    }
    /** 由 App 委派呼叫：iOS 只能偵測「本 App 被切到背景」。 */
    public void appBackgrounded() { guardian.onForeground("other", false); }
    public void appForegrounded() { guardian.onForeground("self", true); rebuild(); }
    private static void ui(Runnable r) { DispatchQueue.getMainQueue().async(r); }

    // ---------- Session 回呼（在計時器執行緒，要切回主執行緒）
    @Override public void onTick(int s) { ui(() -> { remaining = s; if (ring != null) ring.update(phase, remaining, total, pal(), phaseName(phase)); }); }
    @Override public void onDone(int index) { ui(() -> { done = index; history.record(index, true, ""); }); }
    @Override public void onVoided(int index, String reason) { ui(() -> history.record(index, false, reason)); }
    @Override public void onPhase(Phase p) {
        ui(() -> {
            phase = p;
            guardian.setActive(p == Phase.WORK);
            if (p == Phase.WORK) leaves = 0;
            UIApplication.getSharedApplication().setIdleTimerDisabled(p == Phase.WORK || p == Phase.PREP);
            total = p == Phase.PREP ? Config.PREP_SEC : p == Phase.WORK ? Config.WORK_SEC
                : p == Phase.REST ? (done % Config.LONG_REST_EVERY == 0 ? Config.LONG_REST_SEC : Config.SHORT_REST_SEC) : 1;
            boolean run = running();
            remaining = run ? total : p == Phase.IDLE ? Config.WORK_SEC : 0;
            rebuild();
            String key = p == Phase.PREP ? "speech.prep" : p == Phase.WORK ? "speech.work" : p == Phase.REST ? "speech.rest"
                : p == Phase.FINISHED ? "speech.finished" : p == Phase.VOIDED ? "speech.voided" : null;
            if (key != null) announcer.say(I18n.t(key));
        });
    }
    private boolean running() { return phase == Phase.PREP || phase == Phase.WORK || phase == Phase.REST; }

    // ---------- 工具
    private Theme.Palette pal() {
        boolean dark = getView().getTraitCollection().getUserInterfaceStyle() == UIUserInterfaceStyle.Dark;
        return Theme.palette(phase, dark);
    }
    private static String phaseName(Phase p) {
        switch (p) {
            case PREP: return I18n.t("phase.prep"); case WORK: return I18n.t("phase.work"); case REST: return I18n.t("phase.rest");
            case FINISHED: return I18n.t("phase.finished"); case VOIDED: return I18n.t("phase.voided"); default: return I18n.t("phase.idle");
        }
    }
    private UILabel label(String text, double size, double weight, UIColor color, CGRect f, boolean center) {
        UILabel l = new UILabel(f); l.setText(text); l.setFont(UIFont.getSystemFont(size, weight)); l.setTextColor(color);
        l.setNumberOfLines(0); if (center) l.setTextAlignment(NSTextAlignment.Center); return l;
    }
    private UIButton button(String title, boolean primary, CGRect f, Theme.Palette pal, UIColor accent, Runnable action) {
        UIButton b = new UIButton(UIButtonType.System); b.setFrame(f);
        b.setTitle(title, UIControlState.Normal);
        b.setTitleColor(primary ? UIColor.white() : pal.text, UIControlState.Normal);
        b.getTitleLabel().setFont(UIFont.getSystemFont(primary ? 17 : 15, primary ? 0.4 : 0.0));
        b.setBackgroundColor(primary ? accent : UIColor.clear());
        b.getLayer().setCornerRadius(12); b.setClipsToBounds(true);
        if (!primary) { b.getLayer().setBorderWidth(1); b.getLayer().setBorderColor(pal.border.getCGColor()); }
        b.addOnTouchUpInsideListener((c, e) -> action.run());
        return b;
    }
    private UIView card(CGRect f, Theme.Palette pal) {
        UIView v = new UIView(f); v.setBackgroundColor(pal.surface);
        v.getLayer().setCornerRadius(14); v.getLayer().setBorderWidth(1); v.getLayer().setBorderColor(pal.border.getCGColor()); return v;
    }
    private void choose(String title, String message, String[] options, IntConsumer onChoice) {
        UIAlertController a = new UIAlertController(title, message, UIAlertControllerStyle.Alert);
        for (int i = 0; i < options.length; i++) { final int idx = i; a.addAction(new UIAlertAction(options[i], UIAlertActionStyle.Default, act -> onChoice.accept(idx))); }
        a.addAction(new UIAlertAction("✕", UIAlertActionStyle.Cancel, act -> {}));
        presentViewController(a, true, null);
    }

    // ---------- 畫面
    private void rebuild() {
        UIView root = getView();
        for (UIView v : root.getSubviews().toArray(new UIView[0])) v.removeFromSuperview();
        CGRect b = root.getBounds(); lastW = b.getWidth(); lastH = b.getHeight();
        Theme.Palette pal = pal();
        root.setBackgroundColor(pal.bg);
        UIEdgeInsets inset = root.getSafeAreaInsets();
        double top = inset.getTop(), bottom = inset.getBottom(), w = b.getWidth(), h = b.getHeight();
        double tabH = phase == Phase.WORK ? 0 : 56;
        CGRect content = new CGRect(0, top, w, h - top - bottom - tabH);
        ring = null;
        int page = phase == Phase.WORK ? 0 : tab;
        if (page == 0) timerPage(root, content, pal); else if (page == 1) historyPage(root, content, pal); else settingsPage(root, content, pal);
        if (tabH > 0) tabBar(root, new CGRect(0, h - bottom - tabH, w, tabH), pal);
    }
    private void tabBar(UIView root, CGRect f, Theme.Palette pal) {
        String[] names = {I18n.t("tab.timer"), I18n.t("tab.history"), I18n.t("tab.settings")};
        double bw = f.getWidth() / 3;
        for (int i = 0; i < 3; i++) {
            final int idx = i; boolean sel = i == tab;
            UIButton t = new UIButton(UIButtonType.System); t.setFrame(new CGRect(i * bw, f.getY(), bw, f.getHeight()));
            t.setTitle(names[i], UIControlState.Normal);
            t.setTitleColor(sel ? Theme.phaseColor(Phase.IDLE) : pal.muted, UIControlState.Normal);
            t.getTitleLabel().setFont(UIFont.getSystemFont(14, sel ? 0.4 : 0.0));
            t.addOnTouchUpInsideListener((c, e) -> { tab = idx; rebuild(); });
            root.addSubview(t);
        }
    }
    private void timerPage(UIView root, CGRect c, Theme.Palette pal) {
        UIColor accent = Theme.phaseColor(phase);
        double colW = Math.min(c.getWidth() - 48, 520), x = c.getX() + (c.getWidth() - colW) / 2, y = c.getY() + 16;
        if (running()) {
            int idx = phase == Phase.REST ? done : Math.min(done + 1, planned);
            root.addSubview(label(I18n.t("timer.round", Math.max(1, idx), planned), 20, 0.3, pal.text, new CGRect(x, y, colW, 28), true));
        }
        y += 30;
        String hint = phase == Phase.PREP ? "timer.prep_hint" : phase == Phase.REST ? "timer.rest_hint" : phase == Phase.FINISHED ? "timer.done_hint"
            : phase == Phase.VOIDED ? "timer.voided_hint" : phase == Phase.IDLE ? "timer.idle_hint" : null;
        if (hint != null) root.addSubview(label(I18n.t(hint), 13, 0, pal.muted, new CGRect(x, y, colW, 36), true));
        y += 38;
        boolean run = running();
        double controls = run ? 90 + 40 : 190;          // 下方控制區高度
        double ringH = Math.max(200, c.getY() + c.getHeight() - y - controls);
        ring = new RingView(); ring.layoutRing(new CGRect(x, y, colW, ringH)); root.addSubview(ring);
        ring.update(phase, remaining, total, pal, phaseName(phase));
        y += ringH + 6;
        if (!run) {
            root.addSubview(label(I18n.t("timer.pomodoros"), 13, 0, pal.muted, new CGRect(x, y, colW, 18), true));
            y += 20;
            double mid = x + colW / 2;
            UIButton minus = button("−", false, new CGRect(mid - 100, y, 52, 52), pal, accent, () -> setCount(count - 1));
            UIButton plus = button("+", false, new CGRect(mid + 48, y, 52, 52), pal, accent, () -> setCount(count + 1));
            minus.setEnabled(count > 1); plus.setEnabled(count < Config.MAX_ROUNDS);
            root.addSubview(minus); root.addSubview(plus);
            root.addSubview(label(String.valueOf(count), 28, 0.4, pal.text, new CGRect(mid - 44, y + 6, 88, 40), true));
            y += 58;
            root.addSubview(label(I18n.t("timer.start_hint"), 13, 0, pal.muted, new CGRect(x, y, colW, 36), true));
            y += 38;
            root.addSubview(button(I18n.t("timer.start"), true, new CGRect(x, y, colW, 52), pal, accent, this::startSession));
        } else {
            root.addSubview(button(I18n.t("timer.stop"), false, new CGRect(x, y, colW, 52), pal, accent, this::confirmStop));
            y += 60;
            String g = phase == Phase.WORK ? I18n.t("guard.leaves", leaves, Config.MAX_LEAVES) : I18n.t("guard.idle");
            root.addSubview(label(g, 13, 0, pal.muted, new CGRect(x, y, colW, 36), true));
        }
    }
    private void setCount(int n) { count = Math.max(1, Math.min(Config.MAX_ROUNDS, n)); rebuild(); }
    private void startSession() {
        NSUserDefaults.getStandardUserDefaults().put("rounds", count);
        planned = count; done = 0; session.start(count);
    }
    private void confirmStop() {
        UIAlertController a = new UIAlertController(I18n.t("confirm.title"), I18n.t(phase == Phase.REST ? "confirm.rest" : "confirm.work"), UIAlertControllerStyle.Alert);
        a.addAction(new UIAlertAction(I18n.t("confirm.keep"), UIAlertActionStyle.Cancel, act -> {}));
        a.addAction(new UIAlertAction(I18n.t("confirm.stop"), UIAlertActionStyle.Destructive, act -> session.stop()));
        presentViewController(a, true, null);
    }
    private void historyPage(UIView root, CGRect c, Theme.Palette pal) {
        List<History.Entry> rows = history.load();
        int ok = 0, streak = 0, best = 0;
        for (History.Entry e : rows) { if (e.success()) { ok++; streak++; best = Math.max(best, streak); } else streak = 0; }
        double colW = Math.min(c.getWidth() - 40, 560), x = c.getX() + (c.getWidth() - colW) / 2, y = c.getY() + 20, cardW = (colW - 20) / 3;
        int[] nums = {ok, rows.size() - ok, best}; String[] caps = {"history.completed", "history.voided", "history.best_streak"};
        for (int i = 0; i < 3; i++) {
            double cx = x + i * (cardW + 10);
            UIView card = card(new CGRect(cx, y, cardW, 82), pal); root.addSubview(card);
            root.addSubview(label(String.valueOf(nums[i]), 28, 0.4, pal.text, new CGRect(cx, y + 10, cardW, 34), true));
            root.addSubview(label(I18n.t(caps[i]), 12, 0, pal.muted, new CGRect(cx + 6, y + 46, cardW - 12, 30), true));
        }
        y += 100;
        if (rows.isEmpty()) { root.addSubview(label(I18n.t("history.empty"), 14, 0, pal.muted, new CGRect(x, y + 30, colW, 70), true)); return; }
        UIScrollView sv = new UIScrollView(new CGRect(c.getX(), y, c.getWidth(), c.getY() + c.getHeight() - y)); sv.setAlwaysBounceVertical(true); root.addSubview(sv);
        double ry = 0;
        for (int i = rows.size() - 1; i >= 0; i--) {
            History.Entry e = rows.get(i);
            String reason = "manual".equals(e.reason) ? I18n.t("reason.manual") : "leaves".equals(e.reason) ? I18n.t("reason.leaves", Config.MAX_LEAVES) : e.reason;
            String when = e.time.replace('T', ' '); when = when.length() > 16 ? when.substring(0, 16) : when;
            sv.addSubview(card(new CGRect(x - c.getX(), ry, colW, 62), pal));
            sv.addSubview(label((e.success() ? "✓  " : "✕  ") + I18n.t("history.index", e.index), 15, 0.4, e.success() ? Theme.SUCCESS : Theme.FAIL, new CGRect(x - c.getX() + 14, ry + 8, colW - 28, 22), false));
            sv.addSubview(label(when + (!e.success() && !reason.isEmpty() ? "  ·  " + reason : ""), 12, 0, pal.muted, new CGRect(x - c.getX() + 14, ry + 32, colW - 28, 22), false));
            ry += 70;
        }
        sv.setContentSize(new CGSize(c.getWidth(), ry + 8));
    }
    private void settingsPage(UIView root, CGRect c, Theme.Palette pal) {
        UIColor accent = Theme.phaseColor(Phase.IDLE);
        double colW = Math.min(c.getWidth() - 40, 560), x = (c.getWidth() - colW) / 2, y = 20;
        UIScrollView sv = new UIScrollView(c); sv.setAlwaysBounceVertical(true); root.addSubview(sv);
        // 語言
        String[][] langs = I18n.languages(); String cur = I18n.preference();
        String curName = I18n.t("settings.language.auto");
        for (String[] l : langs) if (l[0].equals(cur)) curName = l[1];
        sv.addSubview(card(new CGRect(x, y, colW, 118), pal));
        sv.addSubview(label(I18n.t("settings.language"), 16, 0.4, pal.text, new CGRect(x + 16, y + 12, colW - 32, 22), false));
        sv.addSubview(button(curName, false, new CGRect(x + 16, y + 48, colW - 32, 52), pal, accent, () -> {
            String[] names = new String[langs.length + 1]; names[0] = I18n.t("settings.language.auto");
            for (int i = 0; i < langs.length; i++) names[i + 1] = langs[i][1];
            choose(I18n.t("settings.language"), null, names, i -> { I18n.setPreference(i == 0 ? "auto" : langs[i - 1][0]); rebuild(); });
        }));
        y += 134;
        // iOS 的限制說明
        sv.addSubview(card(new CGRect(x, y, colW, 96), pal));
        sv.addSubview(label(I18n.t("settings.ios_limit"), 13, 0, pal.muted, new CGRect(x + 16, y + 10, colW - 32, 76), false));
        y += 112;
        // 關於
        sv.addSubview(card(new CGRect(x, y, colW, 130), pal));
        sv.addSubview(label(I18n.t("settings.about"), 16, 0.4, pal.text, new CGRect(x + 16, y + 12, colW - 32, 22), false));
        sv.addSubview(label("TomatoGuard", 20, 0.4, pal.text, new CGRect(x + 16, y + 42, colW - 32, 26), false));
        sv.addSubview(label(I18n.t("about.tagline"), 13, 0, pal.muted, new CGRect(x + 16, y + 70, colW - 32, 20), false));
        sv.addSubview(label(I18n.t("settings.version", "0.3.0"), 13, 0, pal.muted, new CGRect(x + 16, y + 94, colW - 32, 20), false));
        y += 150;
        sv.setContentSize(new CGSize(c.getWidth(), y));
    }
}
