// AdlerMail — Einstiegspunkt
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QObject>
#include <QtCore/QStandardPaths>
#include <QtCore/QUrl>
#include <QtCore/QVector>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>

#include "ansichtmodelle/erstellen_ansicht_modell.h"
#include "ansichtmodelle/konto_ansicht_modell.h"
#include "ansichtmodelle/konto_auswahl_modell.h"
#include "ansichtmodelle/nachricht_ansicht_modell.h"
#include "ansichtmodelle/nachrichten_liste_modell.h"
#include "ansichtmodelle/ordner_liste_modell.h"
#include "dienst/konto_dienst.h"
#include "dienst/postfach_dienst.h"
#include "kern/nachricht.h"
#include "protokoll/imap_verbindung.h"
#include "protokoll/smtp_verbindung.h"
#include "speicher/datenbank.h"
#include "speicher/zwischenspeicher.h"

using AdlerMail::ErstellenAnsichtModell;
using AdlerMail::KontoAnsichtModell;
using AdlerMail::KontoAuswahlModell;
using AdlerMail::NachrichtAnsichtModell;
using AdlerMail::NachrichtenListeModell;
using AdlerMail::OrdnerListeModell;
using AdlerMail::Dienst::KontoDienst;
using AdlerMail::Dienst::PostfachDienst;
using AdlerMail::Kern::Nachricht;
using AdlerMail::Protokoll::ImapVerbindung;
using AdlerMail::Protokoll::SmtpVerbindung;
using AdlerMail::Speicher::Datenbank;
using AdlerMail::Speicher::Zwischenspeicher;

