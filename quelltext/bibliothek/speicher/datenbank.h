#pragma once
#include "../kern/qt_alias.h"
#include <QtCore/QObject>
#include <QtSql/QSqlDatabase>

namespace AdlerMail { namespace Speicher {

class Datenbank : public QObject {
    Q_OBJECT
public:
    explicit Datenbank(QObject *eltern = nullptr);
    ~Datenbank() override;

    bool oeffne(const QString &pfad);
    void schliesse();
    bool istOffen() const;

signale:
    void fehlerAufgetreten(const QString &meldung);

private:
    void erzeugeTabellen();
    QSqlDatabase m_db;
};

}} // namespace
