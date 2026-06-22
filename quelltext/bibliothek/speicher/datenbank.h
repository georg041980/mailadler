#pragma once
#include "../kern/konto.h"
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

signals:
    void fehlerAufgetreten(const QString &meldung);

private:
    void erzeugeTabellen();
    QSqlDatabase m_db;
    QString m_verbindungsName;
};

}} // namespace
