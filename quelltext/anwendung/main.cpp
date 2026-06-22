// AdlerMail — Einstiegspunkt
#include <QtCore/QDateTime>
#include <QtCore/QUrl>
#include <QtCore/QVector>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>

#include "ansichtmodelle/nachrichten_liste_modell.h"
#include "ansichtmodelle/erstellen_ansicht_modell.h"
#include "ansichtmodelle/ordner_liste_modell.h"
#include "kern/nachricht.h"

using AdlerMail::NachrichtenListeModell;
using AdlerMail::ErstellenAnsichtModell;
using AdlerMail::OrdnerListeModell;
using AdlerMail::Kern::Nachricht;

int main(int anzahlArgumente, char *argumente[])
{
    QGuiApplication anwendung(anzahlArgumente, argumente);
    anwendung.setApplicationName("AdlerMail");
    anwendung.setApplicationVersion("0.1.0");
    anwendung.setOrganizationName("AdlerMail");

    // --- ViewModels ---

    auto *nachrichtenModell = new NachrichtenListeModell(&anwendung);
    QVector<Nachricht> testDaten = {
        {1, "max@beispiel.de", "Willkommen bei AdlerMail",
         "Hallo! Dies ist Ihre erste E-Mail in AdlerMail.\n\n"
         "AdlerMail ist ein plattformübergreifender E-Mail-Client, "
         "entwickelt mit Qt 6 und QML.", "",
         QDateTime::currentDateTime(), false, false},
        {2, "info@qt.io", "Qt 6.7 veröffentlicht",
         "Das Qt-Team freut sich, Qt 6.7 anzukündigen.\n\n"
         "Neue Features: Verbesserte QML-Engine, schnellere ListViews, "
         "und vieles mehr.", "",
         QDateTime::currentDateTime().addSecs(-3600), false, false},
        {3, "github@notifications.com", "Neuer Commit in mailadler",
         "georg0480 hat einen neuen Commit gepusht:\n\n"
         "protokoll: IMAP-Verbindung mit echtem Protokoll", "",
         QDateTime::currentDateTime().addSecs(-7200), true, false},
    };
    nachrichtenModell->setzeNachrichten(testDaten);

    auto *erstellenModell = new ErstellenAnsichtModell(&anwendung);

    auto *ordnerModell = new OrdnerListeModell(&anwendung);
    ordnerModell->setzeOrdner({"INBOX", "Gesendet", "Entwürfe", "Papierkorb"});

    // --- QML starten ---

    QQmlApplicationEngine maschine;
    maschine.rootContext()->setContextProperty("nachrichtenListeModell", nachrichtenModell);
    maschine.rootContext()->setContextProperty("erstellenAnsichtModell", erstellenModell);
    maschine.rootContext()->setContextProperty("ordnerListeModell", ordnerModell);
    maschine.load(QUrl("qrc:/AdlerMail/HauptFenster.qml"));

    if (maschine.rootObjects().isEmpty()) {
        return -1;
    }

    return anwendung.exec();
}
