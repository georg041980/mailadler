// ---------------------------------------------------------------------------
// tst_imap_verbindung.cpp — Testet IMAP-Verbindung mit Mock-Server
// ---------------------------------------------------------------------------

#include <QtCore>
#include <QtTest>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include "protokoll/imap_verbindung.h"

using AdlerMail::Protokoll::ImapVerbindung;

/**
 * Mock-IMAP-Server, der vordefinierte Antworten sendet.
 *
 * Verwendet QTcpServer für lokale Tests ohne echten IMAP-Server.
 * Sendet realistische, aber minimale IMAP-Antworten.
 */
class MockImapServer : public QObject {
    Q_OBJECT
public:
    explicit MockImapServer(QObject *eltern = nullptr)
        : QObject(eltern)
    {
        m_server = new QTcpServer(this);
        connect(m_server, &QTcpServer::newConnection,
                this, &MockImapServer::beiNeuerVerbindung);
    }

    bool starte(quint16 port = 0)
    {
        return m_server->listen(QHostAddress::LocalHost, port);
    }

    quint16 serverPort() const { return m_server->serverPort(); }
    void schliesse() { m_server->close(); }

    /// Konfiguriert, ob LOGIN mit OK oder NO beantwortet wird.
    void setzeAnmeldungErfolgreich(bool ok) { m_anmeldungOk = ok; }

signals:
    void befehlEmpfangen(const QByteArray &befehl);

private slots:
    void beiNeuerVerbindung()
    {
        m_clientSocket = m_server->nextPendingConnection();
        connect(m_clientSocket, &QTcpSocket::readyRead,
                this, &MockImapServer::beiBereitZumLesen);
        connect(m_clientSocket, &QTcpSocket::disconnected,
                m_clientSocket, &QObject::deleteLater);

        // IMAP-Begrüßung senden
        m_clientSocket->write("* OK IMAP4rev1 Mock-Server bereit\r\n");
    }

    void beiBereitZumLesen()
    {
        m_puffer.append(m_clientSocket->readAll());

        while (true) {
            auto pos = m_puffer.indexOf('\n');
            if (pos < 0) break;

            QByteArray zeile = m_puffer.left(pos).trimmed();
            m_puffer.remove(0, pos + 1);

            if (zeile.isEmpty()) continue;
            emit befehlEmpfangen(zeile);
            verarbeiteBefehl(zeile);
        }
    }

private:
    void verarbeiteBefehl(const QByteArray &zeile)
    {
        // Extrahiere Tag (erstes Wort)
        auto leerPos = zeile.indexOf(' ');
        QByteArray tag = (leerPos > 0) ? zeile.left(leerPos) : QByteArray();
        QByteArray rest = (leerPos > 0) ? zeile.mid(leerPos + 1) : zeile;

        if (rest.startsWith("LOGIN")) {
            if (m_anmeldungOk) {
                m_clientSocket->write(tag + " OK LOGIN erfolgreich\r\n");
            } else {
                m_clientSocket->write(tag + " NO LOGIN fehlgeschlagen\r\n");
            }
        } else if (rest.startsWith("LIST")) {
            m_clientSocket->write("* LIST (\\HasNoChildren) \"/\" \"INBOX\"\r\n");
            m_clientSocket->write("* LIST (\\HasNoChildren) \"/\" \"Gesendet\"\r\n");
            m_clientSocket->write("* LIST (\\HasNoChildren) \"/\" \"Entwürfe\"\r\n");
            m_clientSocket->write(tag + " OK LIST abgeschlossen\r\n");
        } else if (rest.startsWith("LOGOUT")) {
            m_clientSocket->write("* BYE Auf Wiedersehen\r\n");
            m_clientSocket->write(tag + " OK LOGOUT\r\n");
            m_clientSocket->disconnectFromHost();
        } else if (rest.startsWith("SELECT")) {
            m_clientSocket->write("* 3 EXISTS\r\n");
            m_clientSocket->write("* 0 RECENT\r\n");
            m_clientSocket->write("* OK [UIDVALIDITY 1]\r\n");
            m_clientSocket->write(tag + " OK SELECT completed\r\n");
        } else if (rest.startsWith("FETCH")) {
            m_clientSocket->write(
                "* 1 FETCH (FLAGS (\\Seen)\r\n"
                "From: max@beispiel.de\r\n"
                "Subject: Test\r\n"
                "Date: Mon, 01 Jan 2024 12:00:00 +0000\r\n"
                ")\r\n");
            m_clientSocket->write(
                "* 2 FETCH (FLAGS\r\n"
                "From: info@qt.io\r\n"
                "Subject: Qt Update\r\n"
                "Date: Mon, 02 Jan 2024 08:30:00 +0000\r\n"
                ")\r\n");
            m_clientSocket->write(tag + " OK FETCH completed\r\n");
        }
    }

