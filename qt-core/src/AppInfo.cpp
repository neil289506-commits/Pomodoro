#include "pomo/AppInfo.h"
#include <QCoreApplication>
namespace pomo::app {
QString name() { return QStringLiteral("番茄守護 TomatoGuard"); }
QString version() { return QStringLiteral("0.2.0"); }
void applyIdentity() {
    QCoreApplication::setOrganizationName("TomatoGuard");
    QCoreApplication::setApplicationName("TomatoGuard");
    QCoreApplication::setApplicationVersion(version());
}
}
