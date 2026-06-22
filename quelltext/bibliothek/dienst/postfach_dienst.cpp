#include "postfach_dienst.h"
#include "../speicher/zwischenspeicher.h"
#include "../speicher/datenbank.h"
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
        connect(m_imap, &Protokoll::ImapVerbindung::ordnerAusgewaehlt,
                this, &PostfachDienst::beiOrdnerAusgewaehlt);
        connect(m_imap, &Protokoll::ImapVerbindung::nachrichtHeaderEmpfangen,
                this, &PostfachDienst::beiNachrichtHeaderEmpfangen);
        connect(m_imap, &Protokoll::ImapVerbindung::nachrichtenHeaderFertig,
                this, &PostfachDienst::beiNachrichtenHeaderFertig);
        connect(m_imap, &Protokoll::ImapVerbindung::fehlerAufgetreten,
                this, &PostfachDienst::beiImapFehler);
    }
}

void PostfachDienst::setzeDatenbank(Speicher::Datenbank *db)
{
    m_db = db;
}

void PostfachDienst::ordnerLaden()
{
    if (!m_imap || !m_imap->istAngemeldet()) {
        emit fehlerAufgetreten("Nicht verbunden");
        return;
    }
    m_imap->ordnerListeAbrufen();
}

void PostfachDienst::nachrichtenLaden(const QString &ordner)
{
    if (!m_imap || !m_imap->istAngemeldet()) {
        emit fehlerAufgetreten("Nicht verbunden");
        return;
    }
    m_aktuellerOrdner = ordner;
    m_imap->ordnerAuswaehlen(ordner);
}

void PostfachDienst::beiOrdnerListeEmpfangen(const QStringList &ordner)
{
    emit ordnerListeGeaendert(ordner);
}

void PostfachDienst::beiOrdnerAusgewaehlt(int zaehler)
{
    if (zaehler > 0) {
        // Lösche alte Nachrichten für diesen Ordner, dann FETCH
        if (m_db) m_db->nachrichtenLoeschenFuerOrdner(m_aktuellerOrdner);
        m_imap->nachrichtenHeaderAbrufen(1, zaehler);
    } else {
        // Leerer Ordner — Cache leeren und Signal senden
        m_cache->leeren();
        emit nachrichtenGeaendert();
    }
}

void PostfachDienst::beiNachrichtHeaderEmpfangen(const Kern::Nachricht &nachricht)
{
    if (m_db) m_db->nachrichtSpeichern(nachricht);
    m_cache->setzeEinzeln(nachricht);
}

void PostfachDienst::beiNachrichtenHeaderFertig()
{
    emit nachrichtenGeaendert();
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
