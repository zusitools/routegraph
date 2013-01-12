#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // Set application name for settings. This has to be set before creating the MainWindow
    QCoreApplication::setOrganizationName("Zusitools");
    QCoreApplication::setApplicationName("Route Graph");

    QApplication a(argc, argv);
    MainWindow w;

    w.show();
    return a.exec();
}
