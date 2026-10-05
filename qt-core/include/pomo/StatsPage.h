#pragma once
#include <QWidget>
class QLabel; class QListWidget;
namespace pomo {
// 紀錄頁：三張統計卡片（完成、作廢、最佳連勝）與逐筆明細（新的在上）
class StatsPage : public QWidget {
    Q_OBJECT
public:
    explicit StatsPage(QWidget* parent = nullptr);
public slots:
    void refresh();
private:
    QLabel *done_, *voided_, *best_, *doneCap_, *voidedCap_, *bestCap_, *empty_;
    QListWidget* list_;
};
}
