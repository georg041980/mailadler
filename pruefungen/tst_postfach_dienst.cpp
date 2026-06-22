#include <QtCore>
#include <QtTest>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include "dienst/postfach_dienst.h"
#include "speicher/zwischenspeicher.h"
#include "protokoll/imap_verbindung.h"
#include "kern/nachricht.h"

using AdlerMail::Dienst::PostfachDienst;
using AdlerMail::Speicher::Zwischenspeicher;
using AdlerMail::Protokoll::ImapVerbindung;
using AdlerMail::Kern::Nachricht;

// ---------------------------------------------------------------------------
// Einfacher Mock-IMAP-Server für Integrationstests
// ---------------------------------------------------------------------------
class MiniImapServer : public QObject {
    Q_OBJECT
public:
    explicit MiniImapServer(QObject *eltern = nullptr) : QObject(eltern) {
        m_server = new QTcpServer(this);
        connect(m_server, &QTcpServer::newConnection, this, [this]() {
            m_client = m_server->nextPendingConnection();
            connect(m_client, &QTcpSocket::readyRead, this, [this]() {
                auto zeile = m_client->readAll().trimmed();
                if (zeile.isEmpty()) return;
                auto tag = zeile.left(zeile.indexOf(' '));
                if (zeile.contains("LOGIN"))
                    m_client->write(tag + " OK LOGIN ok\r\n");
                else if (zeile.contains("LIST")) {
                    m_client->write("* LIST (\\HasNoChildren) \"/\" \"INBOX\"\r\n");
                    m_client->write("* LIST (\\HasNoChildren) \"/\" \"Gesendet\"\r\n");
                    m_client->write(tag + " OK LIST done\r\n");
                }
            });
            m_client->write("* OK Mock bereit\r\n");
        });
    }
    bool starte() { return m_server->listen(QHostAddress::LocalHost, 0); }
    quint16 port() const { return m_server->serverPort(); }

private:
    QTcpServer *m_server = nullptr;
    QTcpSocket *m_client = nullptr;
};

// ---------------------------------------------------------------------------
class TestPostfachDienst : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        m_cache = new Zwischenspeicher(this);
        m_dienst = new PostfachDienst(m_cache, this);
    }

    void sollteLeerStarten() {
        QVERIFY(m_dienst->nachrichten().isEmpty());
        QCOMPARE(m_dienst->anzahl(), 0);
    }

    void sollteNachrichtenSetzen() {
        Nachricht n;
        n.id = 1; n.betreff = "Test";
        m_dienst->setzeNachrichten({n});
        QCOMPARE(m_dienst->anzahl(), 1);
        QCOMPARE(m_dienst->nachrichten()[0].betreff, "Test");
    }

    void sollteSignaleSenden() {
        QSignalSpy spion(m_dienst, &PostfachDienst::nachrichtenGeaendert);
        Nachricht n;
        m_dienst->setzeNachrichten({n});
        QCOMPARE(spion.count(), 1);
    }

    void sollteOrdnerUeberImapLaden() {
        MiniImapServer server;
        QVERIFY(server.starte());

        ImapVerbindung imap;
        imap.setzeTls(false);
        imap.setzeServer("127.0.0.1");
        imap.setzePort(server.port());

        PostfachDienst dienst(m_cache);
        dienst.setzeImapVerbindung(&imap);

        QSignalSpy verbunden(&imap, &ImapVerbindung::verbunden);
        QSignalSpy angemeldet(&imap, &ImapVerbindung::angemeldet);
        QSignalSpy ordner(&dienst, &PostfachDienst::ordnerListeGeaendert);

        imap.verbinden();
        QVERIFY(verbunden.wait(3000));
        imap.anmelden("u", "p");
        QVERIFY(angemeldet.wait(3000));

        dienst.ordnerLaden();
        QVERIFY(ordner.wait(3000));
        QCOMPARE(ordner.count(), 1);

        QStringList liste = ordner[0][0].toStringList();
        QCOMPARE(liste.size(), 2);
        QCOMPARE(liste[0], "INBOX");
        QCOMPARE(liste[1], "Gesendet");
    }

    void sollteFehlerOhneImapMelden() {
        PostfachDienst dienst(m_cache);
        QSignalSpy fehler(&dienst, &PostfachDienst::fehlerAufgetreten);

        dienst.ordnerLaden();
        QCOMPARE(fehler.count(), 1);
    }

private:
    Zwischenspeicher *m_cache = nullptr;
    PostfachDienst *m_dienst = nullptr;
};

QTEST_MAIN(TestPostfachDienst)
#include "tst_postfach_dienst.moc"
