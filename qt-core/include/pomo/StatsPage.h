#pragma once
#include <QWidget>
class QLabel; class QTableWidget;
namespace pomo {
// 紀錄頁：成功 / 失敗統計、連勝、逐筆明細（新的在上）
class StatsPage : public QWidget {
    Q_OBJECT
public:
    explicit StatsPage(QWidget* parent = nullptr);
public slots:
    void refresh();
private:
    QLabel* summary_;
    QTableWidget* table_;
};
}
