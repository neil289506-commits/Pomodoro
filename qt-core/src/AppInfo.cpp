#include "pomo/AppInfo.h"
#include "pomo/Config.h"
#include <QCoreApplication>
#include <QKeySequence>
namespace pomo::app {
QString hotkeyText() { return QKeySequence(QString::fromLatin1(kLauncherHotkey)).toString(QKeySequence::NativeText); }
QString name() { return QStringLiteral("番茄守護 TomatoGuard"); }
QString version() { return QStringLiteral("0.2.0"); }
void applyIdentity() {
    QCoreApplication::setOrganizationName("TomatoGuard");
    QCoreApplication::setApplicationName("TomatoGuard");
    QCoreApplication::setApplicationVersion(version());
}
}
