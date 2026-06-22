// AdlerMail — Einstiegspunkt
#include <QtCore/QUrl>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>

int main(int anzahlArgumente, char *argumente[])
{
    QGuiApplication anwendung(anzahlArgumente, argumente);
    anwendung.setApplicationName("AdlerMail");
    anwendung.setApplicationVersion("0.1.0");
    anwendung.setOrganizationName("AdlerMail");

    QQmlApplicationEngine maschine;
    maschine.load(QUrl("qrc:/AdlerMail/HauptFenster.qml"));

    if (maschine.rootObjects().isEmpty()) {
        return -1;
    }

    return anwendung.exec();
}
