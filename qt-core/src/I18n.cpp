#include "pomo/I18n.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QSettings>
namespace pomo {
I18n& I18n::instance() { static I18n i; return i; }
I18n::I18n() {
    QFile f(QStringLiteral(":/i18n/strings.json"));
    if (f.open(QIODevice::ReadOnly)) {
        const QJsonObject root = QJsonDocument::fromJson(f.readAll()).object();
        for (const auto& v : root.value("languages").toArray())
            langs_.append({v.toObject().value("code").toString(), v.toObject().value("name").toString()});
        const QJsonObject s = root.value("strings").toObject();
        for (auto it = s.begin(); it != s.end(); ++it) {
            QHash<QString, QString> m;
            const QJsonObject o = it.value().toObject();
            for (auto j = o.begin(); j != o.end(); ++j) m.insert(j.key(), j.value().toString());
            strings_.insert(it.key(), m);
        }
    }
    current_ = resolve(preference());
}
QString I18n::preference() const { return QSettings().value("language", "auto").toString(); }
QString I18n::resolve(const QString& pref) const {
    auto has = [this](const QString& c) { for (const auto& l : langs_) if (l.code == c) return true; return false; };
    if (pref != QStringLiteral("auto") && has(pref)) return pref;
    const QLocale sys = QLocale::system();
    const QString lang = sys.name().section('_', 0, 0);   // 例如 zh_TW -> zh
    if (lang == QStringLiteral("zh")) {   // 繁體：台灣、香港、澳門或繁體字腳本，其餘視為簡體
        const bool trad = sys.territory() == QLocale::Taiwan || sys.territory() == QLocale::HongKong ||
                          sys.territory() == QLocale::Macao || sys.script() == QLocale::TraditionalHanScript;
        return trad ? QStringLiteral("zh-TW") : QStringLiteral("zh-CN");
    }
    return has(lang) ? lang : QStringLiteral("en");
}
void I18n::setPreference(const QString& pref) {
    QSettings().setValue("language", pref);
    const QString next = resolve(pref);
    if (next == current_) return;
    current_ = next;
    emit languageChanged();
}
QString I18n::t(const QString& key) const {
    const auto it = strings_.constFind(key);
    if (it == strings_.constEnd()) return key;
    return it->value(current_, it->value(QStringLiteral("en"), key));
}
}
