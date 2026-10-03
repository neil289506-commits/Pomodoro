#include "pomo/StatsPage.h"
#include "pomo/History.h"
#include "pomo/Stats.h"
#include <QHeaderView>
#include <QJsonObject>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>
namespace pomo {
StatsPage::StatsPage(QWidget* p) : QWidget(p) {
    summary_ = new QLabel; table_ = new QTableWidget(0, 4);
    table_->setHorizontalHeaderLabels({"時間", "第幾個", "結果", "原因"});
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    auto* l = new QVBoxLayout(this); l->addWidget(summary_); l->addWidget(table_);
    refresh();
}
void StatsPage::refresh() {
    const QJsonArray h = History::load(); const Stats s = computeStats(h);
    summary_->setText(QString("共 %1 個　成功 %2　失敗 %3　目前連勝 %4　最佳連勝 %5").arg(s.total).arg(s.success).arg(s.fail).arg(s.streak).arg(s.bestStreak));
    table_->setRowCount(0);
    for (int i = h.size() - 1; i >= 0; --i) {
        const QJsonObject o = h[i].toObject(); const int row = table_->rowCount(); table_->insertRow(row);
        const bool ok = o["result"].toString() == "success";
        table_->setItem(row, 0, new QTableWidgetItem(o["time"].toString()));
        table_->setItem(row, 1, new QTableWidgetItem(QString::number(o["index"].toInt())));
        table_->setItem(row, 2, new QTableWidgetItem(ok ? "成功" : "失敗"));
        table_->setItem(row, 3, new QTableWidgetItem(o["reason"].toString()));
    }
}
}
