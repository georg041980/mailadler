// AdlerMail — Einstiegspunkt
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
    auto* imap = new ImapVerbindung(&anwendung);
    auto* postfachDienst = new PostfachDienst(cache, &anwendung);
    postfachDienst->setzeDatenbank(datenbank);
    postfachDienst->setzeImapVerbindung(imap);

    // --- ViewModels ---

    auto* nachrichtenModell = new NachrichtenListeModell(&anwendung);
    auto* erstellenModell = new ErstellenAnsichtModell(&anwendung);
    auto* ordnerModell = new OrdnerListeModell(&anwendung);
    auto* nachrichtAnsichtModell = new NachrichtAnsichtModell(&anwendung);
    auto* kontoAnsichtModell = new KontoAnsichtModell(&anwendung);
    auto* kontoAuswahlModell = new KontoAuswahlModell(&anwendung);

    auto konten = kontoDienst->alleKonten();
    kontoAuswahlModell->setzeKonten(konten);

    // --- Verdrahtung ---

    // Ordnerliste → ViewModel
    QObject::connect(postfachDienst, &PostfachDienst::ordnerListeGeaendert, ordnerModell,
                     &OrdnerListeModell::setzeOrdner);

    // Nach neuen Nachrichten: ViewModels aktualisieren
    QObject::connect(postfachDienst, &PostfachDienst::nachrichtenGeaendert,
                     [postfachDienst, nachrichtenModell, nachrichtAnsichtModell]()
                     {
                         nachrichtenModell->setzeNachrichten(postfachDienst->nachrichten());
                         nachrichtAnsichtModell->setzeNachricht(postfachDienst->nachrichten().value(0));
                     });

    // Konto speichern → Datenbank
    QObject::connect(kontoAnsichtModell, &KontoAnsichtModell::speichernAngefordert, kontoDienst,
                     [kontoDienst](const AdlerMail::Kern::Konto& k)
                     {
                         kontoDienst->kontoAnlegen(k.email, k.name, k.imapServer, k.imapPort, k.smtpServer, k.smtpPort,
                                                   k.benutzer, k.passwort, k.signatur);
                     });

    // Konto löschen → Datenbank
    QObject::connect(kontoAnsichtModell, &KontoAnsichtModell::loeschenAngefordert, kontoDienst,
                     [kontoDienst](qint64 id) { kontoDienst->kontoLoeschen(id); });

    // SMTP
    auto* smtp = new SmtpVerbindung(&anwendung);
    QObject::connect(erstellenModell, &ErstellenAnsichtModell::sendeAngefordert, smtp,
                     [smtp, erstellenModell]()
                     {
                         smtp->sende(erstellenModell->an(), {erstellenModell->an()}, {erstellenModell->cc()},
                                     erstellenModell->betreff(), erstellenModell->inhalt());
                     });

    // Entwurf speichern
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

    // Nach Verbindung: Ordner-Liste → ersten Ordner laden
    QObject::connect(postfachDienst, &PostfachDienst::verbunden, postfachDienst,
                     [postfachDienst]() { postfachDienst->ordnerLaden(); });

    // Nach Ordner-Liste: ersten Ordner auswählen → Nachrichten laden
    QObject::connect(postfachDienst, &PostfachDienst::ordnerListeGeaendert, postfachDienst,
                     [postfachDienst](const QStringList& ordner)
                     {
                         if (!ordner.isEmpty())
                             postfachDienst->nachrichtenLaden(ordner[0]);
                     });

    // --- Startup: erstes Konto verbinden ---

    if (!konten.isEmpty())
    {
        auto k = konten[0];
        postfachDienst->verbinden(k.imapServer, k.imapPort, k.benutzer, k.passwort);
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
