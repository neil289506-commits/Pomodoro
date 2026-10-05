package org.pomo;
import org.robovm.apple.uikit.UIColor;
/** 與桌面版、Android 相同的色票：專注中一律深色，其餘時段跟隨系統深色模式。 */
public final class Theme {
    private Theme() {}
    public static final class Palette {
        public final UIColor bg, surface, text, muted, border; public final boolean dark;
        Palette(int bg, int surface, int text, int muted, int border, boolean dark) {
            this.bg = c(bg); this.surface = c(surface); this.text = c(text); this.muted = c(muted); this.border = c(border); this.dark = dark;
        }
    }
    public static UIColor c(int rgb) { return UIColor.fromRGBA(((rgb >> 16) & 255) / 255.0, ((rgb >> 8) & 255) / 255.0, (rgb & 255) / 255.0, 1.0); }
    private static final Palette LIGHT = new Palette(0xFAF7F2, 0xFFFFFF, 0x1F1F1F, 0x6B6B6B, 0xE6E0D6, false);
    private static final Palette DARK = new Palette(0x121214, 0x1C1C20, 0xF2F2F3, 0x9A9AA2, 0x2E2E34, true);
    public static Palette palette(Phase p, boolean systemDark) { return p == Phase.WORK || systemDark ? DARK : LIGHT; }
    public static UIColor phaseColor(Phase p) {
        switch (p) {
            case PREP: return c(0xF5A623); case WORK: return c(0xE5484D); case REST: return c(0x30A46C);
            case FINISHED: return c(0x3E63DD); case VOIDED: return c(0x8B8D98); default: return c(0xE5484D);
        }
    }
    public static final UIColor SUCCESS = c(0x30A46C), FAIL = c(0xE5484D);
}
