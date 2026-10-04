# TomatoGuard 手機安裝指南（繁體中文）

如何從 GitHub Release 安裝 **Android APK** 與 **iPhone / iPad IPA**。英文版：[MOBILE_USER_GUIDE.md](MOBILE_USER_GUIDE.md)

> **早期版本。** 每個版本都標示為 **Pre-release**。手機版的畫面與桌面版相同（計時、紀錄、設定），但功能只有桌面版的一部分。APK 使用 debug 金鑰簽署，IPA **沒有簽署**，所以都無法從應用程式商店安裝，需要依照本指南自行安裝。

---

## 1. 下載正確的檔案

1. 開啟專案的 **Releases** 頁面：<https://github.com/neil289506-commits/Pomodoro/releases>
2. 選擇最新的版本。標籤長得像 `CP#7`，標題長得像 `CP#20261003-154530+7`。
3. 在 **Assets** 下載對應你手機的 zip：

| 裝置 | 檔名 | 內容 |
|---|---|---|
| Android | `TomatoGuard-android-CP<N>.zip` | 一個 `.apk` 檔 |
| iPhone / iPad | `TomatoGuard-ios-arm64-CP<N>.zip` | 一個**未簽署**的 `.ipa` 檔 |

4. **解壓縮**。每個 zip 內還有 `BUILD_INFO.txt`，記錄這個檔案來自哪個 commit 與哪次建置。

請只從本專案官方的 Releases 頁面下載。

---

## 2. 安裝到 Android（APK）

**需求：** Android 8.0（API 26）以上。同一個 APK 同時支援 ARM 與 x86 裝置。

### 方法 A：直接在手機上安裝

1. 把 `.apk` 放到手機上：直接在手機下載並解壓縮，或用 USB、雲端硬碟、任何傳檔 App 從電腦傳過去。
2. 用**檔案**App（或瀏覽器的下載清單）開啟該檔案。
3. Android 會詢問是否允許這個來源安裝 App。點 **設定**，打開 **允許來自這個來源**（針對你開啟檔案的那個 App，例如檔案、Chrome），然後返回。
   - 較舊的 Android：**設定 → 安全性 → 安裝未知應用程式**。
4. 點 **安裝**。
5. 若 **Google Play 安全防護**提示這是未辨識的 App，點 **更多詳細資訊 → 仍要安裝**。這是因為本 App 不是從 Google Play 發佈。
6. 點 **開啟**。

### 方法 B：用電腦的 ADB 安裝（Windows 與 macOS）

1. 在手機啟用**開發人員選項**：**設定 → 關於手機**，連續點 **版本號碼** 七次。
2. 到 **設定 → 系統 → 開發人員選項**，打開 **USB 偵錯**。
3. 從 <https://developer.android.com/tools/releases/platform-tools> 下載 Android 的 **platform-tools** 並解壓縮。
4. 用 USB 線連接手機，並在手機上點 **允許 USB 偵錯？**。
5. 在 platform-tools 資料夾開啟終端機：
   - **Windows**（PowerShell）：`.\adb devices`，接著 `.\adb install -r 路徑\TomatoGuard.apk`
   - **macOS**（終端機）：`./adb devices`，接著 `./adb install -r /路徑/TomatoGuard.apk`
6. 裝置應該顯示為 `device`，安裝指令應該印出 `Success`。

### App 可能要求的權限

| 權限 | 用途 |
|---|---|
| **使用情況存取**（設定 → 應用程式 → 特殊應用程式存取權） | 讓專注守護知道目前在前景的是哪個 App |
| **勿擾權限** | 專注時段靜音通知 |

你可以拒絕；對應的功能就不會運作。

### 更新與解除安裝

- 更新時，直接用新的 APK 覆蓋安裝。
- 如果 Android 提示**簽章衝突**，請先解除安裝舊版（**設定 → 應用程式 → TomatoGuard → 解除安裝**）再安裝。App 內儲存的紀錄會一起被刪除。

---

