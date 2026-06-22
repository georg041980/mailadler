// AdlerMail — Einstiegspunkt
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>

int main(int anzahlArgumente, char *argumente[])
{
    QGuiApplication anwendung(anzahlArgumente, argumente);
    anwendung.setApplicationName("AdlerMail");
    anwendung.setApplicationVersion("0.1.0");
    anwendung.setOrganizationName("AdlerMail");

    QQmlApplicationEngine maschine;
    maschine.loadFromModule("AdlerMail", "HauptFenster");

    if (maschine.rootObjects().isEmpty()) {
        return -1;
    }

    return anwendung.exec();
}
