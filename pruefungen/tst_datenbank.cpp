#include <QtCore>
#include <QtTest>
#include "speicher/datenbank.h"
#include "kern/konto.h"
#include "kern/nachricht.h"

using AdlerMail::Speicher::Datenbank;
using AdlerMail::Kern::Konto;

class TestDatenbank : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        m_db = new Datenbank(this);
        QVERIFY(m_db->oeffne(":memory:"));
    }

    void cleanupTestCase() {
        m_db->schliesse();
    }

    // --- Phase 1: Öffnen/Schließen ---
    void sollteOeffnenUndSchliessen() {
        Datenbank db;
        QVERIFY(db.oeffne(":memory:"));
        QVERIFY(db.istOffen());
        db.schliesse();
        QVERIFY(!db.istOffen());
    }

    void sollteFehlerBeiUngueltigemPfadMelden() {
        Datenbank db;
        QSignalSpy spion(&db, &Datenbank::fehlerAufgetreten);

        bool ok = db.oeffne("/ungueltiger/pfad/der/nicht/existiert/db.sqlite");
        QVERIFY(!ok);
        QCOMPARE(spion.count(), 1);
    }

    // --- Phase 2: Konto-CRUD ---
    void sollteKontoSpeichern() {
        Konto konto;
        konto.email = "max@beispiel.de";
        konto.name = "Max Mustermann";
        konto.imapServer = "imap.beispiel.de";
        konto.imapPort = 993;
        konto.smtpServer = "smtp.beispiel.de";
        konto.smtpPort = 587;
        konto.benutzer = "max@beispiel.de";
        konto.passwort = "geheim123";
        konto.istAktiv = true;

        qint64 id = m_db->kontoSpeichern(konto);
        QVERIFY(id > 0);
        QCOMPARE(id, 1);
    }

    void sollteKontoLesen() {
        auto konten = m_db->alleKonten();
        QCOMPARE(konten.size(), 1);
        QCOMPARE(konten[0].email, "max@beispiel.de");
    }

    void sollteKontoLoeschen() {
        auto konten = m_db->alleKonten();
        QCOMPARE(konten.size(), 1);
        qint64 id = konten[0].id;

        QVERIFY(m_db->kontoLoeschen(id));
        QCOMPARE(m_db->alleKonten().size(), 0);
    }

    void sollteDoppelteEmailAblehnen() {
        Konto konto;
        konto.email = "eindeutig@beispiel.de";
        konto.imapServer = "imap.test.de";
        konto.smtpServer = "smtp.test.de";
        konto.benutzer = "eindeutig@beispiel.de";

        qint64 id1 = m_db->kontoSpeichern(konto);
        QVERIFY(id1 > 0);

        konto.imapServer = "anderer.de";
        qint64 id2 = m_db->kontoSpeichern(konto);
        QCOMPARE(id2, -1);
    }

    // --- Phase 3: Nachrichten-CRUD ---
    void sollteNachrichtSpeichern() {
        AdlerMail::Kern::Nachricht n;
        n.absender = "test@example.com";
        n.betreff  = "Testnachricht";
        n.inhalt   = "Das ist der Inhalt.";
        n.datum    = QDateTime::currentDateTime();

        qint64 id = m_db->nachrichtSpeichern(n);
        QVERIFY(id > 0);
    }

    void sollteNachrichtenFuerOrdnerLesen() {
        auto nachrichten = m_db->nachrichtenFuerOrdner("INBOX");
        QVERIFY(nachrichten.size() >= 1);
        QCOMPARE(nachrichten[0].absender, "test@example.com");
        QCOMPARE(nachrichten[0].betreff, "Testnachricht");
    }

    void sollteNachrichtAlsGelesenMarkieren() {
        auto nachrichten = m_db->nachrichtenFuerOrdner("INBOX");
        QVERIFY(!nachrichten.isEmpty());
        QVERIFY(m_db->nachrichtAlsGelesenMarkieren(nachrichten[0].id));
    }

    void sollteNachrichtenLoeschen() {
        m_db->nachrichtenLoeschenFuerOrdner("INBOX");
        auto nachrichten = m_db->nachrichtenFuerOrdner("INBOX");
        QCOMPARE(nachrichten.size(), 0);
    }

private:
    Datenbank *m_db = nullptr;
};

QTEST_MAIN(TestDatenbank)
#include "tst_datenbank.moc"