## 3. 安裝到 iPhone / iPad（IPA）

Apple 只會執行有簽署的 App。Release 裡的 IPA **沒有簽署**，所以必須用你自己的 Apple ID 簽署，這件事由側載工具幫你完成，稱為**側載**或**自行簽署**。

### 準備工作

- 一支 **iOS / iPadOS 15.0 以上**的 iPhone 或 iPad
- 一台 **Windows 電腦或 Mac**
- 一條 USB 線（要支援資料傳輸的 Lightning 或 USB-C 線）
- 一個 **Apple ID**。免費的就可以。建議另外申請一個專門用來側載的 Apple ID。

### 免費 Apple ID 的限制

| 限制 | 說明 |
|---|---|
| 有效期 | App 在 **7 天後就打不開**，必須重新簽署（更新） |
| 同時啟用數量 | 最多同時 **3** 個側載 App |
| 其他限制 | Apple 還限制免費帳號每週可建立的 App ID 與裝置數量 |

付費的 **Apple Developer Program**（每年 99 美元）可把有效期延長到 365 天，Sideloadly 等工具也支援付費帳號。

### 步驟 0：準備 iPhone / iPad

1. 用 USB 線連接電腦，並在裝置上點 **信任**，必要時輸入螢幕鎖定密碼。
2. **開發者模式（iOS 16 以上）：** 開啟 **設定 → 隱私權與安全性 → 開發者模式**，打開它並點 **重新啟動**。重開機後確認 **開啟** 並輸入密碼。
   - 如果看不到開發者模式的開關，先用下方方法 A 或 B 嘗試安裝一次。通常裝過一次開發者簽署的 App 後，這個開關才會出現，之後就能打開。

### 方法 A：Sideloadly（Windows 與 macOS）

Sideloadly 一次完成簽署與安裝。從 <https://sideloadly.io> 下載。

#### Windows

1. 安裝 **Apple 官網版的 iTunes**（<https://www.apple.com/itunes/>）。**不要**用 Microsoft Store 版本，否則 Sideloadly 偵測不到你的裝置。
2. 安裝並開啟 **Sideloadly**。
3. 連接並解鎖 iPhone 或 iPad，它會出現在 **iDevice** 下拉選單中。
4. 把解壓縮後的 `.ipa` 拖進 **IPA** 欄位，或點 IPA 圖示選擇檔案。
5. 在 **Apple account** 欄位輸入你的 **Apple ID**。
6. 點 **Start**，輸入 Apple ID 密碼，若有要求再輸入雙重驗證碼。
7. 等待顯示 **Done**。

#### macOS

1. 開啟下載的 Sideloadly 磁碟映像，把 **Sideloadly** 拖進**應用程式**。
2. 第一次若被 macOS 擋下，到 **系統設定 → 隱私權與安全性** 點 **仍要打開**。
3. 開啟 Sideloadly，連接並解鎖裝置，在 **iDevice** 下拉選單選擇它。
4. 把 `.ipa` 拖進 **IPA** 欄位，輸入你的 **Apple ID**。
5. 點 **Start**，輸入密碼，若有要求再輸入雙重驗證碼。
6. 等待顯示 **Done**。

#### 信任開發者描述檔（兩種系統都要做）

1. 在裝置上開啟 **設定 → 一般 → VPN 與裝置管理**。
2. 在 **開發者 App** 底下點你的 Apple ID，再點 **信任**。
3. 從主畫面開啟 **TomatoGuard**。

### 方法 B：AltStore（Windows 與 macOS）

AltStore 會在裝置上安裝一個 App，只要電腦的 **AltServer** 開著，就能透過 Wi-Fi 幫你自動更新側載的 App。從 <https://altstore.io> 下載。

#### Windows

