#include "imap_verbindung.h"
#include <QtCore/QDebug>
#include <QtCore/QRegularExpression>

namespace AdlerMail { namespace Protokoll {

// ---------------------------------------------------------------------------
// Konstruktor / Destruktor
// ---------------------------------------------------------------------------

ImapVerbindung::ImapVerbindung(QObject *eltern)
    : QObject(eltern)
{
}

ImapVerbindung::~ImapVerbindung()
{
    trennen();
}

// ---------------------------------------------------------------------------
// Getter / Setter
// ---------------------------------------------------------------------------

void ImapVerbindung::setzeServer(const QString &server) { m_server = server; }
void ImapVerbindung::setzePort(quint16 port)            { m_port = port;   }
void ImapVerbindung::setzeTls(bool aktiv)               { m_tls = aktiv;   }

QString ImapVerbindung::server() const { return m_server; }
quint16 ImapVerbindung::port()   const { return m_port;   }
bool    ImapVerbindung::istTls() const { return m_tls;    }

bool ImapVerbindung::istVerbunden() const
{
    return m_verbindung
        && m_verbindung->state() == QAbstractSocket::ConnectedState
        && (m_tls ? m_verbindung->isEncrypted() : true);
}

bool ImapVerbindung::istAngemeldet() const { return m_angemeldet; }

// ---------------------------------------------------------------------------
// Verbindung
// ---------------------------------------------------------------------------

void ImapVerbindung::verbinden()
{
    if (istVerbunden()) return;

    if (!m_verbindung) {
        m_verbindung = new QSslSocket(this);

        if (m_tls) {
            // TLS: verbunden() erst nach Verschlüsselung
            connect(m_verbindung, &QSslSocket::connected,
                    this, &ImapVerbindung::beiVerbunden);
            connect(m_verbindung, &QSslSocket::encrypted,
                    this, &ImapVerbindung::beiVerschluesselt);
        } else {
            // Plain-TCP: verbunden() direkt bei connect
            connect(m_verbindung, &QSslSocket::connected,
                    this, [this]() {
                qDebug() << "[IMAP] TCP verbunden mit" << m_server << ":" << m_port;
                emit verbunden();
            });
        }

        connect(m_verbindung, &QSslSocket::readyRead,
                this, &ImapVerbindung::beiBereitZumLesen);
        connect(m_verbindung, &QSslSocket::disconnected,
                this, &ImapVerbindung::beiTrennung);

        // Fehlersignale (Qt 6.4-kompatibel: QOverload)
        connect(m_verbindung,
                QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::errorOccurred),
                this, &ImapVerbindung::beiSocketFehler);
        connect(m_verbindung, &QSslSocket::sslErrors,
                this, &ImapVerbindung::beiSslFehlern);
    }

    m_aktuellerBefehl = Befehl::Verbinden;
    m_tagZaehler      = 0;
    m_puffer.clear();
    m_angemeldet = false;
    m_willTrennen = false;

