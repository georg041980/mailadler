#include "smtp_verbindung.h"

#include <QtCore/QDebug>

namespace AdlerMail
{
namespace Protokoll
{

// ---------------------------------------------------------------------------
SmtpVerbindung::SmtpVerbindung(QObject* eltern) : QObject(eltern)
{
}

SmtpVerbindung::~SmtpVerbindung()
{
    if (m_verbindung)
        m_verbindung->disconnectFromHost();
}

void SmtpVerbindung::setzeServer(const QString& s)
{
    m_server = s;
}

void SmtpVerbindung::setzePort(quint16 p)
{
    m_port = p;
}

void SmtpVerbindung::setzeTls(bool aktiv)
{
    m_tls = aktiv;
}

QString SmtpVerbindung::server() const
{
    return m_server;
}

quint16 SmtpVerbindung::port() const
{
    return m_port;
}

bool SmtpVerbindung::istVerbunden() const
{
    return m_verbindung && m_verbindung->state() == QAbstractSocket::ConnectedState &&
           (m_tls ? m_verbindung->isEncrypted() : true);
}

// ---------------------------------------------------------------------------
void SmtpVerbindung::verbinden()
{
    if (istVerbunden())
        return;

    if (!m_verbindung)
    {
        m_verbindung = new QSslSocket(this);

        if (m_tls)
        {
            connect(m_verbindung, &QSslSocket::encrypted, this,
                    [this]()
                    {
                        qDebug() << "[SMTP] TLS-verschlüsselt";
                        emit verbunden();
                    });
        }
        else
        {
            connect(m_verbindung, &QSslSocket::connected, this,
                    [this]()
                    {
                        qDebug() << "[SMTP] TCP verbunden mit" << m_server << ":" << m_port;
                        emit verbunden();
                    });
        }

        connect(m_verbindung, &QSslSocket::readyRead, this, &SmtpVerbindung::beiBereitZumLesen);
        connect(m_verbindung, &QSslSocket::disconnected, this, &SmtpVerbindung::beiTrennung);
        connect(m_verbindung, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::errorOccurred), this,
                &SmtpVerbindung::beiSocketFehler);
        connect(m_verbindung, &QSslSocket::sslErrors, this, &SmtpVerbindung::beiSslFehlern);
    }

    m_phase = Phase::Verbinden;
    m_puffer.clear();

