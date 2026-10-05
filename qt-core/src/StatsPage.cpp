#include "pomo/StatsPage.h"
#include "pomo/Config.h"
#include "pomo/History.h"
#include "pomo/I18n.h"
#include "pomo/Stats.h"
#include <QDateTime>
#include <QFrame>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QLabel>
#include <QListWidget>
#include <QLocale>
#include <QVBoxLayout>
namespace pomo {
namespace {
QLabel* makeLabel(const char* prop, QWidget* parent) { auto* l = new QLabel(parent); l->setProperty(prop, true); l->setAlignment(Qt::AlignCenter); return l; }
}
StatsPage::StatsPage(QWidget* p) : QWidget(p) {
    auto* root = new QVBoxLayout(this); root->setContentsMargins(20, 20, 20, 12); root->setSpacing(14);
    auto* cards = new QHBoxLayout; cards->setSpacing(12);
    QLabel** nums[3] = {&done_, &voided_, &best_};
    QLabel** caps[3] = {&doneCap_, &voidedCap_, &bestCap_};
    for (int i = 0; i < 3; ++i) {
        auto* card = new QFrame; card->setProperty("card", true);
        auto* l = new QVBoxLayout(card); l->setContentsMargins(10, 14, 10, 14); l->setSpacing(2);
        *nums[i] = makeLabel("bignum", card); *caps[i] = makeLabel("muted", card);
        (*caps[i])->setWordWrap(true);
        l->addWidget(*nums[i]); l->addWidget(*caps[i]); cards->addWidget(card, 1);
    }
    root->addLayout(cards);
    empty_ = makeLabel("muted", this); empty_->setWordWrap(true);
    list_ = new QListWidget; list_->setSelectionMode(QAbstractItemView::NoSelection); list_->setFocusPolicy(Qt::NoFocus);
    root->addWidget(empty_, 1); root->addWidget(list_, 1);
    refresh();
}
void StatsPage::refresh() {
    const QJsonArray h = History::load(); const Stats s = computeStats(h);
    done_->setText(QString::number(s.success)); doneCap_->setText(T("history.completed"));
    voided_->setText(QString::number(s.fail)); voidedCap_->setText(T("history.voided"));
    best_->setText(QString::number(s.bestStreak)); bestCap_->setText(T("history.best_streak"));
    empty_->setText(T("history.empty")); empty_->setVisible(h.isEmpty()); list_->setVisible(!h.isEmpty());
    list_->clear();
    for (int i = h.size() - 1; i >= 0; --i) {
        const QJsonObject o = h[i].toObject();
        const bool ok = o["result"].toString() == "success";
        const QString raw = o["reason"].toString();
        QString reason = raw;   // 紀錄裡存的是原因代碼；舊版紀錄是文字，直接顯示
        if (raw == "manual") reason = T("reason.manual");
        else if (raw == "leaves") reason = T("reason.leaves").arg(kMaxLeaves);
        const QDateTime dt = QDateTime::fromString(o["time"].toString(), Qt::ISODate);
        QString text = QString("%1  %2").arg(ok ? QStringLiteral("✓") : QStringLiteral("✕"), T("history.index").arg(o["index"].toInt()));
        text += "\n" + (dt.isValid() ? QLocale().toString(dt, QLocale::ShortFormat) : o["time"].toString());
        if (!ok && !reason.isEmpty()) text += "  ·  " + reason;
        auto* it = new QListWidgetItem(text, list_);
        it->setForeground(ok ? QColor("#30A46C") : QColor("#E5484D"));
    }
}
}
