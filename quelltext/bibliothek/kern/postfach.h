#pragma once
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>

#include "nachricht.h"

namespace MailAdler
{
namespace Kern
{

class Postfach : public QObject
{
    Q_OBJECT
public:
    explicit Postfach(QObject* eltern = nullptr);
    ~Postfach() override;

    qint64 kontoId() const;
    void setzeKontoId(qint64 id);

    QString name() const;
    void setzeName(const QString& name);

    int nachrichtenZaehler() const;
    QVector<Nachricht> nachrichten() const;

signals:
    void nameGeaendert();
    void nachrichtenGeaendert();

private:
    qint64 m_kontoId = 0;
    QString m_name;
    int m_nachrichtenZaehler = 0;
    QVector<Nachricht> m_nachrichten;
};

} // namespace Kern
} // namespace MailAdler
