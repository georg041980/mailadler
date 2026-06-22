#include "postfach_dienst.h"
#include "../speicher/zwischenspeicher.h"
#include "../protokoll/imap_verbindung.h"

namespace AdlerMail { namespace Dienst {

PostfachDienst::PostfachDienst(Speicher::Zwischenspeicher *cache, QObject *eltern)
    : QObject(eltern), m_cache(cache) {}

void PostfachDienst::setzeImapVerbindung(Protokoll::ImapVerbindung *imap)
{
    m_imap = imap;
    if (m_imap) {
        connect(m_imap, &Protokoll::ImapVerbindung::ordnerListeEmpfangen,
                this, &PostfachDienst::beiOrdnerListeEmpfangen);
        connect(m_imap, &Protokoll::ImapVerbindung::fehlerAufgetreten,
                this, &PostfachDienst::beiImapFehler);
    }
}

void PostfachDienst::ordnerLaden()
{
    if (!m_imap) {
        emit fehlerAufgetreten("Keine IMAP-Verbindung gesetzt");
        return;
    }
    if (!m_imap->istAngemeldet()) {
        emit fehlerAufgetreten("Nicht angemeldet — bitte zuerst verbinden und anmelden");
        return;
    }
    m_imap->ordnerListeAbrufen();
}

void PostfachDienst::beiOrdnerListeEmpfangen(const QStringList &ordner)
{
    emit ordnerListeGeaendert(ordner);
}

void PostfachDienst::beiImapFehler(const QString &meldung)
{
    emit fehlerAufgetreten(meldung);
}

int PostfachDienst::anzahl() const
{
    return static_cast<int>(m_cache->alle().size());
}

QVector<Kern::Nachricht> PostfachDienst::nachrichten() const
{
    return m_cache->alle();
}

void PostfachDienst::setzeNachrichten(const QVector<Kern::Nachricht> &n)
{
    m_cache->setze(n);
    emit nachrichtenGeaendert();
}

}} // namespace
