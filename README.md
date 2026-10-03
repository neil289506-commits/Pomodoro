# 番茄守護 TomatoGuard

遵循番茄工作法的專注計時器，支援 Windows、macOS、Linux、Android、iOS。開源專案。

> 目前處於早期階段：五個平台的 CI 都能編譯，但尚未做完整的實機驗證，功能完成度請見下方「目前狀態」。

## 規則

- 按下開始後先倒數 **3 分鐘**準備，接著進入 **25 分鐘**專注。
- 可選擇要做幾個番茄鐘（1～12），每一個都是不可切割的一環。
- 每個番茄鐘後休息 5 分鐘，每完成 4 個休息 20 分鐘。
- 專注或準備中按下**停止**，該番茄鐘即作廢。
- **專注守護**：專注中離開視窗（切換到未放行的 App）超過 **3 次**，該番茄鐘作廢。
- **硬鎖（桌面）**：專注中在系統層攔截切換視窗的快捷鍵（見下方「硬鎖」），不只是全螢幕。
- **放行 App 熱鍵**：專注中按 **Ctrl+Alt+Shift+A**（macOS 為 Ctrl+Option+Shift+A）開啟放行 App 清單，選一個就啟動它。
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
| `windows-x64` | Windows x64 | `pomodoro.exe`（GUI 程式，不會跳出主控台視窗）加上 Qt 執行庫，**解壓後直接執行，不需安裝任何東西** |
| `windows-arm64` | Windows ARM64 | 同上，ARM64 原生版，已內含 Qt 執行庫與 VC 執行階段 |
| `macos-universal` | macOS（Apple Silicon + Intel） | `pomodoro.app`，**已內含 Qt**，使用者不需安裝；ad-hoc 簽署，沒有 Apple 開發者簽署與公證，第一次要右鍵 → 打開 |
| `linux-amd64` | Linux x86_64 | `.AppImage`，**已內含 Qt**，`chmod +x` 後直接執行 |
| `linux-arm64` | Linux aarch64 | 同上，ARM 版（在 Ubuntu 24.04 上編譯，需要 glibc 2.39 以上的系統，例如 Ubuntu 24.04、Debian 13、Fedora 40 以上） |
| `android` | Android | debug 金鑰簽署的 APK（非正式簽署），同一個檔案支援 ARM 與 x86 |
| `ios-arm64` | iPhone / iPad（arm64） | **未簽署**的 `.ipa`，需自行簽署或用 AltStore / Sideloadly 等側載工具安裝 |

五個平台全部編譯成功才會發佈；任何一個失敗就不會產生 Release。每個 zip 內都附有 `BUILD_INFO.txt`（commit、執行連結、時間）。

非 `main` 的分支和 Pull request 只會編譯，不會發佈；想預覽打包結果，可在 Actions 手動執行 `release` workflow，zip 會放在 `release-preview` artifact。

### 桌面版第一次執行

| 系統 | 步驟 |
|---|---|
| Windows | 解壓 zip，執行 `pomodoro.exe`。若出現 SmartScreen 警告：**其他資訊 → 仍要執行**（因為沒有買程式碼簽署憑證）。 |
| macOS | 解壓後把 `pomodoro.app` 拖到「應用程式」。第一次**按右鍵 → 打開**；若仍被擋，在終端機執行 `xattr -cr /Applications/pomodoro.app`。 |
| Linux | 解壓後 `chmod +x TomatoGuard-*.AppImage` 再執行。出現 FUSE 錯誤時，安裝 `libfuse2`，或執行 `./TomatoGuard-*.AppImage --appimage-extract-and-run`。語音提醒需要系統已安裝 `speech-dispatcher`。 |

使用者**不需要安裝 Qt 或任何 SDK**。

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

## 手機安裝教學

Android APK 與 iPhone / iPad IPA（含自行簽署）請見 [docs/MOBILE_USER_GUIDE.md](docs/MOBILE_USER_GUIDE.md)（英文）。

## 目前狀態

| 功能 | 桌面（Qt） | Android | iOS |
|---|---|---|---|
| 準備／專注／休息流程、停止作廢 | ✅ | ✅ | ✅（邏輯層） |
| 專注守護（離開次數） | ✅ | ✅ | 僅能偵測 App 進入背景 |
| 前景 App 偵測、放行清單與熱鍵啟動 | ✅ | ✅（需授權使用情況存取，無熱鍵） | ❌（系統不允許） |
| 成功／失敗紀錄 | ✅ | ✅ | ✅ |
| 統計頁、設定頁、圓環介面 | ✅ | ❌（最小畫面） | ❌（最小畫面） |
| 勿擾／專注鎖定 | 硬鎖 + 全螢幕（見上） | 勿擾模式（需授權） | ❌ |
| 音量 50% | ✅ | ✅ | ❌（只能調語音音量） |
| 語音提醒 | ✅ | ✅ | ✅ |
| 瀏覽器守護（非白名單網站顯示專注守護） | ❌ 尚未實作 | ❌ | ❌ |

已知限制：

- 硬鎖無法攔截 Ctrl+Alt+Del（Windows）與電源鍵，Wayland 不支援硬鎖；詳見「硬鎖與放行 App」。
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

Qt 不在預設路徑時加上 `-DCMAKE_PREFIX_PATH=<Qt 6.8.3 路徑>`。語音提醒需另外安裝 Qt 的 Qt Speech 與 Qt Multimedia 模組。以上是開發者自行編譯才需要，一般使用者請下載 Release。

```bash
# Android（需要 Android SDK、JDK 17、Gradle 8.9）
cd android && gradle assembleDebug

# iOS（需要 macOS + Xcode、JDK 11、Gradle 7.6；產生未簽署的 .ipa）
cd ios && gradle createIPA
```

## 授權

尚未指定授權條款，公開前請補上 `LICENSE`。
