#!/usr/bin/env python3
"""Windows ARM64 交叉編譯後，把執行所需的 Qt DLL、外掛與 VC 執行階段複製到輸出資料夾。
windeployqt 在交叉編譯的 ARM64 Qt 上需要寫死路徑的 qtpaths.bat，CI 上不可靠，所以改用 dumpbin 解析相依。
用法：deploy_windows_arm64.py <pomodoro.exe> <ARM64 Qt 根目錄> <輸出資料夾>"""
import glob, os, re, shutil, subprocess, sys

for s in (sys.stdout, sys.stderr):
    s.reconfigure(encoding="utf-8", errors="replace")
exe, qt, out = sys.argv[1:4]
bindir, plugdir = os.path.join(qt, "bin"), os.path.join(qt, "plugins")
PLUGINS = ["platforms", "styles", "imageformats", "iconengines", "generic", "networkinformation", "tls", "texttospeech", "multimedia"]

def dependents(path):
    r = subprocess.run(["dumpbin", "/dependents", path], capture_output=True, text=True)
    return re.findall(r"^\s+(\S+\.dll)\s*$", r.stdout, re.I | re.M)

def is_debug(name, folder):  # qwindowsd.dll 這類除錯版：同資料夾有去掉 d 的對應檔
    stem = name[:-4]
    return stem.endswith("d") and os.path.exists(os.path.join(folder, stem[:-1] + ".dll"))

queue, copied = [exe], set()
for d in PLUGINS:
    src = os.path.join(plugdir, d)
    if not os.path.isdir(src):
        print(f"略過不存在的外掛資料夾：{d}"); continue
    dst = os.path.join(out, d); os.makedirs(dst, exist_ok=True)
    for f in sorted(os.listdir(src)):
        if f.lower().endswith(".dll") and not is_debug(f, src):
            shutil.copy2(os.path.join(src, f), dst); queue.append(os.path.join(dst, f))
while queue:
    for dll in dependents(queue.pop()):
        if dll.lower().startswith("qt6") and dll.lower() not in copied:
            src = os.path.join(bindir, dll)
            if os.path.exists(src):
                shutil.copy2(src, out); copied.add(dll.lower()); queue.append(os.path.join(out, dll))
            else:
                print(f"::warning::找不到 {dll}")
crt = os.environ.get("VCToolsRedistDir", "")
dirs = glob.glob(os.path.join(crt, "arm64", "Microsoft.VC*.CRT")) if crt else []
if not dirs:
    sys.exit("找不到 ARM64 的 VC 執行階段（VCToolsRedistDir）")
for f in glob.glob(os.path.join(dirs[0], "*.dll")):
    shutil.copy2(f, out)
print(f"已複製 {len(copied)} 個 Qt DLL、外掛與 VC 執行階段到 {out}")
if not any(n.startswith("qt6core") for n in copied):
    sys.exit("沒有複製到 Qt6Core.dll，部署失敗")
