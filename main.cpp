#include <QApplication>
#include <QTranslator>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator uebersetzer;
    uebersetzer.load(":/translations/mailadler_de_DE.qm");
    app.installTranslator(&uebersetzer);

    MainWindow w;
    w.show();
    return app.exec();
}