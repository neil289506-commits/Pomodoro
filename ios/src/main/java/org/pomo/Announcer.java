package org.pomo;
import org.robovm.apple.avfoundation.AVSpeechSynthesizer;
import org.robovm.apple.avfoundation.AVSpeechUtterance;
/** 語音提醒。iOS 不允許 App 直接設定系統音量，這裡把語音音量設為 0.5（見 COPILOT_TODO.md）。 */
public class Announcer {
    private final AVSpeechSynthesizer synth = new AVSpeechSynthesizer();
    public void say(String text) {
        AVSpeechUtterance u = new AVSpeechUtterance(text);
        u.setVolume(0.5f);
        synth.speak(u);
    }
}
