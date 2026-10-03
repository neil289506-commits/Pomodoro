#include "pomo/History.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
namespace pomo {
QString History::path() {
    const QString d = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(d); return d + "/history.jsonl";
}
void History::record(int index, bool success, const QString& reason) {
    QFile f(path()); if (!f.open(QIODevice::Append | QIODevice::Text)) return;
    QJsonObject o{{"time", QDateTime::currentDateTime().toString(Qt::ISODate)}, {"index", index},
                  {"result", success ? "success" : "fail"}, {"reason", reason}};
    f.write(QJsonDocument(o).toJson(QJsonDocument::Compact) + "\n");
}
QJsonArray History::load() {
    QJsonArray a; QFile f(path()); if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return a;
    while (!f.atEnd()) a.append(QJsonDocument::fromJson(f.readLine()).object());
    return a;
}
}
