package org.pomo;
import org.robovm.apple.foundation.NSAutoreleasePool;
import org.robovm.apple.uikit.*;
public class IOSLauncher extends UIApplicationDelegateAdapter {
    private final Announcer announcer = new Announcer();
    @Override public boolean didFinishLaunching(UIApplication app, UIApplicationLaunchOptions opts) {
        UIWindow w = new UIWindow(UIScreen.getMainScreen().getBounds());
        w.setRootViewController(new UIViewController());
        w.makeKeyAndVisible();
        announcer.say("番茄鐘已啟動");
        return true;
    }
    public static void main(String[] args) {
        try (NSAutoreleasePool p = new NSAutoreleasePool()) { UIApplication.main(args, null, IOSLauncher.class); }
    }
}
