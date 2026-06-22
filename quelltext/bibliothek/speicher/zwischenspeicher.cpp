#include "zwischenspeicher.h"

namespace AdlerMail { namespace Speicher {

Zwischenspeicher::Zwischenspeicher(QObject *eltern) : QObject(eltern) {}

void Zwischenspeicher::setze(const QVector<Kern::Nachricht> &n) { m_nachrichten = n; }
QVector<Kern::Nachricht> Zwischenspeicher::alle() const { return m_nachrichten; }
void Zwischenspeicher::leeren() { m_nachrichten.clear(); }

}} // namespace
