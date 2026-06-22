#pragma once
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QByteArray>
#include <QtNetwork/QSslSocket>

namespace AdlerMail { namespace Protokoll {

/**
 * Stellt eine SMTP-Verbindung zum Senden von E-Mails her.
 *
 * Alle Vorgänge laufen asynchron. Ergebnisse werden über Signale gemeldet.
 * Verwendet QSslSocket für TLS-verschlüsselte Verbindungen.
 *
 * @thread Nicht threadsicher — nur im GUI-Strang verwenden.
 */
class SmtpVerbindung : public QObject {
    Q_OBJECT
public:
    explicit SmtpVerbindung(QObject *eltern = nullptr);
    ~SmtpVerbindung() override;

    void setzeServer(const QString &server);
    void setzePort(quint16 port);
    void setzeTls(bool aktiv);
    QString server() const;
    quint16 port() const;
    bool istVerbunden() const;

    void verbinden();

public slots:
    void sende(const QString &absender, const QStringList &empfaenger,
               const QString &betreff, const QString &inhalt);

signals:
    void verbunden();
    void gesendet();
    void getrennt();
    void fehlerAufgetreten(const QString &meldung);

private slots:
    void beiBereitZumLesen();
    void beiTrennung();
    void beiSocketFehler(QAbstractSocket::SocketError fehler);
    void beiSslFehlern(const QList<QSslError> &fehler);

private:
    QByteArray sendeBefehl(const QByteArray &befehl);
    void verarbeiteAntwortZeile(const QByteArray &zeile);
    bool istPositiveAntwort(const QByteArray &zeile) const;

    QSslSocket *m_verbindung = nullptr;
    QString     m_server;
    quint16     m_port        = 587;
    bool        m_tls         = true;
    QByteArray  m_puffer;

    enum class Phase {
        Keiner,
        Verbinden,
        Ehlo,
        AuthAnmelden,
        AuthPasswort,
        MailFrom,
        RcptTo,
        Daten,
        InhaltSenden,
        Quit
    };
    Phase m_phase = Phase::Keiner;
};

}} // namespace
