#pragma once
#include <QtCore/QObject>
#include <QtCore/QVector>

#include "../kern/konto.h"

namespace AdlerMail
{

namespace Speicher
{
class Datenbank;
}

namespace Dienst
{

/**
 * High-Level-Dienst für Konto-Verwaltung.
 * Orchestriert Datenbank-Zugriffe und sendet UI-Signale.
 */
class KontoDienst : public QObject
{
    Q_OBJECT
public:
    explicit KontoDienst(Speicher::Datenbank* db, QObject* eltern = nullptr);

    qint64 kontoAnlegen(const QString& email, const QString& name, const QString& imapServer, quint16 imapPort,
                        const QString& smtpServer, quint16 smtpPort, const QString& benutzer, const QString& passwort);

    QVector<Kern::Konto> alleKonten() const;
    bool kontoLoeschen(qint64 id);

signals:
    void kontoHinzugefuegt(qint64 id);
    void kontoGeloescht(qint64 id);
    void fehlerAufgetreten(const QString& meldung);

private:
    Speicher::Datenbank* m_db = nullptr;
};

} // namespace Dienst
} // namespace AdlerMail
