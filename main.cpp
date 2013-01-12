#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // Set application name for settings. This has to be set before creating the MainWindow
    QCoreApplication::setOrganizationName("Zusitools");
    QCoreApplication::setApplicationName("Route Graph");

    QApplication a(argc, argv);

    // Load Qt translations
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    // Load application translations
    QTranslator myappTranslator;
    myappTranslator.load("routegraph_" + QLocale::system().name());
    a.installTranslator(&myappTranslator);

    MainWindow w;
    w.show();
    return a.exec();
}
