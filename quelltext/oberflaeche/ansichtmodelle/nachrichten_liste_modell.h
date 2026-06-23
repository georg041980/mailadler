#pragma once
#include <QtCore/QAbstractListModel>
#include <QtCore/QVector>

#include "../../bibliothek/kern/nachricht.h"

namespace MailAdler
{

class NachrichtenListeModell : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Rollen
    {
        BetreffRolle = Qt::UserRole + 1,
        AbsenderRolle,
        DatumRolle,
        GelesenRolle
    };

    explicit NachrichtenListeModell(QObject* eltern = nullptr);

    int rowCount(const QModelIndex& eltern = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int rolle = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setzeNachrichten(const QVector<Kern::Nachricht>& nachrichten);
    Q_INVOKABLE Kern::Nachricht nachrichtBei(int index) const;

private:
    QVector<Kern::Nachricht> m_nachrichten;
};

} // namespace MailAdler
