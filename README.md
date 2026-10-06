# 番茄守護 TomatoGuard

遵循番茄工作法的專注計時器。**桌面版（Windows、macOS、Linux）是本專案自己寫的**；**手機版（Android、iOS）收錄兩個 GPL-3.0 的開源專案**（見「手機版」）。以 **GPL-3.0** 授權。

> 目前處於早期階段：五個平台的 CI 都能編譯，但尚未做完整的實機驗證，功能完成度請見下方「目前狀態」。
>
> TomatoGuard 的規則與功能（硬鎖、專注守護、放行 App 熱鍵…）**只在桌面版**；手機版是上游專案本身，沒有這些功能。

## 規則

- 按下開始後先倒數 **3 分鐘**準備，接著進入 **25 分鐘**專注。
- 可選擇要做幾個番茄鐘（1～12，**預設 1 個**），每一個都是不可切割的一環。
- 每個番茄鐘後休息 5 分鐘，每完成 4 個休息 20 分鐘。
- 專注或準備中按下**停止**，該番茄鐘即作廢。
- **專注守護**：專注中離開視窗（切換到未放行的 App）超過 **3 次**，該番茄鐘作廢。
- **硬鎖（桌面）**：專注中在系統層攔截切換視窗的快捷鍵（見下方「硬鎖」），不只是全螢幕。
- **放行 App 熱鍵**：專注中按 **Ctrl+Alt+Shift+A**（macOS 為 Ctrl+Option+Shift+A）開啟放行 App 清單，選一個就啟動它。
- 每個番茄鐘的成功或失敗（含失敗原因）都會記錄下來，並提供統計與連勝。
- 每個階段切換時，音量調到 50% 並以語音提醒。

所有數值集中在 `qt-core/include/pomo/Config.h`。

## 取得編譯產物

每次推送到 `main`，CI 會編譯五個平台，並發佈一個標示為 **Pre-release** 的 GitHub Release，到 repo 右側的 **Releases** 下載。

- **Tag**：`CP#<次數>`，例如 `CP#7`（第 7 次發佈）
- **Release 名稱**：`CP#<日期時間>+<次數>`，例如 `CP#20261003-154530+7`（時間為 UTC+8）
- 每個平台各自一個 zip，檔名結尾為 `CP<次數>`（桌面為 `TomatoGuard-<平台>-CP<次數>.zip`，Android 為 `Goodtime-android-…`，iOS 為 `PoGl-Pomodoro-ios-arm64-…`）：

