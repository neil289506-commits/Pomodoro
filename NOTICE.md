# 授權與第三方聲明（Licensing & Third-Party Notices）

## 本專案
**TomatoGuard**（`qt-core/`、`windows/`、`macos/`、`linux/`、`i18n/`、`assets/`、`.github/` 與文件）以 **GNU General Public License v3.0（SPDX：`GPL-3.0-only`）** 授權，全文見 [LICENSE](LICENSE)。

Copyright (C) 2026 neil289506-commits and contributors.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

## 內含的第三方專案（原始碼在本倉庫）

手機版**不是本專案自己寫的**，而是下列兩個開源專案的 fork（原始碼快照）。兩者都是 GPL-3.0，我們保留其授權文件與著作權聲明，並依 GPL 的要求公開原始碼。

| 目錄 | 上游專案 | 作者 | 授權 | 快照版本 |
|---|---|---|---|---|
| [`android/`](android/UPSTREAM.md) | [adrcotfas/goodtime](https://github.com/adrcotfas/goodtime)（Goodtime Productivity） | Adrian Cotfas | GPL-3.0（原始碼檔頭為 GPL-3.0-or-later） | `6c1ddc51bc59379fbec70b6048e0be9cbde30c30` |
| [`ios/`](ios/UPSTREAM.md) | [po-gl/pomodoro](https://github.com/po-gl/pomodoro) | Porter Glines | GPL-3.0 | `9db4bc18e7283d370c0134e0c6fac6978d11fedb` |

各上游目錄保留了它自己的授權文件（`android/COPYING.md`、`ios/LICENSE`）與原始碼檔頭，請不要移除。我們對上游**沒有修改原始碼**，只做了「刪除不需要的檔案」與「加入 UPSTREAM.md」，詳見各目錄的 `UPSTREAM.md`。

這些專案的名稱、圖示與商標屬於其作者。本倉庫是獨立的 fork，**與原作者沒有隸屬或背書關係**，有問題請先確認是否為上游本身的問題。

## 桌面版使用的函式庫
- **Qt 6**（Widgets、TextToSpeech、Multimedia）：以 LGPL-3.0 / GPL-3.0 授權，動態連結；發佈的桌面版 zip 內含 Qt 的函式庫，原始碼見 <https://code.qt.io/>。
