# iOS：po-gl/pomodoro（上游 fork）

| 項目 | 內容 |
|---|---|
| 上游 | https://github.com/po-gl/pomodoro |
| 作者 | Porter Glines |
| 授權 | GNU GPL v3（`LICENSE`） |
| 快照 commit | `9db4bc18e7283d370c0134e0c6fac6978d11fedb`（2024-10-26） |
| 取得日期 | 2026-10-06 |

## 我們做了什麼
- **沒有修改任何原始碼。** 只刪除 `.git/` 與 `ci_scripts/`（Xcode Cloud 腳本），並加入本檔案；上游 README 改名為 `README.upstream.md`。

## 如何編譯
由 [`.github/workflows/ios.yml`](../.github/workflows/ios.yml) 在 macOS 上用 `xcodebuild archive` 編譯 `Pomodoro` scheme（iOS App，內含 watchOS App 與小工具），**不簽署**，再打包成 `.ipa`：

```bash
cd ios && xcodebuild archive -project Pomodoro.xcodeproj -scheme Pomodoro -configuration Release \
  -destination 'generic/platform=iOS' -archivePath build/Pomodoro.xcarchive \
  CODE_SIGNING_ALLOWED=NO CODE_SIGNING_REQUIRED=NO CODE_SIGN_IDENTITY=""
```

## 編譯時產生的檔案
上游把 `Shared/Env.plist` 放在 `.gitignore`（內容是作者自己的即時動態伺服器網址），但專案會把它打包，缺檔時 `xcodebuild` 會失敗。CI 在編譯前產生一個**占位的** `Env.plist`（`serverURL` 為上游程式碼缺檔時的預設值 `http://127.0.0.1:9000`），所以側載版本不會連到任何伺服器。這個檔案不會被提交到倉庫。

## 已知限制（來自上游）
- IPA **未簽署**，無法直接安裝，需要用 AltStore / Sideloadly 等工具以你自己的 Apple ID 重新簽署（見 [手機安裝教學](../docs/MOBILE_USER_GUIDE.zh-TW.md)）。
- 即時動態（Live Activity）依賴上游作者另外維護的伺服器（po-gl/pomodoro-notification-service），自行側載的版本**不會有即時動態**。
- 上游專案最後更新於 2024-10，最低支援 iOS 17、watchOS 10；上游 README 註明該 App 曾被 App Store 拒絕上架。
- 側載用免費 Apple ID 簽署時，App Groups、Live Activities 等需要付費開發者帳號的功能（含手錶 App 與小工具）可能無法使用。