    QTcpServer *m_server = nullptr;
    QTcpSocket *m_clientSocket = nullptr;
    QByteArray  m_puffer;
    bool        m_anmeldungOk = true;
};

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

class TestImapVerbindung : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {}
    void cleanupTestCase() {}

    /// Prüft, dass verbinden() das verbunden()-Signal auslöst.
    void sollteVerbinden()
    {
        MockImapServer server;
        QVERIFY(server.starte(0));

        ImapVerbindung verbindung;
        verbindung.setzeTls(false);
        verbindung.setzeServer("127.0.0.1");
        verbindung.setzePort(server.serverPort());

        QSignalSpy spion(&verbindung, &ImapVerbindung::verbunden);

        verbindung.verbinden();
        QVERIFY(spion.wait(3000));
        QCOMPARE(spion.count(), 1);

        QVERIFY(verbindung.istVerbunden());
    }

    /// Prüft erfolgreiche Anmeldung.
    void sollteAnmelden()
    {
        MockImapServer server;
        QVERIFY(server.starte(0));
        server.setzeAnmeldungErfolgreich(true);

        ImapVerbindung verbindung;
        verbindung.setzeTls(false);
        verbindung.setzeServer("127.0.0.1");
        verbindung.setzePort(server.serverPort());

        QSignalSpy verbundenSpion(&verbindung, &ImapVerbindung::verbunden);
        QSignalSpy angemeldetSpion(&verbindung, &ImapVerbindung::angemeldet);

        verbindung.verbinden();
        QVERIFY(verbundenSpion.wait(3000));

        verbindung.anmelden("benutzer", "passwort");
        QVERIFY(angemeldetSpion.wait(3000));
        QCOMPARE(angemeldetSpion.count(), 1);

        QVERIFY(verbindung.istAngemeldet());
    }

    /// Prüft, dass fehlerhafte Anmeldung ein Fehlersignal sendet.
    void sollteFehlerBeiFalscherAnmeldungMelden()
    {
        MockImapServer server;
        QVERIFY(server.starte(0));
        server.setzeAnmeldungErfolgreich(false);

        ImapVerbindung verbindung;
        verbindung.setzeTls(false);
        verbindung.setzeServer("127.0.0.1");
        verbindung.setzePort(server.serverPort());

        QSignalSpy verbundenSpion(&verbindung, &ImapVerbindung::verbunden);
        QSignalSpy fehlerSpion(&verbindung, &ImapVerbindung::fehlerAufgetreten);
        QSignalSpy angemeldetSpion(&verbindung, &ImapVerbindung::angemeldet);

        verbindung.verbinden();
        QVERIFY(verbundenSpion.wait(3000));

        verbindung.anmelden("falsch", "falsch");
        QVERIFY(fehlerSpion.wait(3000));
        QCOMPARE(fehlerSpion.count(), 1);
        QCOMPARE(angemeldetSpion.count(), 0);

        QVERIFY(!verbindung.istAngemeldet());
    }

    /// Prüft, dass ordnerListeAbrufen() die Ordnerliste liefert.
    void sollteOrdnerListeAbrufen()
    {
        MockImapServer server;
        QVERIFY(server.starte(0));

        ImapVerbindung verbindung;
        verbindung.setzeTls(false);
        verbindung.setzeServer("127.0.0.1");
        verbindung.setzePort(server.serverPort());

        QSignalSpy verbundenSpion(&verbindung, &ImapVerbindung::verbunden);
        QSignalSpy angemeldetSpion(&verbindung, &ImapVerbindung::angemeldet);
        QSignalSpy ordnerSpion(&verbindung, &ImapVerbindung::ordnerListeEmpfangen);

        verbindung.verbinden();
        QVERIFY(verbundenSpion.wait(3000));

        verbindung.anmelden("benutzer", "passwort");
        QVERIFY(angemeldetSpion.wait(3000));

        verbindung.ordnerListeAbrufen();
        QVERIFY(ordnerSpion.wait(3000));
        QCOMPARE(ordnerSpion.count(), 1);

        QStringList ordner = ordnerSpion[0][0].toStringList();
        QCOMPARE(ordner.size(), 3);
        QCOMPARE(ordner[0], "INBOX");
        QCOMPARE(ordner[1], "Gesendet");
        QCOMPARE(ordner[2], "Entwürfe");
    }

    /// Prüft, dass ordnerListeAbrufen() ohne Anmeldung einen Fehler meldet.
    void sollteFehlerOhneAnmeldungMelden()
    {
        MockImapServer server;
        QVERIFY(server.starte(0));

        ImapVerbindung verbindung;
        verbindung.setzeTls(false);
        verbindung.setzeServer("127.0.0.1");
        verbindung.setzePort(server.serverPort());

        QSignalSpy verbundenSpion(&verbindung, &ImapVerbindung::verbunden);
        QSignalSpy fehlerSpion(&verbindung, &ImapVerbindung::fehlerAufgetreten);

        verbindung.verbinden();
        QVERIFY(verbundenSpion.wait(3000));

        verbindung.ordnerListeAbrufen();
        QCOMPARE(fehlerSpion.count(), 1);
    }

    /// Prüft, dass doppeltes verbinden() keinen Fehler wirft.
    void sollteDoppeltesVerbindenIgnorieren()
    {
        MockImapServer server;
        QVERIFY(server.starte(0));

        ImapVerbindung verbindung;
        verbindung.setzeTls(false);
        verbindung.setzeServer("127.0.0.1");
        verbindung.setzePort(server.serverPort());

        QSignalSpy spion(&verbindung, &ImapVerbindung::verbunden);

        verbindung.verbinden();
        QVERIFY(spion.wait(3000));
        QCOMPARE(spion.count(), 1);

        // Zweites verbinden() — sollte ignoriert werden
        verbindung.verbinden();
        QCOMPARE(spion.count(), 1);
    }

    /// Prüft SELECT — ordnerAuswaehlen liefert EXISTS-Zähler.
    void sollteOrdnerAuswaehlen()
    {
        MockImapServer server;
        QVERIFY(server.starte(0));

        ImapVerbindung verbindung;
        verbindung.setzeTls(false);
        verbindung.setzeServer("127.0.0.1");
        verbindung.setzePort(server.serverPort());

        QSignalSpy verbunden(&verbindung, &ImapVerbindung::verbunden);
        QSignalSpy angemeldet(&verbindung, &ImapVerbindung::angemeldet);
        QSignalSpy gewaehlt(&verbindung, &ImapVerbindung::ordnerAusgewaehlt);

        verbindung.verbinden();
        QVERIFY(verbunden.wait(3000));
        verbindung.anmelden("u", "p");
        QVERIFY(angemeldet.wait(3000));

        verbindung.ordnerAuswaehlen("INBOX");
        QVERIFY(gewaehlt.wait(3000));
        QCOMPARE(gewaehlt.count(), 1);
        QCOMPARE(gewaehlt[0][0].toInt(), 3);
    }

    /// Prüft FETCH — nachrichtenHeaderAbrufen liefert Header.
    void sollteNachrichtenHeaderAbrufen()
    {
        MockImapServer server;
        QVERIFY(server.starte(0));

        ImapVerbindung verbindung;
        verbindung.setzeTls(false);
        verbindung.setzeServer("127.0.0.1");
        verbindung.setzePort(server.serverPort());

        QSignalSpy verbunden(&verbindung, &ImapVerbindung::verbunden);
        QSignalSpy angemeldet(&verbindung, &ImapVerbindung::angemeldet);
        QSignalSpy header(&verbindung, &ImapVerbindung::nachrichtHeaderEmpfangen);
        QSignalSpy fertig(&verbindung, &ImapVerbindung::nachrichtenHeaderFertig);

        verbindung.verbinden();
        QVERIFY(verbunden.wait(3000));
        verbindung.anmelden("u", "p");
        QVERIFY(angemeldet.wait(3000));

        verbindung.nachrichtenHeaderAbrufen(1, 2);
        QVERIFY(fertig.wait(3000));
        QVERIFY(header.count() >= 1);
        QCOMPARE(header[0][0].value<AdlerMail::Kern::Nachricht>().absender, "max@beispiel.de");
    }
};

QTEST_MAIN(TestImapVerbindung)
#include "tst_imap_verbindung.moc"
