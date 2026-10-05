package org.pomo;
import org.robovm.apple.coreanimation.CALineCap;
import org.robovm.apple.coreanimation.CAShapeLayer;
import org.robovm.apple.coregraphics.CGPoint;
import org.robovm.apple.coregraphics.CGRect;
import org.robovm.apple.uikit.*;
/** 圓環倒數：顏色隨階段變化，中央顯示剩餘時間與階段名稱。 */
public class RingView extends UIView {
    private final CAShapeLayer track = new CAShapeLayer(), arc = new CAShapeLayer();
    private final UILabel time = new UILabel(), name = new UILabel();
    public RingView() {
        super(new CGRect(0, 0, 100, 100));
        for (CAShapeLayer l : new CAShapeLayer[] {track, arc}) {
            l.setFillColor(UIColor.clear().getCGColor()); l.setLineCap(CALineCap.Round); getLayer().addSublayer(l);
        }
        arc.setStrokeEnd(1.0);
        for (UILabel l : new UILabel[] {time, name}) { l.setTextAlignment(NSTextAlignment.Center); addSubview(l); }
    }
    /** 重新排版（由控制器在大小改變時呼叫）。 */
    public void layoutRing(CGRect frame) {
        setFrame(frame);
        double w = frame.getWidth(), h = frame.getHeight(), side = Math.min(w, h) - 40, lw = Math.max(10, side / 22);
        CGRect zero = new CGRect(0, 0, w, h);
        track.setFrame(zero); arc.setFrame(zero);
        track.setLineWidth(lw); arc.setLineWidth(lw);
        UIBezierPath path = UIBezierPath.newArc(new CGPoint(w / 2, h / 2), (side - lw) / 2, -Math.PI / 2, 1.5 * Math.PI, true);
        track.setPath(path.getCGPath()); arc.setPath(path.getCGPath());
        time.setFrame(new CGRect(0, h / 2 - side * 0.19, w, side * 0.3));
        time.setFont(UIFont.getMonospacedDigitSystemFont(side / 4.2, -0.4));
        name.setFrame(new CGRect(0, h / 2 + side * 0.13, w, side * 0.12));
        name.setFont(UIFont.getSystemFont(side / 14, 0.3));
    }
    public void update(Phase p, int remaining, int total, Theme.Palette pal, String label) {
        track.setStrokeColor(pal.border.getCGColor());
        arc.setStrokeColor(Theme.phaseColor(p).getCGColor());
        arc.setStrokeEnd(total > 0 ? Math.max(0, Math.min(1, remaining / (double) total)) : 0);
        time.setText(String.format("%02d:%02d", remaining / 60, remaining % 60)); time.setTextColor(pal.text);
        name.setText(label); name.setTextColor(Theme.phaseColor(p));
    }
}
