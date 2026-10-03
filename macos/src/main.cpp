#include <QApplication>
#include "pomo/AppInfo.h"
#include "pomo/MainWindow.h"
int main(int argc, char** argv) {
    QApplication app(argc, argv);
    pomo::app::applyIdentity();
    pomo::MainWindow w; w.show();
    return app.exec();
}
