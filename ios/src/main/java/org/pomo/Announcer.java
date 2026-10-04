package org.pomo;
import org.robovm.apple.avfoundation.AVSpeechSynthesisVoice;
import org.robovm.apple.avfoundation.AVSpeechSynthesizer;
import org.robovm.apple.avfoundation.AVSpeechUtterance;
/** 語音提醒，語言跟隨介面語言。iOS 不允許 App 直接設定系統音量，所以把語音音量設為 0.5。 */
public class Announcer {
    private final AVSpeechSynthesizer synth = new AVSpeechSynthesizer();
    public void say(String text) {
        AVSpeechUtterance u = new AVSpeechUtterance(text);
        u.setVolume(0.5f);
        u.setVoice(new AVSpeechSynthesisVoice(I18n.speechTag()));
        synth.enqueueSpeakUtterance(u);
    }
}
