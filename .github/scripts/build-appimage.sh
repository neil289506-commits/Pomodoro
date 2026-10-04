#!/usr/bin/env bash
# 用 linuxdeploy 把 Qt 與相依函式庫一起打包成 AppImage，使用者不需要安裝 Qt。
# 用法：build-appimage.sh <build 目錄> <x86_64|aarch64> [輸出資料夾]
set -euo pipefail
# 失敗時把最後的輸出變成註解（完整記錄需要登入才能看）
exec > >(tee appimage.log) 2>&1
trap 'sleep 1; msg=$(tail -n 14 appimage.log | cut -c1-200); msg="${msg//$'"'"'\n'"'"'/%0A}"; echo "::error::AppImage 失敗，最後輸出：%0A${msg}"' ERR
BUILD_DIR="$1"; ARCH="$2"; OUT="${3:-dist-appimage}"
: "${QT_ROOT_DIR:?需要 QT_ROOT_DIR（由 install-qt-action 設定）}"

rm -rf appdir "$OUT" tools; mkdir -p appdir/usr/bin "$OUT" tools
cp "$BUILD_DIR/pomodoro" appdir/usr/bin/
cp assets/tomato.png tomato.png
cat > tomato.desktop <<'DESKTOP'
[Desktop Entry]
Type=Application
Name=TomatoGuard
Comment=Pomodoro focus timer with focus guard
Exec=pomodoro
Icon=tomato
Categories=Utility;
Terminal=false
DESKTOP

base=https://github.com/linuxdeploy
curl -fsSL -o tools/linuxdeploy "$base/linuxdeploy/releases/download/continuous/linuxdeploy-$ARCH.AppImage"
curl -fsSL -o tools/linuxdeploy-plugin-qt "$base/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-$ARCH.AppImage"
chmod +x tools/*
export PATH="$PWD/tools:$PATH"
export APPIMAGE_EXTRACT_AND_RUN=1                      # CI 沒有 FUSE
export QMAKE="$QT_ROOT_DIR/bin/qmake"
export LD_LIBRARY_PATH="$QT_ROOT_DIR/lib:${LD_LIBRARY_PATH:-}"
# 本程式只用語音提醒（speechd 後端），不播放媒體：移除 GStreamer 播放外掛，避免把一大堆 gst 函式庫塞進 AppImage
rm -f "$QT_ROOT_DIR"/plugins/multimedia/libgstreamermediaplugin.so
export EXTRA_QT_PLUGINS="texttospeech"                 # 語音提醒需要
export EXTRA_PLATFORM_PLUGINS="libqoffscreen.so"       # 讓無螢幕環境也能做啟動測試

tools/linuxdeploy --appdir appdir --executable appdir/usr/bin/pomodoro \
  --desktop-file tomato.desktop --icon-file tomato.png --plugin qt --output appimage
mv ./*.AppImage "$OUT/TomatoGuard-$ARCH.AppImage"
ls -la "$OUT"
