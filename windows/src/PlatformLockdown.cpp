#include "pomo/Lockdown.h"
#include <QMetaObject>
#include <windows.h>
namespace pomo {
namespace {
HHOOK g_hook = nullptr;
bool g_paused = false;
ILockdown* g_owner = nullptr;
LRESULT CALLBACK keyProc(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION) {
        const auto* k = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lp);
        const bool down = wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN;
        const bool alt = (k->flags & LLKHF_ALTDOWN) != 0;
        const bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        const bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        const DWORD v = k->vkCode;
        if (v == 'A' && ctrl && alt && shift) {  // 開啟放行 App 熱鍵（Ctrl+Alt+Shift+A），一律攔截
            if (down && g_owner) QMetaObject::invokeMethod(g_owner, "hotkeyPressed", Qt::QueuedConnection);
            return 1;
        }
        if (!g_paused) {
            if (v == VK_LWIN || v == VK_RWIN || v == VK_APPS) return 1;                 // 開始功能表 / Win 快捷鍵
            if (alt && (v == VK_TAB || v == VK_ESCAPE || v == VK_F4 || v == VK_SPACE)) return 1;  // 切換 / 關閉 / 系統功能表
            if (ctrl && v == VK_ESCAPE) return 1;                                       // Ctrl+Esc（含 Ctrl+Shift+Esc 工作管理員）
        }
    }
    return CallNextHookEx(g_hook, code, wp, lp);
}
}
// Windows：WH_KEYBOARD_LL 低階鍵盤 hook。Ctrl+Alt+Del 由系統處理，應用程式無法攔截。
class WinLockdown final : public ILockdown {
public:
    explicit WinLockdown(QObject* parent) : ILockdown(parent) {}
    ~WinLockdown() override { release(); }
    bool engage(WId) override {
        g_paused = false; g_owner = this;
        if (g_hook) return true;
        g_hook = SetWindowsHookExW(WH_KEYBOARD_LL, keyProc, GetModuleHandleW(nullptr), 0);
        if (!g_hook) { emit failed(QStringLiteral("無法安裝鍵盤攔截（錯誤碼 %1）").arg(GetLastError())); return false; }
        return true;
    }
    void release() override {
        if (g_hook) { UnhookWindowsHookEx(g_hook); g_hook = nullptr; }
        if (g_owner == this) g_owner = nullptr;
    }
    void setPaused(bool p) override { g_paused = p; }
    QString describe() const override {
        return QStringLiteral("硬鎖（鍵盤 hook）：Win 鍵、Alt+Tab、Alt+Esc、Alt+F4、Ctrl+Esc、Ctrl+Shift+Esc 已攔截。無法攔截 Ctrl+Alt+Del。");
    }
};
std::unique_ptr<ILockdown> createPlatformLockdown(QObject* parent) { return std::make_unique<WinLockdown>(parent); }
}
