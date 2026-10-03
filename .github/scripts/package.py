#!/usr/bin/env python3
"""把資料夾內容打包成 zip（保留執行權限與符號連結），並附上 BUILD_INFO.txt。
用法：package.py <輸出.zip> <來源資料夾>"""
import datetime, os, stat, sys, zipfile

# Windows 主控台預設不是 UTF-8，輸出中文會 UnicodeEncodeError
for s in (sys.stdout, sys.stderr):
    s.reconfigure(encoding="utf-8", errors="replace")

def build_info():
    env = os.environ.get
    return "\n".join([
        f"commit: {env('GITHUB_SHA', 'unknown')}",
        f"run:    {env('GITHUB_SERVER_URL', '')}/{env('GITHUB_REPOSITORY', '')}/actions/runs/{env('GITHUB_RUN_ID', '')}",
        f"time:   {datetime.datetime.now(datetime.timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ')}",
        f"zip:    {os.path.basename(sys.argv[1])}", ""])

def main():
    out, src = sys.argv[1], sys.argv[2]
    os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
    count = 0
    with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED) as z:
        for root, dirs, files in os.walk(src):
            dirs.sort()
            links = [d for d in dirs if os.path.islink(os.path.join(root, d))]
            for name in sorted(files + links):
                path = os.path.join(root, name)
                arc = os.path.relpath(path, src).replace(os.sep, "/")
                if os.path.islink(path):  # 符號連結以連結本身存入（macOS .app 需要）
                    zi = zipfile.ZipInfo(arc)
                    zi.create_system = 3
                    zi.external_attr = (stat.S_IFLNK | 0o755) << 16
                    z.writestr(zi, os.readlink(path))
                else:
                    z.write(path, arc)
                count += 1
        if count == 0:
            sys.exit("來源資料夾是空的，沒有可打包的檔案")
        z.writestr("BUILD_INFO.txt", build_info())
    print(f"已建立 {out}（{count} 個檔案）")

main()
