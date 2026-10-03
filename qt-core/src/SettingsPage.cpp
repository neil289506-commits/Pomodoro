#include "pomo/SettingsPage.h"
#include "pomo/Settings.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <functional>
namespace {
QWidget* makeEditor(const QString& title, const QString& hint, const QStringList& init,
                    std::function<void(const QStringList&)> onChange, QWidget* parent) {
    auto* box = new QGroupBox(title, parent); auto* l = new QVBoxLayout(box);
    auto* list = new QListWidget; list->addItems(init);
    auto* edit = new QLineEdit; edit->setPlaceholderText(hint);
    auto* add = new QPushButton("新增"); auto* del = new QPushButton("移除選取");
    auto* row = new QHBoxLayout; row->addWidget(edit, 1); row->addWidget(add);
    l->addWidget(list); l->addLayout(row); l->addWidget(del);
    auto collect = [list, onChange] { QStringList v; for (int i = 0; i < list->count(); ++i) v << list->item(i)->text(); onChange(v); };
    auto doAdd = [=] { const QString t = edit->text().trimmed(); if (t.isEmpty()) return; list->addItem(t); edit->clear(); collect(); };
    QObject::connect(add, &QPushButton::clicked, box, doAdd);
    QObject::connect(edit, &QLineEdit::returnPressed, box, doAdd);
    QObject::connect(del, &QPushButton::clicked, box, [=] { qDeleteAll(list->selectedItems()); collect(); });
    return box;
}
}
namespace pomo {
SettingsPage::SettingsPage(QWidget* p) : QWidget(p) {
    auto& s = Settings::instance(); auto* l = new QVBoxLayout(this);
    l->addWidget(makeEditor("專注中放行的 App", "程式名稱，例如 Code / code.exe", s.allowedApps(),
        [this](const QStringList& v) { Settings::instance().setAllowedApps(v); emit allowedAppsChanged(QSet<QString>(v.begin(), v.end())); }, this));
    l->addWidget(makeEditor("專注中放行的網站", "網域，例如 docs.qt.io", s.allowedSites(),
        [](const QStringList& v) { Settings::instance().setAllowedSites(v); }, this));
}
}
