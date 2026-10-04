package org.pomo;
import org.robovm.apple.foundation.NSAutoreleasePool;
import org.robovm.apple.uikit.*;
public class IOSLauncher extends UIApplicationDelegateAdapter {
    private RootViewController root;
    private UIWindow window;   // 保持參考，避免被回收
    @Override public boolean didFinishLaunching(UIApplication app, UIApplicationLaunchOptions opts) {
        UIWindow w = new UIWindow(UIScreen.getMainScreen().getBounds());
        root = new RootViewController();
        w.setRootViewController(root);
        w.makeKeyAndVisible();
        window = w;
        return true;
    }
    @Override public void didEnterBackground(UIApplication app) { if (root != null) root.appBackgrounded(); }
    @Override public void willEnterForeground(UIApplication app) { if (root != null) root.appForegrounded(); }
    public static void main(String[] args) {
        try (NSAutoreleasePool p = new NSAutoreleasePool()) { UIApplication.main(args, null, IOSLauncher.class); }
    }
}
