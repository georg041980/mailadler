#include "konto_auswahl_modell.h"

namespace MailAdler
{

KontoAuswahlModell::KontoAuswahlModell(QObject* eltern) : QStringListModel(eltern)
{
}

void KontoAuswahlModell::setzeKonten(const QVector<Kern::Konto>& konten)
{
    m_konten = konten;
    QStringList namen;
    for (const auto& k : konten)
        namen.append(k.email);
    setStringList(namen);
}

Kern::Konto KontoAuswahlModell::kontoBei(int index) const
{
    if (index >= 0 && index < m_konten.size())
        return m_konten[index];
    return {};
}

void KontoAuswahlModell::beiIndexGeaendert(int index)
{
    if (index >= 0 && index < m_konten.size())
        emit kontoAusgewaehlt(m_konten[index]);
}

} // namespace MailAdler
