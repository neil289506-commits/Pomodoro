# TomatoGuard — Mobile User Guide

How to install the **Android APK** and the **iPhone / iPad IPA** from a GitHub Release.

> **Early-stage software.** Every build is published as a **Pre-release**. The mobile apps currently have a minimal screen and only part of the desktop feature set. The APK is signed with a debug key, and the IPA is **unsigned**, so neither can be installed from an app store. This guide shows how to install them yourself.

---

## 1. Download the right file

1. Open the project's **Releases** page: <https://github.com/neil289506-commits/Pomodoro/releases>
2. Pick the newest release. Its tag looks like `CP#7` and its title looks like `CP#20261003-154530+7`.
3. Under **Assets**, download the zip for your phone:

| Device | Asset name | What is inside |
|---|---|---|
| Android | `TomatoGuard-android-CP<N>.zip` | an `.apk` file |
| iPhone / iPad | `TomatoGuard-ios-arm64-CP<N>.zip` | an unsigned `.ipa` file |

4. **Unzip** the file. Each zip also contains `BUILD_INFO.txt` with the commit and build run it came from.

Only download files from the official Releases page of this repository.

---

## 2. Install on Android (APK)

**Requirements:** Android 8.0 (API 26) or newer. One APK works on ARM and x86 devices.

### Option A — Install directly on the phone

