#include "nachricht_ansicht_modell.h"

namespace MailAdler
{

NachrichtAnsichtModell::NachrichtAnsichtModell(QObject* eltern) : QObject(eltern)
{
}

QString NachrichtAnsichtModell::absender() const
{
    return m_nachricht.absender;
}

QString NachrichtAnsichtModell::betreff() const
{
    return m_nachricht.betreff;
}

QString NachrichtAnsichtModell::inhalt() const
{
    return m_nachricht.inhalt;
}

QString NachrichtAnsichtModell::datum() const
{
    return m_nachricht.datum.toString("dd.MM.yyyy hh:mm");
}

bool NachrichtAnsichtModell::hatNachricht() const
{
    return m_hatNachricht;
}

bool NachrichtAnsichtModell::istHtml() const
{
    return !m_nachricht.inhaltHtml.isEmpty();
}

int NachrichtAnsichtModell::anzahlAnhaenge() const
{
    return static_cast<int>(m_anhaengeNamen.size());
}

QStringList NachrichtAnsichtModell::anhaengeNamen() const
{
    return m_anhaengeNamen;
}

void NachrichtAnsichtModell::setzeNachricht(const Kern::Nachricht& nachricht)
{
    m_nachricht = nachricht;
    m_hatNachricht = true;
    emit nachrichtGeaendert();
}

void NachrichtAnsichtModell::leeren()
{
    m_nachricht = Kern::Nachricht {};
    m_hatNachricht = false;
    emit nachrichtGeaendert();
}

} // namespace MailAdler
