#pragma once
#include <QtCore/QObject>
#include <QtCore/QVector>

#include "../kern/nachricht.h"

namespace MailAdler
{
namespace Speicher
{

class Zwischenspeicher : public QObject
{
    Q_OBJECT
public:
    explicit Zwischenspeicher(QObject* eltern = nullptr);

    void setze(const QVector<Kern::Nachricht>& nachrichten);
    void setzeEinzeln(const Kern::Nachricht& n);
    QVector<Kern::Nachricht> alle() const;
    void leeren();

private:
    QVector<Kern::Nachricht> m_nachrichten;
};

} // namespace Speicher
} // namespace MailAdler
