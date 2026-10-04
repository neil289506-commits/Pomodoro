#pragma once
#include <QSet>
#include <QWidget>
class QScrollArea;
namespace pomo {
// 設定頁：語言、放行 App、放行網站、熱鍵說明、關於。語言切換時整頁重建。
class SettingsPage : public QWidget {
    Q_OBJECT
public:
    explicit SettingsPage(QWidget* parent = nullptr);
signals:
    void allowedAppsChanged(const QSet<QString>& ids);
private:
    void build();
    QScrollArea* scroll_;
};
}
