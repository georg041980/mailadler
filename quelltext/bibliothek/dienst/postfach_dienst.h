#pragma once
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QStringList>
#include "../kern/nachricht.h"

namespace AdlerMail {

namespace Speicher { class Zwischenspeicher; }
namespace Protokoll { class ImapVerbindung; }

namespace Dienst {

/**
 * High-Level-Dienst für Postfach-Operationen.
 *
 * Hält den Zwischenspeicher aktuell, orchestriert IMAP-Zugriffe
 * und sendet UI-Signale.
 */
class PostfachDienst : public QObject {
    Q_OBJECT
public:
    explicit PostfachDienst(Speicher::Zwischenspeicher *cache,
                            QObject *eltern = nullptr);

    void setzeImapVerbindung(Protokoll::ImapVerbindung *imap);

    int anzahl() const;
    QVector<Kern::Nachricht> nachrichten() const;
    void setzeNachrichten(const QVector<Kern::Nachricht> &nachrichten);

public slots:
    void ordnerLaden();

signals:
    void nachrichtenGeaendert();
    void ordnerListeGeaendert(const QStringList &ordner);
    void fehlerAufgetreten(const QString &meldung);

private slots:
    void beiOrdnerListeEmpfangen(const QStringList &ordner);
    void beiImapFehler(const QString &meldung);

private:
    Speicher::Zwischenspeicher *m_cache = nullptr;
    Protokoll::ImapVerbindung  *m_imap  = nullptr;
};

}} // namespace