| zip | 平台 | 內容 |
|---|---|---|
| `windows-x64` | Windows x64 | `pomodoro.exe`（GUI 程式，不會跳出主控台視窗）加上 Qt 執行庫，**解壓後直接執行，不需安裝任何東西** |
| `windows-arm64` | Windows ARM64 | 同上，ARM64 原生版，已內含 Qt 執行庫與 VC 執行階段 |
| `macos-universal` | macOS（Apple Silicon + Intel） | `pomodoro.app`，**已內含 Qt**，使用者不需安裝；ad-hoc 簽署，沒有 Apple 開發者簽署與公證，第一次要右鍵 → 打開 |
| `linux-amd64` | Linux x86_64 | `.AppImage`，**已內含 Qt**，`chmod +x` 後直接執行 |
| `linux-arm64` | Linux aarch64 | 同上，ARM 版（在 Ubuntu 24.04 上編譯，需要 glibc 2.39 以上的系統，例如 Ubuntu 24.04、Debian 13、Fedora 40 以上） |
| `android` | Android | [Goodtime](https://github.com/adrcotfas/goodtime) 的 APK（fork，F-Droid 風味）。沒有設定簽署金鑰時用測試金鑰簽署 |
| `ios-arm64` | iPhone / iPad（arm64） | [po-gl/pomodoro](https://github.com/po-gl/pomodoro) 的**未簽署** `.ipa`（fork），需用 AltStore / Sideloadly 等側載工具安裝 |

五個平台全部編譯成功才會發佈；任何一個失敗就不會產生 Release。每個 zip 內都附有 `BUILD_INFO.txt`（commit、執行連結、時間）。

非 `main` 的分支和 Pull request 只會編譯，不會發佈；想預覽打包結果，可在 Actions 手動執行 `release` workflow，zip 會放在 `release-preview` artifact。

### 桌面版第一次執行

| 系統 | 步驟 |
|---|---|
| Windows | 解壓 zip，執行 `pomodoro.exe`。若出現 SmartScreen 警告：**其他資訊 → 仍要執行**（因為沒有買程式碼簽署憑證）。 |
| macOS | 解壓後把 `pomodoro.app` 拖到「應用程式」。第一次**按右鍵 → 打開**；若仍被擋，在終端機執行 `xattr -cr /Applications/pomodoro.app`。 |
| Linux | 解壓後 `chmod +x TomatoGuard-*.AppImage` 再執行。出現 FUSE 錯誤時，安裝 `libfuse2`，或執行 `./TomatoGuard-*.AppImage --appimage-extract-and-run`。語音提醒需要系統已安裝 `speech-dispatcher`。 |

使用者**不需要安裝 Qt 或任何 SDK**。

## 語言（i18n）

介面與語音提醒支援 **English、繁體中文、简体中文、日本語、한국어、Español**，預設跟隨系統語言，也可以在「設定」手動切換（立即生效）。

桌面版的字串來源是 [`i18n/strings.json`](i18n/strings.json)，編進資源檔。**新增語言只需要兩步**：在 `i18n/build_catalog.py` 的 `LANGS` 加一欄並補齊每一列翻譯，再執行 `python3 i18n/build_catalog.py`（腳本會檢查每一列的 `%1`、`%2` 佔位符是否一致）。

## 硬鎖與放行 App（桌面版）

專注時段（25 分鐘）會啟用硬鎖，各平台攔截的範圍不同：

| 平台 | 方式 | 已攔截 | 攔截不了 |
|---|---|---|---|
| Windows | 低階鍵盤 hook | Win 鍵、Alt+Tab、Alt+Esc、Alt+F4、Alt+Space、Ctrl+Esc、Ctrl+Shift+Esc | **Ctrl+Alt+Del**（系統保留，應用程式無法攔截） |
| macOS | Kiosk 展示選項 | Cmd+Tab、Dock、選單列、強制結束（Cmd+Option+Esc）、登出對話框 | 電源鍵 |
| Linux（X11） | 鍵盤獨佔（XGrabKeyboard） | Alt+Tab、Super 等所有視窗管理員快捷鍵 | Ctrl+Alt+F1~F7 切換終端機、電源鍵 |
| Linux（Wayland） | 不支援 | — | Wayland 不允許應用程式攔截系統快捷鍵，只剩全螢幕置頂 |

此外專注中視窗若被搶走焦點，會在 0.4 秒內拉回最上層；**停止鍵永遠可用**（停止 = 該番茄鐘作廢），所以這不是不可逆的鎖定。

**使用放行 App**
1. 開始專注前，到「設定」分頁新增放行的 App：用「瀏覽…」選擇程式（會存成完整路徑），也可以只填程式名稱。
2. 專注中按 **Ctrl+Alt+Shift+A**（或按畫面上的按鈕），選擇要開啟的 App。
3. 硬鎖會暫停並讓出畫面。離開該 App（切到清單以外的 App、或回到本程式）或再按一次熱鍵，就會立刻重新鎖上；切到清單以外的 App 同時會算一次「離開」。
4. 放行 App 的比對方式是「程式檔名（小寫、不含 .exe / .app）」。

## 語音提醒

每次階段切換會把系統音量調到 50% 並語音提醒。CI 與發佈版本都有安裝 Qt Speech 模組，缺少該模組時 CI 會直接失敗，不會悄悄編出沒有語音的版本。
Linux 需要系統已安裝 speech-dispatcher（`speechd`）才有聲音。

## 手機版（Android、iOS）

手機版**不是本專案自己寫的**，是下面兩個開源專案的 fork（原始碼快照放在本倉庫，**沒有修改任何原始碼**）。兩者都是 GPL-3.0，與本專案相同：

| 目錄 | 上游專案 | 作者 | 說明 |
|---|---|---|---|
| [`android/`](android/UPSTREAM.md) | [adrcotfas/goodtime](https://github.com/adrcotfas/goodtime)（Goodtime Productivity） | Adrian Cotfas | Kotlin Multiplatform + Compose 的專注計時器。我們編譯 **F-Droid 風味**（沒有 RevenueCat 等專有函式庫） |
| [`ios/`](ios/UPSTREAM.md) | [po-gl/pomodoro](https://github.com/po-gl/pomodoro) | Porter Glines | SwiftUI 的 iOS + watchOS 專注計時器 |

- 出處、快照 commit、刪除了哪些檔案、已知限制：見各目錄的 `UPSTREAM.md` 與根目錄的 [NOTICE.md](NOTICE.md)。
- 這些 App 的功能與錯誤屬於上游；**請把 App 本身的問題回報到上游**。
- Android 沒有設定簽署金鑰時用測試金鑰簽署，每個版本的簽章不同，**更新前要先解除安裝舊版**；要穩定更新，請在 GitHub Secrets 設定 `ANDROID_KEYSTORE_BASE64`、`ANDROID_KEYSTORE_PASSWORD`、`ANDROID_KEY_ALIAS`、`ANDROID_KEY_PASSWORD`。
- iOS 的 IPA 沒有簽署，安裝方式見手機安裝教學：[繁體中文](docs/MOBILE_USER_GUIDE.zh-TW.md)｜[English](docs/MOBILE_USER_GUIDE.md)。

## 目前狀態（TomatoGuard 桌面版）

| 功能 | 狀態 |
|---|---|
| 準備／專注／休息流程、停止作廢 | ✅ |
| 專注守護（離開次數）、前景 App 偵測、放行清單與熱鍵啟動 | ✅ |
| 硬鎖（見上） | ✅（Wayland 與部分系統快捷鍵除外） |
| 成功／失敗紀錄、統計 | ✅ |
| 圓環計時、紀錄頁、設定頁、停止前確認、深色模式 | ✅ |
| 多語系（6 種，可手動切換）、語音提醒（語言跟隨介面） | ✅ |
| 音量 50% | ✅ |
| 瀏覽器守護（非白名單網站顯示專注守護） | ❌ 尚未實作 |

已知限制：

- 硬鎖無法攔截 Ctrl+Alt+Del（Windows）與電源鍵，Wayland 不支援硬鎖；詳見「硬鎖與放行 App」。
- 設定頁裡的「放行網站」目前只會儲存，尚無瀏覽器端的實作。

## 專案結構

```
qt-core/   桌面共用核心（Qt6/C++）：流程、守護、紀錄、統計、介面
windows/   Windows 平台層：音量控制、前景視窗偵測
macos/     macOS 平台層（Objective-C++）
linux/     Linux 平台層（X11）
android/   Android 版：Goodtime 的 fork（GPL-3.0，出處見 android/UPSTREAM.md）
ios/       iOS 版：po-gl/pomodoro 的 fork（GPL-3.0，出處見 ios/UPSTREAM.md）
i18n/      桌面版的多語系字串
assets/    圖示
.github/   五個平台的 CI 與發佈流程
```

## 本機編譯

桌面版統一使用 **Qt 6.8.3**，編譯器：Windows 用 MSVC 2022、Linux 用 g++、macOS 用 Clang（CMake 會檢查）。

```bash
# Linux
cmake -S linux -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
cmake --build build

# macOS（預設即為 arm64 + x86_64 通用版）
cmake -S macos -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++
cmake --build build

# Windows（x64 Native Tools Command Prompt for VS 2022）
cmake -S windows -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=cl
cmake --build build
```

Qt 不在預設路徑時加上 `-DCMAKE_PREFIX_PATH=<Qt 6.8.3 路徑>`。語音提醒需另外安裝 Qt 的 Qt Speech 與 Qt Multimedia 模組。以上是開發者自行編譯才需要，一般使用者請下載 Release。

```bash
# Android（Goodtime；需要 Android SDK、JDK 21）
cd android && ./gradlew :androidApp:assembleFdroidRelease

# iOS（po-gl/pomodoro；需要 macOS + Xcode，見 ios/UPSTREAM.md）
```

## 授權

本專案以 **GNU General Public License v3.0（`GPL-3.0-only`）** 授權，全文見 [LICENSE](LICENSE)。

手機版收錄的 Goodtime 與 po-gl/pomodoro 同為 GPL-3.0，保留各自的授權文件與著作權聲明。完整的出處與第三方聲明見 [NOTICE.md](NOTICE.md)。
