#pragma once
#include <QHash>
#include <QList>
#include <QObject>
#include <QString>
namespace pomo {
// 多語系：字串來源是 repo 根目錄的 i18n/strings.json（Android、iOS 也由同一份產生），編進資源檔。
class I18n : public QObject {
    Q_OBJECT
public:
    struct Lang { QString code, name; };
    static I18n& instance();
    QList<Lang> languages() const { return langs_; }
    QString preference() const;                 // "auto" 或語言代碼（使用者在設定頁選的）
    QString language() const { return current_; }  // 實際使用的語言代碼
    void setPreference(const QString& pref);    // 立即套用並發出 languageChanged
    QString t(const QString& key) const;        // 找不到時回傳 key 本身
signals:
    void languageChanged();
private:
    I18n();
    QString resolve(const QString& pref) const;
    QList<Lang> langs_;
    QHash<QString, QHash<QString, QString>> strings_;
    QString current_ = QStringLiteral("en");
};
inline QString T(const char* key) { return I18n::instance().t(QString::fromLatin1(key)); }
}
