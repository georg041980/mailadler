#pragma once
#include <QtCore/QStringListModel>

#include "../bibliothek/kern/konto.h"

namespace MailAdler
{

class KontoAuswahlModell : public QStringListModel
{
    Q_OBJECT
public:
    explicit KontoAuswahlModell(QObject* eltern = nullptr);
    void setzeKonten(const QVector<Kern::Konto>& konten);
    Kern::Konto kontoBei(int index) const;

signals:
    void kontoAusgewaehlt(const Kern::Konto& konto);

public slots:
    void beiIndexGeaendert(int index);

private:
    QVector<Kern::Konto> m_konten;
};

} // namespace MailAdler
