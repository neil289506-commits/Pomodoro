#pragma once
#include <QSettings>
#include <QStringList>
namespace pomo {
// 使用者設定：番茄鐘數、放行 App、放行網站（瀏覽器守護用）
class Settings {
public:
    static Settings& instance();
    int rounds() const { return s_.value("rounds", 4).toInt(); }
    void setRounds(int n) { s_.setValue("rounds", n); }
    QStringList allowedApps() const { return s_.value("allowedApps").toStringList(); }
    void setAllowedApps(const QStringList& v) { s_.setValue("allowedApps", v); }
    QStringList allowedSites() const { return s_.value("allowedSites").toStringList(); }
    void setAllowedSites(const QStringList& v) { s_.setValue("allowedSites", v); }
private:
    Settings() = default;
    QSettings s_;
};
}
