#include "pomo/AppEntry.h"
#include <QFileInfo>
#include <QProcess>
namespace pomo::appentry {
QString idOf(const QString& entry) {
    QString name = QFileInfo(entry.trimmed()).fileName();
    if (name.endsWith(QStringLiteral(".app"), Qt::CaseInsensitive)) name.chop(4);
    return name.toLower();
}
bool launch(const QString& entry) {
    const QString e = entry.trimmed();
#ifdef Q_OS_MACOS
    if (e.endsWith(QStringLiteral(".app"), Qt::CaseInsensitive)) return QProcess::startDetached("open", {e});
#endif
    return QProcess::startDetached(e, {});
}
}
