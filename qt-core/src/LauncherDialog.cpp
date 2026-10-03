#include "pomo/LauncherDialog.h"
#include "pomo/AppEntry.h"
#include "pomo/Config.h"
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
namespace pomo {
LauncherDialog::LauncherDialog(const QStringList& entries, QWidget* parent) : QDialog(parent) {
    setWindowTitle(QStringLiteral("放行的 App"));
    auto* l = new QVBoxLayout(this);
    list_ = new QListWidget; list_->addItems(entries);
    auto* go = new QPushButton(QStringLiteral("開啟")); go->setEnabled(!entries.isEmpty());
    if (entries.isEmpty())
        l->addWidget(new QLabel(QStringLiteral("尚未設定放行的 App。\n請在開始專注前，到「設定」分頁新增。")));
    else { l->addWidget(new QLabel(QStringLiteral("選擇要開啟的 App（%1）").arg(QString::fromLatin1(kLauncherHotkey)))); list_->setCurrentRow(0); }
    l->addWidget(list_); l->addWidget(go);
    connect(go, &QPushButton::clicked, this, &LauncherDialog::launchCurrent);
    connect(list_, &QListWidget::itemActivated, this, &LauncherDialog::launchCurrent);
}
void LauncherDialog::launchCurrent() {
    auto* it = list_->currentItem(); if (!it) return;
    if (appentry::launch(it->text())) { emit launched(it->text()); accept(); }
    else it->setText(it->text() + QStringLiteral("　（無法啟動，請改用完整路徑）"));
}
}
