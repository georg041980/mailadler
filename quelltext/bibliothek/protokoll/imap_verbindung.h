#pragma once
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QByteArray>
#include <QtNetwork/QSslSocket>

namespace AdlerMail { namespace Protokoll {

/**
 * Stellt eine IMAP-Verbindung zu einem Mail-Server her.
 *
 * Alle Vorgänge laufen asynchron. Ergebnisse werden über Signale gemeldet.
 * Verwendet QSslSocket für TLS-verschlüsselte Verbindungen.
 *
 * @thread Nicht threadsicher — nur im GUI-Strang verwenden.
 */
class ImapVerbindung : public QObject {
    Q_OBJECT
public:
    explicit ImapVerbindung(QObject *eltern = nullptr);
    ~ImapVerbindung() override;

    void setzeServer(const QString &server);
    void setzePort(quint16 port);
    void setzeTls(bool aktiv);   ///< false = Plain-TCP für Tests
    QString server() const;
    quint16 port() const;
    bool istTls() const;
    bool istVerbunden() const;
    bool istAngemeldet() const;

    /// Startet TCP+TLS-Verbindung zum Server.
    void verbinden();

    /// Trennt die Verbindung (sendet LOGOUT, dann disconnect).
    void trennen();

public slots:
    /// Sendet LOGIN-Kommando. Ergebnis kommt über angemeldet() oder fehlerAufgetreten().
    void anmelden(const QString &benutzer, const QString &passwort);

    /// Sendet LIST "" "*" Kommando. Ergebnis kommt über ordnerListeEmpfangen().
    void ordnerListeAbrufen();

signals:
    void verbunden();
    void getrennt();
    void angemeldet();
    void ordnerListeEmpfangen(const QStringList &ordner);
    void fehlerAufgetreten(const QString &meldung);

private slots:
    void beiVerbunden();
    void beiVerschluesselt();
    void beiBereitZumLesen();
    void beiTrennung();
    void beiSocketFehler(QAbstractSocket::SocketError fehler);
    void beiSslFehlern(const QList<QSslError> &fehler);

private:
    /// Sendet ein IMAP-Kommando und gibt den Tag zurück.
    QByteArray sendeBefehl(const QByteArray &befehl);

    /// Verarbeitet eine vollständige IMAP-Antwortzeile.
    void verarbeiteAntwortZeile(const QByteArray &zeile);

    /// Prüft, ob die Zeile eine Statusantwort ist (TAG OK/NO/BAD).
    bool istStatusZeile(const QByteArray &zeile) const;
    void verarbeiteStatusZeile(const QByteArray &zeile);

    /// Prüft, ob die Zeile eine LIST-Antwort enthält.
    bool istListeZeile(const QByteArray &zeile) const;
    QString parseListeOrdnerName(const QByteArray &zeile) const;

    QSslSocket *m_verbindung = nullptr;
    QString     m_server;
    quint16     m_port        = 993;
    bool        m_tls         = true;
    bool        m_angemeldet  = false;
    bool        m_willTrennen = false;

    // IMAP-Protokollzustand
    int         m_tagZaehler   = 0;
    QByteArray  m_letzterTag;
    QByteArray  m_puffer;                // für unvollständige Zeilen
    QStringList m_ordnerListe;           // während LIST gesammelt

    enum class Befehl {
        Keiner,
        Verbinden,       // Warten auf Begrüßung
        Anmelden,
        OrdnerListe,
        Logout
    };
    Befehl m_aktuellerBefehl = Befehl::Keiner;
};

}} // namespace
