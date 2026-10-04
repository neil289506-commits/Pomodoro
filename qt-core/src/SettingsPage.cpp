#include "pomo/SettingsPage.h"
#include "pomo/AppInfo.h"
#include "pomo/Config.h"
#include "pomo/I18n.h"
#include "pomo/Settings.h"
#include <QComboBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <functional>
namespace pomo {
namespace {
QLabel* muted(const QString& text, QWidget* parent) { auto* l = new QLabel(text, parent); l->setProperty("muted", true); l->setWordWrap(true); return l; }
QWidget* makeEditor(const QString& title, const QString& hint, const QString& placeholder, const QStringList& init,
                    std::function<void(const QStringList&)> onChange, QWidget* parent, bool browse) {
    auto* box = new QGroupBox(title, parent); auto* l = new QVBoxLayout(box); l->setSpacing(10);
    l->addWidget(muted(hint, box));
    auto* list = new QListWidget; list->addItems(init); list->setMinimumHeight(110);
    auto* edit = new QLineEdit; edit->setPlaceholderText(placeholder);
    auto* add = new QPushButton(T("settings.add")); auto* del = new QPushButton(T("settings.remove"));
    auto* row = new QHBoxLayout; row->setSpacing(8); row->addWidget(edit, 1);
    if (browse) {   // 選擇程式後存成完整路徑，專注中才能用熱鍵直接開啟
        auto* pick = new QPushButton(T("settings.browse")); row->addWidget(pick);
        QObject::connect(pick, &QPushButton::clicked, box, [=] {
            const QString f = QFileDialog::getOpenFileName(box, T("settings.apps.title"));
            if (!f.isEmpty()) edit->setText(f);
        });
    }
    row->addWidget(add);
    l->addWidget(list); l->addLayout(row); l->addWidget(del);
    auto collect = [list, onChange] { QStringList v; for (int i = 0; i < list->count(); ++i) v << list->item(i)->text(); onChange(v); };
    auto doAdd = [=] { const QString t = edit->text().trimmed(); if (t.isEmpty()) return; list->addItem(t); edit->clear(); collect(); };
    QObject::connect(add, &QPushButton::clicked, box, doAdd);
    QObject::connect(edit, &QLineEdit::returnPressed, box, doAdd);
    QObject::connect(del, &QPushButton::clicked, box, [=] { qDeleteAll(list->selectedItems()); collect(); });
    return box;
}
}
SettingsPage::SettingsPage(QWidget* p) : QWidget(p) {
    auto* l = new QVBoxLayout(this); l->setContentsMargins(0, 0, 0, 0);
    scroll_ = new QScrollArea; scroll_->setWidgetResizable(true); scroll_->setFrameShape(QFrame::NoFrame);
    l->addWidget(scroll_);
    build();
    connect(&I18n::instance(), &I18n::languageChanged, this, [this] { QTimer::singleShot(0, this, [this] { build(); }); });
}
void SettingsPage::build() {
    auto* page = new QWidget; auto* l = new QVBoxLayout(page); l->setContentsMargins(20, 20, 20, 20); l->setSpacing(14);
    auto& s = Settings::instance(); auto& i18n = I18n::instance();
    // 語言
    auto* lang = new QGroupBox(T("settings.language"), page); auto* ll = new QVBoxLayout(lang);
    auto* combo = new QComboBox; combo->addItem(T("settings.language.auto"), "auto");
    for (const auto& g : i18n.languages()) combo->addItem(g.name, g.code);
    combo->setCurrentIndex(qMax(0, combo->findData(i18n.preference())));
    connect(combo, &QComboBox::activated, this, [combo](int i) { I18n::instance().setPreference(combo->itemData(i).toString()); });
    ll->addWidget(combo); l->addWidget(lang);
    // 放行清單
    l->addWidget(makeEditor(T("settings.apps.title"), T("settings.apps.hint"), T("settings.placeholder.app"), s.allowedApps(),
        [this](const QStringList& v) { Settings::instance().setAllowedApps(v); emit allowedAppsChanged(QSet<QString>(v.begin(), v.end())); }, page, true));
    l->addWidget(makeEditor(T("settings.sites.title"), T("settings.sites.hint"), T("settings.placeholder.site"), s.allowedSites(),
        [](const QStringList& v) { Settings::instance().setAllowedSites(v); }, page, false));
    // 熱鍵
    l->addWidget(muted(T("settings.hotkey").arg(app::hotkeyText()), page));
    // 關於
    auto* about = new QGroupBox(T("settings.about"), page); auto* al = new QVBoxLayout(about);
    auto* name = new QLabel(app::name(), about); name->setProperty("title", true);
    al->addWidget(name); al->addWidget(muted(T("about.tagline"), about)); al->addWidget(muted(T("settings.version").arg(app::version()), about));
    l->addWidget(about); l->addStretch(1);
    scroll_->setWidget(page);   // 舊的內容會被 QScrollArea 刪除
}
}
