#include <QApplication>
#include <QTranslator>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator uebersetzer;
    bool geladen = uebersetzer.load(":/translations/mailadler_de_DE.qm");
    if (!geladen) {
        qWarning() << "Deutsche Übersetzung konnte nicht geladen werden!";
    }
    app.installTranslator(&uebersetzer);

    MainWindow w;
    w.show();
    return app.exec();
}