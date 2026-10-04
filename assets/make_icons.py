#!/usr/bin/env python3
"""用純 Python 繪製番茄圖示並輸出各平台需要的尺寸（不需要 Pillow）。
輸出：assets/tomato.png / .ico / .icns、Android mipmap、iOS 圖示。用法：python3 assets/make_icons.py"""
import math, os, struct, zlib
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
def png(w, h, rows):
    def chunk(t, d):
        c = struct.pack(">I", len(d)) + t + d
        return c + struct.pack(">I", zlib.crc32(t + d) & 0xffffffff)
    raw = b"".join(b"\x00" + bytes(r) for r in rows)
    return b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 6, 0, 0, 0)) + chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b"")
def pixel(u, v, bg):
    """u,v 為 0..1 的座標；回傳 RGBA。bg=None 為透明背景，否則是圓角方形底色。"""
    cx, cy, r = 0.5, 0.56, 0.37
    if ((u - .5) / .13) ** 2 + ((v - .19) / .05) ** 2 < 1 or (abs(u - .5) < .025 and .13 < v < .22):
        return (48, 164, 108, 255)                                   # 葉子與蒂
    d = math.hypot(u - cx, v - cy)
    if d < r:
        k = 1 - 0.35 * max(0, ((u - cx) + (v - cy)) / 0.78)
        return (int(229 * k), int(72 * k), int(77 * k), 255)         # 番茄身體
    if bg is not None:
        q = 0.22                                                       # 圓角方形底色
        x, y = abs(u - .5) - (.5 - q), abs(v - .5) - (.5 - q)
        if bg == "square" or max(x, y) <= 0 or math.hypot(max(x, 0), max(y, 0)) <= q:
            return (250, 247, 242, 255)
    return (0, 0, 0, 0)
def render(size, bg=None, ss=2):
    rows = []
    for y in range(size):
        row = bytearray()
        for x in range(size):
            acc = [0, 0, 0, 0]
            for sy in range(ss):
                for sx in range(ss):
                    p = pixel((x + (sx + .5) / ss) / size, (y + (sy + .5) / ss) / size, bg)
                    a = p[3]; acc[0] += p[0] * a; acc[1] += p[1] * a; acc[2] += p[2] * a; acc[3] += a
            n = ss * ss
            if acc[3]: row += bytes((acc[0] // acc[3], acc[1] // acc[3], acc[2] // acc[3], acc[3] // n))
            else: row += b"\x00\x00\x00\x00"
        rows.append(row)
    return png(size, size, rows)
def write(path, data):
    path = os.path.join(ROOT, path); os.makedirs(os.path.dirname(path), exist_ok=True)
    open(path, "wb").write(data); print("寫入", os.path.relpath(path, ROOT), len(data), "bytes")
p256, p512 = render(256), render(512)
write("assets/tomato.png", p256)
write("assets/tomato.ico", struct.pack("<HHH", 0, 1, 1) + struct.pack("<BBBBHHII", 0, 0, 0, 0, 1, 32, len(p256), 22) + p256)
icns = b"ic08" + struct.pack(">I", 8 + len(p256)) + p256 + b"ic09" + struct.pack(">I", 8 + len(p512)) + p512
write("assets/tomato.icns", b"icns" + struct.pack(">I", 8 + len(icns)) + icns)
for name, px in (("mdpi", 48), ("hdpi", 72), ("xhdpi", 96), ("xxhdpi", 144), ("xxxhdpi", 192)):
    write(f"android/app/src/main/res/mipmap-{name}/ic_launcher.png", render(px, bg="round"))
for name, px in (("Icon-60@2x.png", 120), ("Icon-60@3x.png", 180), ("Icon-76.png", 76), ("Icon-76@2x.png", 152), ("Icon-83.5@2x.png", 167)):
    write(f"ios/resources/{name}", render(px, bg="square"))   # iOS 圖示必須不透明，系統會自己裁圓角