int main(int anzahlArgumente, char* argumente[])
{
    QGuiApplication anwendung(anzahlArgumente, argumente);
    anwendung.setApplicationName("AdlerMail");
    anwendung.setApplicationVersion("1.0.0");
    anwendung.setOrganizationName("AdlerMail");

    // --- Datenbank ---

    auto datenpfad = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(datenpfad);
    auto* datenbank = new Datenbank(&anwendung);
    datenbank->oeffne(datenpfad + "/adlermail.db");

    // --- Dienste ---

    auto* kontoDienst = new KontoDienst(datenbank, &anwendung);
    auto* cache = new Zwischenspeicher(&anwendung);
    auto* postfachDienst = new PostfachDienst(cache, &anwendung);
    postfachDienst->setzeDatenbank(datenbank);

    // --- ViewModels ---

    auto* nachrichtenModell = new NachrichtenListeModell(&anwendung);
    QVector<Nachricht> testDaten = {
        {1, "max@beispiel.de", "Willkommen bei AdlerMail",
         "Hallo! Dies ist Ihre erste E-Mail in AdlerMail.\n\n"
         "AdlerMail ist ein plattformübergreifender E-Mail-Client, "
         "entwickelt mit Qt 6 und QML.",
         "", QDateTime::currentDateTime(), false, false},
        {2, "info@qt.io", "Qt 6.7 veröffentlicht",
         "Das Qt-Team freut sich, Qt 6.7 anzukündigen.\n\n"
         "Neue Features: Verbesserte QML-Engine, schnellere ListViews, "
         "und vieles mehr.",
         "", QDateTime::currentDateTime().addSecs(-3600), false, false},
        {3, "github@notifications.com", "Neuer Commit in mailadler",
         "georg0480 hat einen neuen Commit gepusht:\n\n"
         "protokoll: IMAP-Verbindung mit echtem Protokoll",
         "", QDateTime::currentDateTime().addSecs(-7200), true, false},
    };
    nachrichtenModell->setzeNachrichten(testDaten);

    auto* erstellenModell = new ErstellenAnsichtModell(&anwendung);
    auto* ordnerModell = new OrdnerListeModell(&anwendung);
    ordnerModell->setzeOrdner({"INBOX", "Gesendet", "Entwürfe", "Papierkorb"});
    auto* nachrichtAnsichtModell = new NachrichtAnsichtModell(&anwendung);
    nachrichtAnsichtModell->setzeNachricht(testDaten[0]);

    auto* kontoAnsichtModell = new KontoAnsichtModell(&anwendung);

    auto konten = kontoDienst->alleKonten();
    auto* kontoAuswahlModell = new KontoAuswahlModell(&anwendung);
    kontoAuswahlModell->setzeKonten(konten);

    // --- Verdrahtung ---

    QObject::connect(postfachDienst, &PostfachDienst::ordnerListeGeaendert, ordnerModell,
                     &OrdnerListeModell::setzeOrdner);

    QObject::connect(kontoAnsichtModell, &KontoAnsichtModell::speichernAngefordert, kontoDienst,
                     [kontoDienst](const AdlerMail::Kern::Konto& k)
                     {
                         kontoDienst->kontoAnlegen(k.email, k.name, k.imapServer, k.imapPort, k.smtpServer, k.smtpPort,
                                                   k.benutzer, k.passwort);
                     });

    QObject::connect(kontoAnsichtModell, &KontoAnsichtModell::loeschenAngefordert, kontoDienst,
                     [kontoDienst](qint64 id) { kontoDienst->kontoLoeschen(id); });

    // SMTP
    auto* smtp = new SmtpVerbindung(&anwendung);
    QObject::connect(erstellenModell, &ErstellenAnsichtModell::sendeAngefordert, smtp,
                     [smtp, erstellenModell]()
                     {
                         smtp->sende(erstellenModell->an(), {erstellenModell->an()}, erstellenModell->betreff(),
                                     erstellenModell->inhalt());
                     });

    QObject::connect(erstellenModell, &ErstellenAnsichtModell::entwurfSpeichernAngefordert, datenbank,
                     [erstellenModell, datenbank]()
                     {
                         AdlerMail::Kern::Nachricht n;
                         n.absender = "ich@adlermail.de";
                         n.betreff = erstellenModell->betreff();
                         n.inhalt = erstellenModell->inhalt();
                         n.datum = QDateTime::currentDateTime();
                         n.istEntwurf = true;
                         datenbank->nachrichtSpeichern(n);
                     });

    // --- Startup: erstes Konto → IMAP verbinden ---

    if (!konten.isEmpty())
    {
        auto k = konten[0];
        auto* imap = new ImapVerbindung(&anwendung);
        imap->setzeServer(k.imapServer);
        imap->setzePort(k.imapPort);
        postfachDienst->setzeImapVerbindung(imap);

        QObject::connect(imap, &ImapVerbindung::verbunden, imap,
                         [imap, k]() { imap->anmelden(k.benutzer, k.passwort); });
        QObject::connect(imap, &ImapVerbindung::angemeldet, postfachDienst,
                         [postfachDienst]() { postfachDienst->ordnerLaden(); });
        QObject::connect(postfachDienst, &PostfachDienst::ordnerListeGeaendert, postfachDienst,
                         [postfachDienst](const QStringList& ordner)
                         {
                             if (!ordner.isEmpty())
                                 postfachDienst->nachrichtenLaden(ordner[0]);
                         });
        QObject::connect(postfachDienst, &PostfachDienst::nachrichtenGeaendert, nachrichtenModell,
                         [postfachDienst, nachrichtenModell, nachrichtAnsichtModell]()
                         {
                             nachrichtenModell->setzeNachrichten(postfachDienst->nachrichten());
                             nachrichtAnsichtModell->setzeNachricht(postfachDienst->nachrichten().value(0));
                         });

        imap->verbinden();
    }

    // --- QML starten ---

    QQmlApplicationEngine maschine;
    maschine.rootContext()->setContextProperty("nachrichtenListeModell", nachrichtenModell);
    maschine.rootContext()->setContextProperty("erstellenAnsichtModell", erstellenModell);
    maschine.rootContext()->setContextProperty("ordnerListeModell", ordnerModell);
    maschine.rootContext()->setContextProperty("nachrichtAnsichtModell", nachrichtAnsichtModell);
    maschine.rootContext()->setContextProperty("kontoAnsichtModell", kontoAnsichtModell);
    maschine.rootContext()->setContextProperty("kontoAuswahlModell", kontoAuswahlModell);
    maschine.rootContext()->setContextProperty("postfachDienst", postfachDienst);
    maschine.load(QUrl("qrc:/AdlerMail/HauptFenster.qml"));

    if (maschine.rootObjects().isEmpty())
        return -1;
    return anwendung.exec();
}
