#include "nachrichten_liste_modell.h"

namespace MailAdler
{

NachrichtenListeModell::NachrichtenListeModell(QObject* eltern) : QAbstractListModel(eltern)
{
}

int NachrichtenListeModell::rowCount(const QModelIndex&) const
{
    return static_cast<int>(m_nachrichten.size());
}

QVariant NachrichtenListeModell::data(const QModelIndex& index, int rolle) const
{
    if (!index.isValid() || index.row() >= m_nachrichten.size())
        return {};
    const auto& n = m_nachrichten.at(index.row());
    switch (rolle)
    {
    case BetreffRolle:
        return n.betreff;
    case AbsenderRolle:
        return n.absender;
    case DatumRolle:
        return n.datum;
    case GelesenRolle:
        return n.gelesen;
    default:
        return {};
    }
}

QHash<int, QByteArray> NachrichtenListeModell::roleNames() const
{
    return {{BetreffRolle, "betreff"}, {AbsenderRolle, "absender"}, {DatumRolle, "datum"}, {GelesenRolle, "gelesen"}};
}

void NachrichtenListeModell::setzeNachrichten(const QVector<Kern::Nachricht>& n)
{
    beginResetModel();
    m_nachrichten = n;
    endResetModel();
}

Kern::Nachricht NachrichtenListeModell::nachrichtBei(int index) const
{
    if (index < 0 || index >= m_nachrichten.size())
        return {};
    return m_nachrichten.at(index);
}

} // namespace MailAdler
