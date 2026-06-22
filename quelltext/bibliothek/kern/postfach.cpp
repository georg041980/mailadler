#include "postfach.h"

namespace AdlerMail { namespace Kern {

Postfach::Postfach(QObject *eltern) : QObject(eltern) {}
Postfach::~Postfach() = default;

qint64 Postfach::kontoId() const { return m_kontoId; }
void Postfach::setzeKontoId(qint64 id) { m_kontoId = id; }

QString Postfach::name() const { return m_name; }
void Postfach::setzeName(const QString &name) {
    if (m_name != name) { m_name = name; emit nameGeaendert(); }
}

int Postfach::nachrichtenZaehler() const { return m_nachrichtenZaehler; }
QVector<Nachricht> Postfach::nachrichten() const { return m_nachrichten; }

}} // namespace
