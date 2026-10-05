#include "pomo/LauncherDialog.h"
#include "pomo/AppEntry.h"
#include "pomo/AppInfo.h"
#include "pomo/Config.h"
#include "pomo/I18n.h"
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
namespace pomo {
LauncherDialog::LauncherDialog(const QStringList& entries, QWidget* parent) : QDialog(parent) {
    setWindowTitle(T("launcher.title")); setMinimumWidth(380);
    auto* l = new QVBoxLayout(this); l->setSpacing(12); l->setContentsMargins(20, 20, 20, 20);
    list_ = new QListWidget; list_->addItems(entries);
    auto* go = new QPushButton(T("launcher.open")); go->setProperty("primary", true); go->setEnabled(!entries.isEmpty());
    auto* msg = new QLabel(entries.isEmpty() ? T("launcher.empty") : T("launcher.pick").arg(app::hotkeyText()));
    msg->setProperty(entries.isEmpty() ? "muted" : "title", true); msg->setWordWrap(true);
    if (!entries.isEmpty()) list_->setCurrentRow(0);
    l->addWidget(msg); l->addWidget(list_); l->addWidget(go);
    connect(go, &QPushButton::clicked, this, &LauncherDialog::launchCurrent);
    connect(list_, &QListWidget::itemActivated, this, &LauncherDialog::launchCurrent);
}
void LauncherDialog::launchCurrent() {
    auto* it = list_->currentItem(); if (!it) return;
    const QString entry = it->data(Qt::UserRole).toString().isEmpty() ? it->text() : it->data(Qt::UserRole).toString();
    if (appentry::launch(entry)) { emit launched(entry); accept(); }
    else { it->setData(Qt::UserRole, entry); it->setText(entry + "\n" + T("launcher.failed")); }
}
}
