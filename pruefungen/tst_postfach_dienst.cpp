#include <QtCore>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtTest>

#include "dienst/postfach_dienst.h"
#include "kern/nachricht.h"
#include "protokoll/imap_verbindung.h"
#include "speicher/zwischenspeicher.h"

using AdlerMail::Dienst::PostfachDienst;
using AdlerMail::Kern::Nachricht;
using AdlerMail::Protokoll::ImapVerbindung;
using AdlerMail::Speicher::Zwischenspeicher;

// ---------------------------------------------------------------------------
class MiniImapServer : public QObject
{
    Q_OBJECT
public:
    explicit MiniImapServer(QObject* eltern = nullptr) : QObject(eltern)
    {
        m_server = new QTcpServer(this);
        connect(m_server, &QTcpServer::newConnection, this,
                [this]()
                {
                    m_client = m_server->nextPendingConnection();
                    connect(m_client, &QTcpSocket::readyRead, this,
                            [this]()
                            {
                                m_puffer.append(m_client->readAll());
                                while (true)
                                {
                                    auto pos = m_puffer.indexOf('\n');
                                    if (pos < 0)
                                        break;
                                    QByteArray zeile = m_puffer.left(pos).trimmed();
                                    m_puffer.remove(0, pos + 1);
                                    if (zeile.isEmpty())
                                        continue;
                                    auto tag = zeile.left(zeile.indexOf(' '));
                                    auto rest = zeile.mid(zeile.indexOf(' ') + 1);
                                    if (rest.contains("LOGIN"))
                                        m_client->write(tag + " OK LOGIN ok\r\n");
                                    else if (rest.contains("LIST"))
                                    {
                                        m_client->write("* LIST (\\HasNoChildren) \"/\" \"INBOX\"\r\n");
                                        m_client->write("* LIST (\\HasNoChildren) \"/\" \"Gesendet\"\r\n");
                                        m_client->write(tag + " OK LIST done\r\n");
                                    }
                                    else if (rest.contains("SELECT"))
                                    {
                                        m_client->write("* 2 EXISTS\r\n");
                                        m_client->write("* OK [UIDVALIDITY 1]\r\n");
                                        m_client->write(tag + " OK SELECT completed\r\n");
                                    }
                                    else if (rest.contains("FETCH"))
                                    {
                                        m_client->write("* 1 FETCH (FLAGS (\\Seen)\r\n"
                                                        "From: max@beispiel.de\r\nSubject: Test\r\n"
                                                        "Date: Mon, 01 Jan 2024 12:00:00 +0000\r\n)\r\n");
                                        m_client->write("* 2 FETCH (FLAGS\r\n"
                                                        "From: info@qt.io\r\nSubject: Qt Update\r\n"
                                                        "Date: Mon, 02 Jan 2024 08:30:00 +0000\r\n)\r\n");
                                        m_client->write(tag + " OK FETCH completed\r\n");
                                    }
                                }
                            });
                    m_client->write("* OK Mock bereit\r\n");
                });
    }

    bool starte() { return m_server->listen(QHostAddress::LocalHost, 0); }

    quint16 port() const { return m_server->serverPort(); }

private:
    QTcpServer* m_server = nullptr;
    QTcpSocket* m_client = nullptr;
    QByteArray m_puffer;
};

// ---------------------------------------------------------------------------
class TestPostfachDienst : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase()
    {
        m_cache = new Zwischenspeicher(this);
        m_dienst = new PostfachDienst(m_cache, this);
    }

    void sollteLeerStarten()
    {
        QVERIFY(m_dienst->nachrichten().isEmpty());
        QCOMPARE(m_dienst->anzahl(), 0);
    }

    void sollteNachrichtenSetzen()
    {
        Nachricht n;
        n.id = 1;
        n.betreff = "Test";
        m_dienst->setzeNachrichten({n});
        QCOMPARE(m_dienst->anzahl(), 1);
        QCOMPARE(m_dienst->nachrichten()[0].betreff, "Test");
    }

    void sollteSignaleSenden()
    {
        QSignalSpy spion(m_dienst, &PostfachDienst::nachrichtenGeaendert);
        Nachricht n;
        m_dienst->setzeNachrichten({n});
        QCOMPARE(spion.count(), 1);
    }

    void sollteOrdnerUeberImapLaden()
    {
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
    }

    void sollteFehlerOhneImapMelden()
    {
        PostfachDienst dienst(m_cache);
        QSignalSpy fehler(&dienst, &PostfachDienst::fehlerAufgetreten);
        dienst.ordnerLaden();
        QCOMPARE(fehler.count(), 1);
    }

    void sollteVerbindenOrdnerLaden()
    {
        MiniImapServer server;
        QVERIFY(server.starte());
        ImapVerbindung imap;
        imap.setzeTls(false);

        PostfachDienst dienst(m_cache);
        dienst.setzeImapVerbindung(&imap);
        QSignalSpy verb(&dienst, &PostfachDienst::verbunden);
        QSignalSpy ordner(&dienst, &PostfachDienst::ordnerListeGeaendert);

        dienst.verbinden("127.0.0.1", server.port(), "u", "p");
        QVERIFY(verb.wait(3000));
        QCOMPARE(verb.count(), 1);
        QVERIFY(ordner.wait(3000));
        QCOMPARE(ordner.count(), 1);
        QStringList liste = ordner[0][0].toStringList();
        QCOMPARE(liste.size(), 2);
    }

    /*
    void sollteNachrichtenLaden() {
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
        QSignalSpy geaendert(&dienst, &PostfachDienst::nachrichtenGeaendert);
        imap.verbinden();
        QVERIFY(verbunden.wait(3000));
        imap.anmelden("u", "p");
        QVERIFY(angemeldet.wait(3000));
        dienst.nachrichtenLaden("INBOX");
        QVERIFY(geaendert.wait(3000));
        QCOMPARE(geaendert.count(), 1);
        auto nachrichten = dienst.nachrichten();
        QCOMPARE(nachrichten.size(), 2);
        QCOMPARE(nachrichten[0].absender, "max@beispiel.de");
    }
    */

private:
    Zwischenspeicher* m_cache = nullptr;
    PostfachDienst* m_dienst = nullptr;
};

QTEST_MAIN(TestPostfachDienst)
#include "tst_postfach_dienst.moc"