1. Get the `.apk` onto your phone: download and unzip it on the phone, or copy it from a computer over USB, cloud storage, or any file-transfer app.
2. Open the file with the **Files** app (or the browser's download list).
3. Android asks for permission to install apps from this source. Tap **Settings** and switch on **Allow from this source** for the app you opened the file with (Files, Chrome, etc.), then go back.
   - Older Android versions: **Settings → Security → Install unknown apps**.
4. Tap **Install**.
5. If **Google Play Protect** warns that the app is unrecognized, tap **More details → Install anyway**. This happens because the app is not distributed through Google Play.
6. Tap **Open**.

### Option B — Install from a computer with ADB (Windows and macOS)

1. On the phone, enable **Developer options**: **Settings → About phone**, tap **Build number** seven times.
2. Go to **Settings → System → Developer options** and switch on **USB debugging**.
3. Install Android's **platform-tools** from <https://developer.android.com/tools/releases/platform-tools> and unzip them.
4. Connect the phone with a USB cable and accept the **Allow USB debugging?** prompt on the phone.
5. Open a terminal in the platform-tools folder:
   - **Windows** (PowerShell): `.\adb devices`, then `.\adb install -r path\to\TomatoGuard.apk`
   - **macOS** (Terminal): `./adb devices`, then `./adb install -r /path/to/TomatoGuard.apk`
6. The device should be listed as `device`, and the install command should print `Success`.

### Permissions the app may ask for

| Permission | Why |
|---|---|
| **Usage access** (Settings → Apps → Special app access) | Lets the focus guard see which app is in the foreground |
| **Do Not Disturb access** | Silences notifications during a focus session |

You can deny them; the matching feature will simply not work.

### Updating or uninstalling

- To update, install the newer APK over the old one.
- If Android reports a **signature conflict**, uninstall the old version first (**Settings → Apps → TomatoGuard → Uninstall**) and install again. Your history stored in the app is removed with it.

---

## 3. Install on iPhone / iPad (IPA)

Apple only runs apps that are signed. The IPA in the release is **unsigned**, so you must sign it with your own Apple ID, which a sideloading tool does for you. This is called **sideloading** or **self-signing**.

### What you need

- An iPhone or iPad running **iOS / iPadOS 15.0 or newer**
- A **Windows PC or Mac**
- A USB cable (a Lightning or USB-C cable that supports data)
- An **Apple ID**. A free one works. Using a secondary Apple ID just for sideloading is a good idea.

### Limits of a free Apple ID

| Limit | Details |
|---|---|
| Validity | The app **stops opening after 7 days** and must be refreshed (re-signed) |
| Active apps | At most **3** sideloaded apps at the same time |
| Other limits | Apple also caps how many App IDs and devices a free account can use per week |

A paid **Apple Developer Program** account (US$99/year) extends validity to 365 days. Tools such as Sideloadly accept paid accounts too.

### Step 0 — Prepare the iPhone / iPad

1. Connect the device to the computer with the cable and tap **Trust** on the device. Enter your passcode if asked.
2. **Developer Mode (iOS 16 and later):** open **Settings → Privacy & Security → Developer Mode**, switch it **On**, and tap **Restart**. After the restart, confirm **Turn On** and enter your passcode.
   - If you do not see the Developer Mode switch, run the installation in Option A or B below once. The switch usually appears after a developer-signed app has been installed, and then you can turn it on.

### Option A — Sideloadly (Windows and macOS)

Sideloadly signs and installs the IPA in one step. Download it from <https://sideloadly.io>.

#### Windows

1. Install **iTunes from Apple's website** (<https://www.apple.com/itunes/>). Do **not** use the Microsoft Store version; Sideloadly will not detect your device with it.
2. Install and open **Sideloadly**.
3. Connect the iPhone or iPad and unlock it. It should appear in the **iDevice** drop-down.
4. Drag the unzipped `.ipa` into the **IPA** box, or click the IPA icon and choose the file.
5. Type your **Apple ID** in the **Apple account** field.
6. Click **Start**, enter your Apple ID password and, if asked, the two-factor code.
7. Wait for **Done**.

#### macOS

1. Open the downloaded Sideloadly disk image and drag **Sideloadly** into **Applications**.
2. The first time, if macOS blocks it, open **System Settings → Privacy & Security** and click **Open Anyway**.
3. Open Sideloadly, connect and unlock the device, and choose it in the **iDevice** drop-down.
4. Drag the `.ipa` into the **IPA** box and enter your **Apple ID**.
5. Click **Start**, enter your password and the two-factor code if asked.
6. Wait for **Done**.

#### Trust the developer profile (both systems)

1. On the device, open **Settings → General → VPN & Device Management**.
2. Under **Developer App**, tap your Apple ID and tap **Trust**.
3. Open **TomatoGuard** from the Home Screen.

### Option B — AltStore (Windows and macOS)

AltStore installs an on-device app that can refresh your sideloaded apps over Wi-Fi while the computer's **AltServer** is running. Download it from <https://altstore.io>.

#### Windows

1. Install **iTunes** and **iCloud** from **Apple's website**, not the Microsoft Store, then open iCloud and sign in with your Apple ID.
2. Install **AltServer**. It appears as an icon in the system tray.
3. Connect the device. Click the AltServer tray icon, choose **Install AltStore**, pick your device, and enter your Apple ID.
4. On the device, trust your Apple ID profile (**Settings → General → VPN & Device Management**).
5. Put the `.ipa` somewhere the device can open it (the **Files** app, iCloud Drive, or AirDrop).
6. Open **AltStore** on the device, go to **My Apps**, tap **+**, and choose the `.ipa`.

#### macOS

1. Install **AltServer**. It appears in the menu bar. Follow AltServer's prompts for any extra setup it asks for.
2. Connect the device. Click the AltServer menu-bar icon, choose **Install AltStore**, pick your device, and enter your Apple ID.
3. Trust your Apple ID profile on the device as above.
4. Open **AltStore** on the device, go to **My Apps**, tap **+**, and choose the `.ipa` from **Files**.

#### Keeping the app alive

- AltStore refreshes apps automatically when the device and the computer are on the same Wi-Fi network and AltServer is running.
- If the app shows "no longer available", open AltStore and tap **Refresh All**.

### Refreshing and updating

- **Free Apple ID:** repeat the install (or use the tool's refresh feature) at least every 7 days.
- **New version:** install the new `.ipa` the same way. It replaces the old one and keeps your data.

---

## 4. Troubleshooting

| Problem | Fix |
|---|---|
| Android: "App not installed" | Uninstall any older version and try again; make sure free storage is available |
| Android: "Install blocked" | Allow installs from the app you opened the file with (Section 2, step 3) |
| Android: Play Protect warning | **More details → Install anyway** |
| ADB: `no devices` or `unauthorized` | Re-plug the cable, accept the USB debugging prompt, and run `adb devices` again |
| iOS: device not listed | Unlock the device, tap **Trust**, use a data-capable cable, and (Windows) reinstall iTunes from Apple's website |
| iOS: "Untrusted Developer" | **Settings → General → VPN & Device Management → Developer App → Trust** |
| iOS: "Developer Mode required" | **Settings → Privacy & Security → Developer Mode → On**, then restart |
| iOS: app closes immediately or says "no longer available" | The 7-day signature expired; refresh or reinstall |
| iOS: sign-in fails | Check two-factor code; try again with a secondary Apple ID |
| iOS: "maximum number of apps" error | Delete one of your other sideloaded apps, or wait for the weekly limit to reset |

---

## 5. Privacy and safety notes

- Your Apple ID and password go to **Apple** through the sideloading tool to create a free developer certificate. Use tools from their official websites and consider a secondary Apple ID.
- Install only builds from this repository's **Releases** page. Compare `BUILD_INFO.txt` with the release commit if you want to confirm where a build came from.
- Sideloaded apps are not reviewed by Google or Apple. Remove the app if you no longer trust the source.

---

## 6. Known limitations of the mobile builds

- The mobile apps currently have a **minimal screen**; the full timer ring, history page, and settings page are available on desktop only.
- **iOS** cannot control system volume, turn on Do Not Disturb, or see other apps in the foreground, so the focus guard can only react to the app moving to the background.
- The browser guard (blocking non-allowed websites) is **not implemented** on any platform yet.
- Builds are **Pre-releases** and may change without notice.
