#pragma once
#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include "../kern/nachricht.h"

namespace AdlerMail
{

namespace Speicher
{
class Zwischenspeicher;
class Datenbank;
} // namespace Speicher

namespace Protokoll
{
class ImapVerbindung;
}

namespace Dienst
{

class PostfachDienst : public QObject
{
    Q_OBJECT
public:
    explicit PostfachDienst(Speicher::Zwischenspeicher* cache, QObject* eltern = nullptr);

    void setzeImapVerbindung(Protokoll::ImapVerbindung* imap);
    void setzeDatenbank(Speicher::Datenbank* db);

    /** IMAP verbinden und anmelden — Ein-Schritt-Komfortmethode. */
    void verbinden(const QString& server, quint16 port, const QString& benutzer, const QString& passwort);

    int anzahl() const;
    QVector<Kern::Nachricht> nachrichten() const;
    void setzeNachrichten(const QVector<Kern::Nachricht>& nachrichten);

public slots:
    void ordnerLaden();
    void nachrichtenLaden(const QString& ordner);

signals:
    void nachrichtenGeaendert();
    void ordnerListeGeaendert(const QStringList& ordner);
    void verbunden();
    void neueNachricht();
    void fehlerAufgetreten(const QString& meldung);

private slots:
    void beiImapVerbunden();
    void beiImapAngemeldet();
    void beiOrdnerListeEmpfangen(const QStringList& ordner);
    void beiOrdnerAusgewaehlt(int zaehler);
    void beiNachrichtHeaderEmpfangen(const Kern::Nachricht& nachricht);
    void beiNachrichtenHeaderFertig();
    void beiNeueNachricht();
    void beiImapFehler(const QString& meldung);

private:
    Speicher::Zwischenspeicher* m_cache = nullptr;
    Protokoll::ImapVerbindung* m_imap = nullptr;
    Speicher::Datenbank* m_db = nullptr;
    QString m_aktuellerOrdner;
    QString m_credsBenutzer;
    QString m_credsPasswort;
    bool m_autoLadeOrdner = false;
};

} // namespace Dienst
} // namespace AdlerMail
