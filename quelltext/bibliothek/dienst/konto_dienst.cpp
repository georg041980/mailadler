#include "konto_dienst.h"

#include "../speicher/datenbank.h"

namespace MailAdler
{
namespace Dienst
{

KontoDienst::KontoDienst(Speicher::Datenbank* db, QObject* eltern) : QObject(eltern), m_db(db)
{
}

qint64 KontoDienst::kontoAnlegen(const QString& email, const QString& name, const QString& imapServer, quint16 imapPort,
                                 const QString& smtpServer, quint16 smtpPort, const QString& benutzer,
                                 const QString& passwort, const QString& signatur)
{
    Kern::Konto k;
    k.email = email;
    k.name = name;
    k.imapServer = imapServer;
    k.imapPort = imapPort;
    k.smtpServer = smtpServer;
    k.smtpPort = smtpPort;
    k.benutzer = benutzer;
    k.passwort = passwort;
    k.signatur = signatur;
    k.istAktiv = true;

    qint64 id = m_db->kontoSpeichern(k);
    if (id > 0)
        emit kontoHinzugefuegt(id);
    else
        emit fehlerAufgetreten("Konto konnte nicht angelegt werden");
    return id;
}

QVector<Kern::Konto> KontoDienst::alleKonten() const
{
    return m_db->alleKonten();
}

bool KontoDienst::kontoLoeschen(qint64 id)
{
    bool ok = m_db->kontoLoeschen(id);
    if (ok)
        emit kontoGeloescht(id);
    return ok;
}

} // namespace Dienst
} // namespace MailAdler
