#include "datenbank.h"
#include "kern/konto.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

namespace AdlerMail { namespace Speicher {

Datenbank::Datenbank(QObject *eltern) : QObject(eltern) {
    m_verbindungsName = QString("adlermail_%1").arg(
        reinterpret_cast<quintptr>(this), 0, 16);
}
Datenbank::~Datenbank() { schliesse(); }

bool Datenbank::oeffne(const QString &pfad) {
    m_db = QSqlDatabase::addDatabase("QSQLITE", m_verbindungsName);
    m_db.setDatabaseName(pfad);
    if (!m_db.open()) {
        m_db = QSqlDatabase();
        emit fehlerAufgetreten(m_db.lastError().text());
        return false;
    }
    erzeugeTabellen();
    return true;
}

void Datenbank::schliesse() {
    if (m_db.isOpen()) m_db.close();
    m_db = QSqlDatabase();
    if (QSqlDatabase::contains(m_verbindungsName)) {
        QSqlDatabase::removeDatabase(m_verbindungsName);
    }
}

bool Datenbank::istOffen() const { return m_db.isOpen(); }

// ---------------------------------------------------------------------------
// Konto-Verwaltung
// ---------------------------------------------------------------------------
qint64 Datenbank::kontoSpeichern(const Kern::Konto &konto) {
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO konten (email, name, imap_server, imap_port, "
                  "smtp_server, smtp_port, benutzer, passwort, aktiv) "
                  "VALUES (:email, :name, :imap, :imapp, :smtp, :smtpp, "
                  ":benutzer, :passwort, :aktiv)");
    query.bindValue(":email",    konto.email);
    query.bindValue(":name",     konto.name);
    query.bindValue(":imap",     konto.imapServer);
    query.bindValue(":imapp",    konto.imapPort);
    query.bindValue(":smtp",     konto.smtpServer);
    query.bindValue(":smtpp",    konto.smtpPort);
    query.bindValue(":benutzer", konto.benutzer);
    query.bindValue(":passwort", konto.passwort);
    query.bindValue(":aktiv",    konto.istAktiv ? 1 : 0);

    if (!query.exec()) {
        if (query.lastError().nativeErrorCode() == "19") {  // SQLITE_CONSTRAINT
            return -1;
        }
        emit fehlerAufgetreten(query.lastError().text());
        return -1;
    }
    return query.lastInsertId().toLongLong();
}

QVector<Kern::Konto> Datenbank::alleKonten() const {
    QVector<Kern::Konto> konten;
    QSqlQuery query(m_db);
    query.exec("SELECT id, email, name, imap_server, imap_port, "
               "smtp_server, smtp_port, benutzer, passwort, aktiv "
               "FROM konten ORDER BY id");

    while (query.next()) {
        Kern::Konto k;
        k.id          = query.value(0).toLongLong();
        k.email       = query.value(1).toString();
        k.name        = query.value(2).toString();
        k.imapServer  = query.value(3).toString();
        k.imapPort    = query.value(4).toUInt();
        k.smtpServer  = query.value(5).toString();
        k.smtpPort    = query.value(6).toUInt();
        k.benutzer    = query.value(7).toString();
        k.passwort    = query.value(8).toString();
        k.istAktiv    = query.value(9).toBool();
        konten.append(k);
    }
    return konten;
}

bool Datenbank::kontoLoeschen(qint64 id) {
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM konten WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec() && query.numRowsAffected() > 0;
}

// ---------------------------------------------------------------------------
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