    if (m_tls) {
        m_verbindung->connectToHostEncrypted(m_server, m_port);
    } else {
        m_verbindung->connectToHost(m_server, m_port);
    }
}

void ImapVerbindung::trennen()
{
    if (!m_verbindung || !istVerbunden()) return;

    m_willTrennen = true;
    m_aktuellerBefehl = Befehl::Logout;
    sendeBefehl("LOGOUT");
}

// ---------------------------------------------------------------------------
// IMAP-Kommandos
// ---------------------------------------------------------------------------

void ImapVerbindung::anmelden(const QString &benutzer, const QString &passwort)
{
    if (!istVerbunden()) {
        emit fehlerAufgetreten("Nicht verbunden — vor dem Anmelden verbinden() aufrufen");
        return;
    }
    if (m_angemeldet) {
        emit fehlerAufgetreten("Bereits angemeldet");
        return;
    }

    m_aktuellerBefehl = Befehl::Anmelden;
    QByteArray befehl = "LOGIN " + benutzer.toUtf8() + " " + passwort.toUtf8();
    sendeBefehl(befehl);
}

void ImapVerbindung::ordnerListeAbrufen()
{
    if (!m_angemeldet) {
        emit fehlerAufgetreten("Nicht angemeldet — vorher anmelden() aufrufen");
        return;
    }

    m_ordnerListe.clear();
    m_aktuellerBefehl = Befehl::OrdnerListe;
    sendeBefehl("LIST \"\" \"*\"");
}

// ---------------------------------------------------------------------------
// IMAP-Protokoll-Helfer
// ---------------------------------------------------------------------------

QByteArray ImapVerbindung::sendeBefehl(const QByteArray &befehl)
{
    m_tagZaehler++;
    QByteArray tag = "A" + QByteArray::number(m_tagZaehler).rightJustified(4, '0');
    m_letzterTag = tag;

    QByteArray zeile = tag + " " + befehl + "\r\n";
    m_verbindung->write(zeile);

    return tag;
}

// ---------------------------------------------------------------------------
// Antwort-Verarbeitung
// ---------------------------------------------------------------------------

void ImapVerbindung::beiBereitZumLesen()
{
    m_puffer.append(m_verbindung->readAll());

    // Zeilenweise verarbeiten
    while (true) {
        auto pos = m_puffer.indexOf('\n');
        if (pos < 0) break;  // Keine vollständige Zeile

        QByteArray zeile = m_puffer.left(pos).trimmed();
        m_puffer.remove(0, pos + 1);

        if (zeile.isEmpty()) continue;
        verarbeiteAntwortZeile(zeile);
    }
}

void ImapVerbindung::verarbeiteAntwortZeile(const QByteArray &zeile)
{
    // IMAP-Begrüßung: * OK [CAPABILITY ...] ...
    if (zeile.startsWith("* OK") && m_aktuellerBefehl == Befehl::Verbinden) {
        // Begrüßung empfangen — Verbindung bereit
        m_aktuellerBefehl = Befehl::Keiner;
        return;
    }

    // Unaufgeforderte Daten: * LIST ..., * EXISTS, * RECENT, * FLAGS, ...
    if (zeile.startsWith("* ") && m_aktuellerBefehl == Befehl::OrdnerListe) {
        if (istListeZeile(zeile)) {
            QString ordner = parseListeOrdnerName(zeile);
            if (!ordner.isEmpty()) {
                m_ordnerListe.append(ordner);
            }
        }
        return;
    }

    // Statuszeile: TAG OK/NO/BAD [rest]
    if (istStatusZeile(zeile)) {
        verarbeiteStatusZeile(zeile);
        return;
    }

    // BYE vom Server (unaufgefordert)
    if (zeile.startsWith("* BYE")) {
        emit fehlerAufgetreten("Server hat Verbindung beendet: " +
                               QString::fromUtf8(zeile.mid(5).trimmed()));
        return;
    }
}

bool ImapVerbindung::istStatusZeile(const QByteArray &zeile) const
{
    return zeile.startsWith(m_letzterTag + " OK")
        || zeile.startsWith(m_letzterTag + " NO")
        || zeile.startsWith(m_letzterTag + " BAD");
}

void ImapVerbindung::verarbeiteStatusZeile(const QByteArray &zeile)
{
    const bool ok  = zeile.contains(" OK ");
    const bool bad = zeile.contains(" BAD ");

    if (!ok) {
        QByteArray meldung = bad ? "BAD — unbekannter Befehl: " : "NO — fehlgeschlagen: ";
        meldung += zeile.mid(m_letzterTag.size() + 4).trimmed();
        emit fehlerAufgetreten(QString::fromUtf8(meldung));
        return;
    }

    switch (m_aktuellerBefehl) {
    case Befehl::Anmelden:
        m_angemeldet = true;
        emit angemeldet();
        break;

    case Befehl::OrdnerListe:
        emit ordnerListeEmpfangen(m_ordnerListe);
        break;

    case Befehl::Logout:
        // Nichts tun — Trennung folgt über beiTrennung()
        break;

    default:
        break;
    }

    m_aktuellerBefehl = Befehl::Keiner;
}

bool ImapVerbindung::istListeZeile(const QByteArray &zeile) const
{
    // * LIST (...) "/" "INBOX"
    return zeile.startsWith("* LIST ");
}

QString ImapVerbindung::parseListeOrdnerName(const QByteArray &zeile) const
{
    // Extrahiere den letzten quoted-String aus "* LIST (...) "/" "INBOX""
    // Suche nach dem letzten '"..."'  Paar.
    auto ende  = zeile.lastIndexOf('"');
    if (ende < 0) return QString();

    auto start = zeile.lastIndexOf('"', ende - 1);
    if (start < 0) return QString();

    return QString::fromUtf8(zeile.mid(start + 1, ende - start - 1));
}

// ---------------------------------------------------------------------------
// Signal-Handler
// ---------------------------------------------------------------------------

void ImapVerbindung::beiVerbunden()
{
    // TCP verbunden, TLS-Handshake läuft jetzt
    qDebug() << "[IMAP] TCP verbunden mit" << m_server << ":" << m_port;
}

void ImapVerbindung::beiVerschluesselt()
{
    // TLS-Handshake abgeschlossen — die Gegenseite sendet jetzt die Begrüßung.
    qDebug() << "[IMAP] TLS-verschlüsselt — warte auf Begrüßung";
    emit verbunden();
}

void ImapVerbindung::beiTrennung()
{
    m_angemeldet = false;
    m_aktuellerBefehl = Befehl::Keiner;
    m_puffer.clear();
    emit getrennt();
}

void ImapVerbindung::beiSocketFehler(QAbstractSocket::SocketError fehler)
{
    Q_UNUSED(fehler)
    QString meldung = m_verbindung ? m_verbindung->errorString()
                                   : "Unbekannter Socket-Fehler";
    emit fehlerAufgetreten(meldung);
}

void ImapVerbindung::beiSslFehlern(const QList<QSslError> &fehler)
{
    // Akzeptiere alle Zertifikatsfehler für Entwicklungszwecke.
    // TODO: In Produktion durch korrekte Prüfung ersetzen.
    m_verbindung->ignoreSslErrors();

    for (const auto &f : fehler) {
        qWarning() << "[IMAP] SSL-Fehler ignoriert:" << f.errorString();
    }
}

}} // namespace
