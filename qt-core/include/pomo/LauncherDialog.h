#pragma once
#include <QDialog>
#include <QStringList>
class QListWidget;
namespace pomo {
// 專注中按下熱鍵彈出：列出放行的 App，選一個就啟動它
class LauncherDialog : public QDialog {
    Q_OBJECT
public:
    LauncherDialog(const QStringList& entries, QWidget* parent = nullptr);
signals:
    void launched(const QString& entry);
private:
    void launchCurrent();
    QListWidget* list_;
};
}
