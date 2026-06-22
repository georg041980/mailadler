#include "datenbank.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

namespace AdlerMail { namespace Speicher {

Datenbank::Datenbank(QObject *eltern) : QObject(eltern) {}
Datenbank::~Datenbank() { schliesse(); }

bool Datenbank::oeffne(const QString &pfad) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(pfad);
    if (!m_db.open()) {
        emit fehlerAufgetreten(m_db.lastError().text());
        return false;
    }
    erzeugeTabellen();
    return true;
}

void Datenbank::schliesse() {
    if (m_db.isOpen()) m_db.close();
}

bool Datenbank::istOffen() const { return m_db.isOpen(); }

void Datenbank::erzeugeTabellen() {
    QSqlQuery query(m_db);
    query.exec("CREATE TABLE IF NOT EXISTS konten ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "email TEXT NOT NULL UNIQUE,"
               "name TEXT,"
               "imap_server TEXT, imap_port INTEGER,"
               "smtp_server TEXT, smtp_port INTEGER,"
               "benutzer TEXT, passwort TEXT,"
               "aktiv INTEGER DEFAULT 1)");
}

}} // namespace
