// ---------------------------------------------------------------------------
// tst_smtp_verbindung.cpp — Testet SMTP-Verbindung mit Mock-Server
// ---------------------------------------------------------------------------

#include <QtCore>
#include <QtTest>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include "protokoll/smtp_verbindung.h"

using AdlerMail::Protokoll::SmtpVerbindung;

/**
 * Mock-SMTP-Server, der vordefinierte Antworten sendet.
 */
class MockSmtpServer : public QObject {
    Q_OBJECT
public:
    explicit MockSmtpServer(QObject *eltern = nullptr) : QObject(eltern) {
        m_server = new QTcpServer(this);
        connect(m_server, &QTcpServer::newConnection, this, [this]() {
            m_client = m_server->nextPendingConnection();
            connect(m_client, &QTcpSocket::readyRead, this, [this]() {
                auto zeile = m_client->readAll().trimmed();
                if (zeile.isEmpty()) return;
                if (zeile.startsWith("EHLO"))
                    m_client->write("250-server.example.com\r\n250 STARTTLS\r\n");
                else if (zeile.startsWith("AUTH LOGIN"))
                    m_client->write("334 VXNlcm5hbWU6\r\n");
                else if (zeile == "dXNlcg==")
                    m_client->write("334 UGFzc3dvcmQ6\r\n");
                else if (zeile == "cGFzcw==" || zeile.length() == 8)
                    m_client->write("235 2.7.0 Authentifiziert\r\n");
                else if (zeile.startsWith("MAIL FROM"))
                    m_client->write("250 2.1.0 OK\r\n");
                else if (zeile.startsWith("RCPT TO"))
                    m_client->write("250 2.1.5 OK\r\n");
                else if (zeile == "DATA")
                    m_client->write("354 End data with <CR><LF>.<CR><LF>\r\n");
                else if (zeile == ".")
                    m_client->write("250 2.0.0 OK: queued\r\n");
                else if (zeile == "QUIT")
                    m_client->write("221 2.0.0 Bye\r\n");
            });
            m_client->write("220 mock.smtp.example.com ESMTP bereit\r\n");
        });
    }
    bool starte() { return m_server->listen(QHostAddress::LocalHost, 0); }
    quint16 port() const { return m_server->serverPort(); }

private:
    QTcpServer *m_server = nullptr;
    QTcpSocket *m_client = nullptr;
};

// ---------------------------------------------------------------------------
class TestSmtpVerbindung : public QObject {
    Q_OBJECT

private slots:
    void sollteVerbinden() {
        MockSmtpServer server;
        QVERIFY(server.starte());

        SmtpVerbindung smtp;
        smtp.setzeTls(false);
        smtp.setzeServer("127.0.0.1");
        smtp.setzePort(server.port());

        QSignalSpy spion(&smtp, &SmtpVerbindung::verbunden);
        smtp.verbinden();
        QVERIFY(spion.wait(3000));
        QCOMPARE(spion.count(), 1);
        QVERIFY(smtp.istVerbunden());
    }

    void sollteSenden() {
        MockSmtpServer server;
        QVERIFY(server.starte());

        SmtpVerbindung smtp;
        smtp.setzeTls(false);
        smtp.setzeServer("127.0.0.1");
        smtp.setzePort(server.port());

        QSignalSpy verbunden(&smtp, &SmtpVerbindung::verbunden);
        QSignalSpy gesendet(&smtp, &SmtpVerbindung::gesendet);

        smtp.verbinden();
        QVERIFY(verbunden.wait(3000));

        smtp.sende("von@test.de", {"an@test.de"}, "Betreff", "Inhalt");
        QVERIFY(gesendet.wait(3000));
        QCOMPARE(gesendet.count(), 1);
    }

    void sollteDoppeltesVerbindenIgnorieren() {
        MockSmtpServer server;
        QVERIFY(server.starte());

        SmtpVerbindung smtp;
        smtp.setzeTls(false);
        smtp.setzeServer("127.0.0.1");
        smtp.setzePort(server.port());

        QSignalSpy spion(&smtp, &SmtpVerbindung::verbunden);
        smtp.verbinden();
        QVERIFY(spion.wait(3000));
        smtp.verbinden();
        QCOMPARE(spion.count(), 1);
    }
};

QTEST_MAIN(TestSmtpVerbindung)
#include "tst_smtp_verbindung.moc"
