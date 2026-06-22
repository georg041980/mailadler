#pragma once
#include <QtCore/QStringListModel>

namespace AdlerMail
{

class OrdnerListeModell : public QStringListModel
{
    Q_OBJECT
public:
    explicit OrdnerListeModell(QObject* eltern = nullptr);
    void setzeOrdner(const QStringList& ordner);
};

} // namespace AdlerMail
