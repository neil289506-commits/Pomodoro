#pragma once
#include <QSet>
#include <QWidget>
namespace pomo {
// 設定頁：放行 App 清單、放行網站清單
class SettingsPage : public QWidget {
    Q_OBJECT
public:
    explicit SettingsPage(QWidget* parent = nullptr);
signals:
    void allowedAppsChanged(const QSet<QString>& ids);
};
}