1. 從 **Apple 官網**（不是 Microsoft Store）安裝 **iTunes** 與 **iCloud**，然後開啟 iCloud 並登入你的 Apple ID。
2. 安裝 **AltServer**，它會以圖示的形式出現在系統匣。
3. 連接裝置。點系統匣的 AltServer 圖示，選 **Install AltStore**，選擇你的裝置並輸入 Apple ID。
4. 在裝置上信任你的 Apple ID 描述檔（**設定 → 一般 → VPN 與裝置管理**）。
5. 把 `.ipa` 放在裝置打得開的地方（**檔案** App、iCloud Drive 或 AirDrop）。
6. 在裝置上開啟 **AltStore**，到 **My Apps**，點 **+**，選擇該 `.ipa`。

#### macOS

1. 安裝 **AltServer**，它會出現在選單列。若 AltServer 要求額外設定，請依照它的提示操作。
2. 連接裝置。點選單列的 AltServer 圖示，選 **Install AltStore**，選擇你的裝置並輸入 Apple ID。
3. 如上所述，在裝置上信任你的 Apple ID 描述檔。
4. 在裝置上開啟 **AltStore**，到 **My Apps**，點 **+**，從**檔案**選擇該 `.ipa`。

#### 讓 App 保持可用

- 只要裝置與電腦在同一個 Wi-Fi 網路、且 AltServer 開著，AltStore 會自動更新 App。
- 如果 App 顯示「不再提供使用」，開啟 AltStore 點 **Refresh All**。

### 更新與重新簽署

- **免費 Apple ID：** 至少每 7 天重新安裝一次（或使用工具的更新功能）。
- **新版本：** 用同樣的方法安裝新的 `.ipa`，會取代舊版並保留你的資料。

---

## 4. 疑難排解

| 問題 | 解法 |
|---|---|
| Android：「未安裝 App」 | 先解除安裝舊版再試一次，並確認儲存空間足夠 |
| Android：安裝被封鎖 | 允許你開啟檔案的那個 App 安裝未知來源 App（第 2 節步驟 3） |
| Android：Play 安全防護警告 | **更多詳細資訊 → 仍要安裝** |
| ADB：`no devices` 或 `unauthorized` | 重新插拔 USB 線，在手機點允許 USB 偵錯，再執行一次 `adb devices` |
| iOS：看不到裝置 | 解鎖裝置、點信任、改用支援資料傳輸的線，（Windows）重新安裝 Apple 官網版的 iTunes |
| iOS：「未受信任的開發者」 | **設定 → 一般 → VPN 與裝置管理 → 開發者 App → 信任** |
| iOS：「需要開發者模式」 | **設定 → 隱私權與安全性 → 開發者模式 → 開啟**，然後重新啟動 |
| iOS：App 一打開就關閉，或顯示「不再提供使用」 | 7 天的簽署過期了，重新更新或重新安裝 |
| iOS：登入失敗 | 檢查雙重驗證碼，或改用另一個 Apple ID 再試 |
| iOS：出現「已達 App 數量上限」 | 刪除另一個側載的 App，或等每週的額度重置 |

---

## 5. 隱私與安全提醒

- 你的 Apple ID 與密碼會透過側載工具傳給 **Apple**，用來產生免費的開發者憑證。請只從官方網站下載工具，並考慮使用另一個 Apple ID。
- 只安裝本專案 **Releases** 頁面的版本。想確認來源時，可以拿 `BUILD_INFO.txt` 的 commit 與 Release 對照。
- 側載的 App 沒有經過 Google 或 Apple 審查。如果不再信任來源，請移除這個 App。

---

## 6. 手機版目前的限制

- 手機版與桌面版外觀一致並支援**六種語言**（在「設定」切換），但功能不完全相同：沒有桌面版那樣的全域熱鍵與硬鎖。Android 改用螢幕固定與勿擾模式。
- **iOS** 無法控制系統音量、無法開啟勿擾模式、也看不到其他 App 是否在前景，所以專注守護只能偵測本 App 是否被切到背景。
- 瀏覽器守護（封鎖未放行的網站）在所有平台都**還沒有實作**。
- 每個版本都是 **Pre-release**，內容可能隨時改變。
