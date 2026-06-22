#pragma once
#include "../kern/nachricht.h"
#include <QtCore/QObject>
#include <QtCore/QVector>

namespace AdlerMail { namespace Speicher {

class Zwischenspeicher : public QObject {
    Q_OBJECT
public:
    explicit Zwischenspeicher(QObject *eltern = nullptr);

    void setze(const QVector<Kern::Nachricht> &nachrichten);
    void setzeEinzeln(const Kern::Nachricht &n);
    QVector<Kern::Nachricht> alle() const;
    void leeren();

private:
    QVector<Kern::Nachricht> m_nachrichten;
};

}} // namespace