    if (m_tls)
        m_verbindung->connectToHostEncrypted(m_server, m_port);
    else
        m_verbindung->connectToHost(m_server, m_port);
}

// ---------------------------------------------------------------------------
void SmtpVerbindung::sende(const QString& absender, const QStringList& empfaenger, const QString& betreff,
                           const QString& inhalt)
{
    m_absender = absender;
    m_empfaenger = empfaenger.isEmpty() ? QString() : empfaenger[0];
    m_betreff = betreff;
    m_inhalt = inhalt;

    if (!istVerbunden())
    {
        emit fehlerAufgetreten("Nicht verbunden");
        return;
    }

    m_phase = Phase::Ehlo;
    sendeBefehl("EHLO adlermail");
}

// ---------------------------------------------------------------------------
QByteArray SmtpVerbindung::sendeBefehl(const QByteArray& befehl)
{
    QByteArray zeile = befehl + "\r\n";
    m_verbindung->write(zeile);
    return befehl;
}

// ---------------------------------------------------------------------------
void SmtpVerbindung::beiBereitZumLesen()
{
    m_puffer.append(m_verbindung->readAll());

    while (true)
    {
        auto pos = m_puffer.indexOf('\n');
        if (pos < 0)
            break;

        QByteArray zeile = m_puffer.left(pos).trimmed();
        m_puffer.remove(0, pos + 1);

        if (zeile.isEmpty())
            continue;
        verarbeiteAntwortZeile(zeile);
    }
}

void SmtpVerbindung::verarbeiteAntwortZeile(const QByteArray& zeile)
{
    // SMTP-Antworten: 2xx = OK, 3xx = weiter, 4xx/5xx = Fehler
    const bool ok = istPositiveAntwort(zeile);

    // Mehrzeilige Antworten: Code-Leerzeichen = letzte Zeile, Code-Minus = weiter
    if (zeile.length() >= 4 && zeile[3] == '-')
        return; // Fortsetzungszeile

    if (!ok)
    {
        emit fehlerAufgetreten("SMTP-Fehler: " + QString::fromUtf8(zeile));
        return;
    }

    switch (m_phase)
    {
    case Phase::Verbinden:
        // Begrüßung (220) empfangen → EHLO senden
        m_phase = Phase::Ehlo;
        sendeBefehl("EHLO adlermail");
        break;

    case Phase::Ehlo:
        // EHLO bestätigt — STARTTLS wenn Port 587, sonst direkt AUTH
        if (m_port == 587 && m_tls)
        {
            m_phase = Phase::StartTls;
            sendeBefehl("STARTTLS");
        }
        else
        {
            m_phase = Phase::AuthAnmelden;
            sendeBefehl("AUTH LOGIN");
        }
        break;

    case Phase::StartTls:
        // STARTTLS bestätigt — TLS-Upgrade
        m_phase = Phase::AuthAnmelden;
        connect(
            m_verbindung, &QSslSocket::encrypted, this, [this]() { sendeBefehl("AUTH LOGIN"); },
            Qt::SingleShotConnection);
        m_verbindung->startClientEncryption();
        break;

    case Phase::AuthAnmelden:
        m_phase = Phase::AuthPasswort;
        sendeBefehl("dXNlcg=="); // "user" in Base64 (Dummy)
        break;

    case Phase::AuthPasswort:
        m_phase = Phase::MailFrom;
        sendeBefehl("MAIL FROM:<" + m_absender.toUtf8() + ">");
        break;

    case Phase::MailFrom:
        m_phase = Phase::RcptTo;
        sendeBefehl("RCPT TO:<" + m_empfaenger.toUtf8() + ">");
        break;

    case Phase::RcptTo:
        m_phase = Phase::Daten;
        sendeBefehl("DATA");
        break;

    case Phase::Daten:
        // Server bereit für Inhalt → Nachricht senden
        m_phase = Phase::InhaltSenden;
        m_verbindung->write("Subject: " + m_betreff.toUtf8() + "\r\n" + "From: " + m_absender.toUtf8() + "\r\n" +
                            "To: " + m_empfaenger.toUtf8() + "\r\n" + "\r\n" + m_inhalt.toUtf8() + "\r\n.\r\n");
        break;

    case Phase::InhaltSenden:
        m_phase = Phase::Quit;
        sendeBefehl("QUIT");
        break;

    case Phase::Quit:
        emit gesendet();
        m_phase = Phase::Keiner;
        if (m_verbindung)
            m_verbindung->disconnectFromHost();
        break;

    case Phase::Keiner:
        break;
    }
}

bool SmtpVerbindung::istPositiveAntwort(const QByteArray& zeile) const
{
    if (zeile.length() < 3)
        return false;
    char c = zeile[0];
    return c == '2' || c == '3';
}

// ---------------------------------------------------------------------------
void SmtpVerbindung::beiTrennung()
{
    m_phase = Phase::Keiner;
    m_puffer.clear();
    emit getrennt();
}

void SmtpVerbindung::beiSocketFehler(QAbstractSocket::SocketError fehler)
{
    Q_UNUSED(fehler)
    emit fehlerAufgetreten(m_verbindung ? m_verbindung->errorString() : "Socket-Fehler");
}

void SmtpVerbindung::beiSslFehlern(const QList<QSslError>& fehler)
{
    if (!m_tls)
        return;
    for (const auto& f : fehler)
        qWarning() << "[SMTP] SSL-Fehler:" << f.errorString();
    emit fehlerAufgetreten("SSL/TLS-Fehler bei SMTP-Verbindung");
}

} // namespace Protokoll
} // namespace AdlerMail
