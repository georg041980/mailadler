#include "postfach_dienst.h"
#include "../speicher/zwischenspeicher.h"

namespace AdlerMail { namespace Dienst {

PostfachDienst::PostfachDienst(Speicher::Zwischenspeicher *cache, QObject *eltern)
    : QObject(eltern), m_cache(cache) {}

int PostfachDienst::anzahl() const {
    return m_cache->alle().size();
}

QVector<Kern::Nachricht> PostfachDienst::nachrichten() const {
    return m_cache->alle();
}

void PostfachDienst::setzeNachrichten(const QVector<Kern::Nachricht> &n) {
    m_cache->setze(n);
    emit nachrichtenGeaendert();
}

}} // namespace
