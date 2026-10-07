# Android：Goodtime Productivity（上游 fork）

| 項目 | 內容 |
|---|---|
| 上游 | https://github.com/adrcotfas/goodtime |
| 作者 | Adrian Cotfas |
| 授權 | GNU GPL v3（`COPYING.md`；原始碼檔頭為「版本 3 或（由你選擇）更新版本」） |
| 快照 commit | `6c1ddc51bc59379fbec70b6048e0be9cbde30c30`（2026-08-02） |
| 取得日期 | 2026-10-06 |

## 我們做了什麼
- **沒有修改任何原始碼。** 只刪除下列不需要的檔案，並加入本檔案：
  - `.git/`、`.github/`（上游 CI 在子目錄不會被 GitHub 讀取）
  - `fastlane/`（商店中繼資料與截圖，約 6 MB）
  - `iosApp/`（Goodtime 自己的 iOS App；本專案的 iOS 版改用 po-gl/pomodoro）
  - `crowdin.yml`、`renovate.json`（上游的翻譯與依賴更新設定）
- 上游的 README 改名為 `README.upstream.md`（其中的圖片連結因為刪掉了 fastlane 而失效）。

## 如何編譯
由 [`.github/workflows/android.yml`](../.github/workflows/android.yml) 編譯 **F-Droid 風味**（`fdroid`，沒有 RevenueCat 等專有函式庫）：

```bash
cd android && ./gradlew :androidApp:assembleFdroidRelease   # 需要 JDK 21
```

## 簽署
- 設定了 GitHub Secrets（`ANDROID_KEYSTORE_BASE64`、`ANDROID_KEYSTORE_PASSWORD`、`ANDROID_KEY_ALIAS`、`ANDROID_KEY_PASSWORD`）時，用你的金鑰簽署，之後各版本可以直接覆蓋更新。
- **沒有設定時**，上游的 Gradle 會退回 debug 金鑰；CI 每次執行的 debug 金鑰都不同，所以**每個版本的簽章都不一樣，更新前必須先解除安裝舊版**。
- 套件名稱（`com.apps.adrcotfas.goodtime`）與上游相同：與 Google Play 或 F-Droid 上的正式版**簽章不同，無法互相覆蓋安裝**，兩者只能擇一。

## 回報問題
這個目錄是上游的快照，功能問題請到上游回報；本倉庫只負責收錄與編譯。要更新時重新取得上游快照並更新上面的 commit。
