#pragma once
#include <QtCore/QObject>
#include <QtCore/QVector>
#include "../kern/nachricht.h"

namespace AdlerMail {

namespace Speicher { class Zwischenspeicher; }

namespace Dienst {

/**
 * High-Level-Dienst für Postfach-Operationen.
 * Hält den Zwischenspeicher aktuell und sendet UI-Signale.
 */
class PostfachDienst : public QObject {
    Q_OBJECT
public:
    explicit PostfachDienst(Speicher::Zwischenspeicher *cache,
                            QObject *eltern = nullptr);

    int anzahl() const;
    QVector<Kern::Nachricht> nachrichten() const;
    void setzeNachrichten(const QVector<Kern::Nachricht> &nachrichten);

signals:
    void nachrichtenGeaendert();

private:
    Speicher::Zwischenspeicher *m_cache = nullptr;
};

}} // namespace
