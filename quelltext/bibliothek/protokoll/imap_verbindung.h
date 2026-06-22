#pragma once
#include <QtCore/QByteArray>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtNetwork/QSslSocket>

#include "../kern/nachricht.h"

namespace AdlerMail
{
namespace Protokoll
{

class ImapVerbindung : public QObject
{
    Q_OBJECT
public:
    explicit ImapVerbindung(QObject* eltern = nullptr);
    ~ImapVerbindung() override;

    void setzeServer(const QString& server);
    void setzePort(quint16 port);
    void setzeTls(bool aktiv);
    QString server() const;
    quint16 port() const;
    bool istTls() const;
    bool istVerbunden() const;
    bool istAngemeldet() const;

    void verbinden();
    void trennen();

public slots:
    void anmelden(const QString& benutzer, const QString& passwort);
    void ordnerListeAbrufen();

    /// SELECT \"ordner\" — wählt einen Ordner aus.
    void ordnerAuswaehlen(const QString& ordnerName);

    /// FETCH von:bis (FLAGS BODY.PEEK[HEADER.FIELDS (FROM SUBJECT DATE)])
    void nachrichtenHeaderAbrufen(int von, int bis);

    /// FETCH uid BODY[TEXT] — lädt den Text-Inhalt einer Nachricht.
    void nachrichtInhaltAbrufen(int uid);

    /// STORE uid +FLAGS (\Deleted) — markiert Nachricht als gelöscht.
    void nachrichtLoeschen(int uid);

    /// EXPUNGE — löscht alle als \Deleted markierten Nachrichten endgültig.
    void ordnerBereinigen();

signals:
    void verbunden();
    void getrennt();
    void angemeldet();
    void ordnerListeEmpfangen(const QStringList& ordner);
    void ordnerAusgewaehlt(int nachrichtenZaehler);
    void nachrichtHeaderEmpfangen(const Kern::Nachricht& nachricht);
    void nachrichtenHeaderFertig();
    void nachrichtInhaltEmpfangen(int uid, const QString& inhalt);
    void nachrichtGeloescht();
    void ordnerBereinigt();
    void fehlerAufgetreten(const QString& meldung);

private slots:
    void beiVerbunden();
    void beiVerschluesselt();
    void beiBereitZumLesen();
    void beiTrennung();
    void beiSocketFehler(QAbstractSocket::SocketError fehler);
    void beiSslFehlern(const QList<QSslError>& fehler);

private:
    QByteArray sendeBefehl(const QByteArray& befehl);
    void verarbeiteAntwortZeile(const QByteArray& zeile);
    bool istStatusZeile(const QByteArray& zeile) const;
    void verarbeiteStatusZeile(const QByteArray& zeile);
    bool istListeZeile(const QByteArray& zeile) const;
    QString parseListeOrdnerName(const QByteArray& zeile) const;

    /// Parst eine FETCH-Antwortzeile und befüllt eine Nachricht.
    Kern::Nachricht parseFetchZeile(const QByteArray& zeile) const;

    QSslSocket* m_verbindung = nullptr;
    QString m_server;
    quint16 m_port = 993;
    bool m_tls = true;
    bool m_angemeldet = false;
    bool m_willTrennen = false;

    int m_tagZaehler = 0;
    QByteArray m_letzterTag;
    QByteArray m_puffer;
    QStringList m_ordnerListe;
    int m_letzteExistsZahl = 0;
    QByteArray m_fetchPuffer; // akkumuliert mehrzeilige FETCH-Daten

    enum class Befehl
    {
        Keiner,
        Verbinden,
        Anmelden,
        OrdnerListe,
        OrdnerAuswaehlen,
        NachrichtenHeader,
        NachrichtenInhalt,
        NachrichtLoeschen,
        OrdnerBereinigen,
        Logout
    };
    Befehl m_aktuellerBefehl = Befehl::Keiner;
};

} // namespace Protokoll
} // namespace AdlerMail
