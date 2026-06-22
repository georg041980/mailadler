#pragma once
#include "../kern/konto.h"
#include "../kern/nachricht.h"
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtSql/QSqlDatabase>

namespace AdlerMail { namespace Speicher {

/**
 * SQLite-Datenbank für lokalen Mail-Cache und Konten.
 */
class Datenbank : public QObject {
    Q_OBJECT
public:
    explicit Datenbank(QObject *eltern = nullptr);
    ~Datenbank() override;

    bool oeffne(const QString &pfad);
    void schliesse();
    bool istOffen() const;

    // Konto-Verwaltung
    qint64 kontoSpeichern(const Kern::Konto &konto);
    QVector<Kern::Konto> alleKonten() const;
    bool kontoLoeschen(qint64 id);

    // Nachrichten-Verwaltung
    qint64 nachrichtSpeichern(const Kern::Nachricht &nachricht);
    QVector<Kern::Nachricht> nachrichtenFuerOrdner(const QString &ordner) const;
    bool nachrichtAlsGelesenMarkieren(qint64 id);
    void nachrichtenLoeschenFuerOrdner(const QString &ordner);

    // Suche
    QVector<Kern::Nachricht> sucheNachrichten(const QString &text) const;

signals:
    void fehlerAufgetreten(const QString &meldung);

private:
    void erzeugeTabellen();
    QSqlDatabase m_db;
    QString m_verbindungsName;
};

}} // namespace
