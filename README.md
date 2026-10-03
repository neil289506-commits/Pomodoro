# 番茄守護 TomatoGuard

遵循番茄工作法的專注計時器，支援 Windows、macOS、Linux、Android、iOS。開源專案。

> 目前處於早期階段：五個平台的 CI 都能編譯，但尚未做完整的實機驗證，功能完成度請見下方「目前狀態」。

## 規則

- 按下開始後先倒數 **3 分鐘**準備，接著進入 **25 分鐘**專注。
- 可選擇要做幾個番茄鐘（1～12），每一個都是不可切割的一環。
- 每個番茄鐘後休息 5 分鐘，每完成 4 個休息 20 分鐘。
- 專注或準備中按下**停止**，該番茄鐘即作廢。
- **專注守護**：專注中離開視窗（切換到未放行的 App）超過 **3 次**，該番茄鐘作廢。
- 每個番茄鐘的成功或失敗（含失敗原因）都會記錄下來，並提供統計與連勝。
- 每個階段切換時，音量調到 50% 並以語音提醒。

所有數值集中在 `qt-core/include/pomo/Config.h`（桌面）與各平台的 `Config` 檔。

## 取得編譯產物

每次推送到 `main`，CI 會編譯五個平台，並發佈一個標示為 **Pre-release** 的 GitHub Release，到 repo 右側的 **Releases** 下載。

- **Tag**：`CP#<次數>`，例如 `CP#7`（第 7 次發佈）
- **Release 名稱**：`CP#<日期時間>+<次數>`，例如 `CP#20261003-154530+7`（時間為 UTC+8）
- 每個平台各自一個 zip，檔名為 `TomatoGuard-<平台>-CP<次數>.zip`：

| zip | 平台 | 內容 |
|---|---|---|
| `windows-x64` | Windows x64 | `pomodoro.exe`（GUI 程式，不會跳出主控台視窗）加上 Qt 執行庫，解壓後可直接執行 |
| `windows-arm64` | Windows ARM64 | 只有 `pomodoro.exe`，**尚未打包 Qt 執行庫**，需自備 Qt 6.8.3 ARM64 執行庫 |
| `macos-universal` | macOS（Apple Silicon + Intel） | `pomodoro.app`，**需要本機已安裝 Qt 6.8.3**，尚未內含 Qt 執行庫、未簽署 |
| `linux-amd64` | Linux x86_64 | 單一執行檔，**需要本機已安裝 Qt 6.8.3 的執行庫** |
| `linux-arm64` | Linux aarch64 | 同上，ARM 版 |
| `android` | Android | debug 金鑰簽署的 APK（非正式簽署），同一個檔案支援 ARM 與 x86 |
| `ios-arm64` | iPhone / iPad（arm64） | **未簽署**的 `.ipa`，需自行簽署或用 AltStore / Sideloadly 等側載工具安裝 |

五個平台全部編譯成功才會發佈；任何一個失敗就不會產生 Release。每個 zip 內都附有 `BUILD_INFO.txt`（commit、執行連結、時間）。

非 `main` 的分支和 Pull request 只會編譯，不會發佈；想預覽打包結果，可在 Actions 手動執行 `release` workflow，zip 會放在 `release-preview` artifact。

## 目前狀態

| 功能 | 桌面（Qt） | Android | iOS |
|---|---|---|---|
| 準備／專注／休息流程、停止作廢 | ✅ | ✅ | ✅（邏輯層） |
| 專注守護（離開次數） | ✅ | ✅ | 僅能偵測 App 進入背景 |
| 前景 App 偵測與放行清單 | ✅ | ✅（需授權使用情況存取） | ❌（系統不允許） |
| 成功／失敗紀錄 | ✅ | ✅ | ✅ |
| 統計頁、設定頁、圓環介面 | ✅ | ❌（最小畫面） | ❌（最小畫面） |
| 勿擾／專注鎖定 | 全螢幕 | 勿擾模式（需授權） | ❌ |
| 音量 50% | ✅ | ✅ | ❌（只能調語音音量） |
| 語音提醒 | ⚠️ 見下 | ✅ | ✅ |
| 瀏覽器守護（非白名單網站顯示專注守護） | ❌ 尚未實作 | ❌ | ❌ |

已知限制：

- CI 目前沒有安裝 Qt 的 `qtspeech` 模組，**CI 編出的桌面版沒有語音提醒**（只會調整音量）。
- 桌面端的專注鎖定是全螢幕的「軟鎖」，無法攔截 Alt+Tab、Ctrl+Alt+Del 等系統快捷鍵。
- 設定頁裡的「放行網站」目前只會儲存，尚無瀏覽器端的實作。
- iOS 的平台限制：App 無法設定系統音量、開啟勿擾、偵測其他 App 前景。

## 專案結構

```
qt-core/   桌面共用核心（Qt6/C++）：流程、守護、紀錄、統計、介面
windows/   Windows 平台層：音量控制、前景視窗偵測
macos/     macOS 平台層（Objective-C++）
linux/     Linux 平台層（X11）
android/   Android 版（Gradle + Kotlin）
ios/       iOS 版（Gradle + Java，MobiVM / RoboVM）
.github/   五個平台的 CI
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

Qt 不在預設路徑時加上 `-DCMAKE_PREFIX_PATH=<Qt 6.8.3 路徑>`。語音提醒需另外安裝 Qt 的 Qt Speech 模組。

```bash
# Android（需要 Android SDK、JDK 17、Gradle 8.9）
cd android && gradle assembleDebug

# iOS（需要 macOS + Xcode、JDK 11、Gradle 7.6；產生未簽署的 .ipa）
cd ios && gradle createIPA
```

## 授權

尚未指定授權條款，公開前請補上 `LICENSE`。
