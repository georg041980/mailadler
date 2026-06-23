#include "datenbank.h"

#include <QtCore/QDateTime>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include "kern/konto.h"
#include "kern/nachricht.h"

namespace
{

QString verschluessele(const QString& klartext)
{
    return QString::fromUtf8(klartext.toUtf8().toBase64());
}

QString entschluessele(const QString& base64)
{
    return QString::fromUtf8(QByteArray::fromBase64(base64.toUtf8()));
}

} // namespace

namespace MailAdler
{
namespace Speicher
{

Datenbank::Datenbank(QObject* eltern) : QObject(eltern)
{
    m_verbindungsName = QString("mailadler_%1").arg(reinterpret_cast<quintptr>(this), 0, 16);
}

Datenbank::~Datenbank()
{
    schliesse();
}

bool Datenbank::oeffne(const QString& pfad)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", m_verbindungsName);
    m_db.setDatabaseName(pfad);
    if (!m_db.open())
    {
        m_db = QSqlDatabase();
        emit fehlerAufgetreten(m_db.lastError().text());
        return false;
    }
    erzeugeTabellen();
    return true;
}

void Datenbank::schliesse()
{
    if (m_db.isOpen())
        m_db.close();
    m_db = QSqlDatabase();
    if (QSqlDatabase::contains(m_verbindungsName))
    {
        QSqlDatabase::removeDatabase(m_verbindungsName);
    }
}

bool Datenbank::istOffen() const
{
    return m_db.isOpen();
}

// ---------------------------------------------------------------------------
// Konto-Verwaltung
// ---------------------------------------------------------------------------
qint64 Datenbank::kontoSpeichern(const Kern::Konto& konto)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO konten (email, name, imap_server, imap_port, "
                  "smtp_server, smtp_port, benutzer, passwort, signatur, aktiv) "
                  "VALUES (:email, :name, :imap, :imapp, :smtp, :smtpp, :benutzer, :passwort, :signatur, :aktiv)");
    query.bindValue(":email", konto.email);
    query.bindValue(":name", konto.name);
    query.bindValue(":imap", konto.imapServer);
    query.bindValue(":imapp", konto.imapPort);
    query.bindValue(":smtp", konto.smtpServer);
    query.bindValue(":smtpp", konto.smtpPort);
    query.bindValue(":benutzer", konto.benutzer);
    query.bindValue(":passwort", verschluessele(konto.passwort));
    query.bindValue(":signatur", konto.signatur);
    query.bindValue(":aktiv", konto.istAktiv ? 1 : 0);

    if (!query.exec())
    {
        if (query.lastError().nativeErrorCode() == "19")
        { // SQLITE_CONSTRAINT
            return -1;
        }
        emit fehlerAufgetreten(query.lastError().text());
        return -1;
    }
    return query.lastInsertId().toLongLong();
}

QVector<Kern::Konto> Datenbank::alleKonten() const
{
    QVector<Kern::Konto> konten;
    QSqlQuery query(m_db);
    query.exec("SELECT id, email, name, imap_server, imap_port, "
               "smtp_server, smtp_port, benutzer, passwort, signatur, aktiv "
               "FROM konten ORDER BY id");

    while (query.next())
    {
        Kern::Konto k;
        k.id = query.value(0).toLongLong();
        k.email = query.value(1).toString();
        k.name = query.value(2).toString();
        k.imapServer = query.value(3).toString();
        k.imapPort = query.value(4).toUInt();
        k.smtpServer = query.value(5).toString();
        k.smtpPort = query.value(6).toUInt();
        k.benutzer = query.value(7).toString();
        k.passwort = entschluessele(query.value(8).toString());
        k.signatur = query.value(9).toString();
        k.istAktiv = query.value(10).toBool();
        konten.append(k);
    }
    return konten;
}

bool Datenbank::kontoLoeschen(qint64 id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM konten WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec() && query.numRowsAffected() > 0;
}

// ---------------------------------------------------------------------------
// Nachrichten-Verwaltung
// ---------------------------------------------------------------------------

qint64 Datenbank::nachrichtSpeichern(const Kern::Nachricht& n)
{
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT OR REPLACE INTO nachrichten "
        "(id, ordner, uid, absender, betreff, inhalt, inhalt_html, datum, gelesen, hat_anhaenge, ist_entwurf) "
        "VALUES (:id, :ordner, :uid, :absender, :betreff, :inhalt, :html, :datum, :gelesen, :anhaenge, :entwurf)");
    query.bindValue(":id", n.id > 0 ? n.id : QVariant());
    query.bindValue(":ordner", "INBOX");
    query.bindValue(":uid", n.id);
    query.bindValue(":absender", n.absender);
    query.bindValue(":betreff", n.betreff);
    query.bindValue(":inhalt", n.inhalt);
    query.bindValue(":html", n.inhaltHtml);
    query.bindValue(":datum", n.datum.toString(Qt::ISODate));
    query.bindValue(":gelesen", n.gelesen ? 1 : 0);
    query.bindValue(":anhaenge", n.hatAnhaenge ? 1 : 0);
    query.bindValue(":entwurf", n.istEntwurf ? 1 : 0);

    if (!query.exec())
    {
        emit fehlerAufgetreten(query.lastError().text());
        return -1;
    }
    return query.lastInsertId().toLongLong();
}

QVector<Kern::Nachricht> Datenbank::nachrichtenFuerOrdner(const QString& ordner) const
{
    QVector<Kern::Nachricht> ergebnis;
    QSqlQuery query(m_db);
    query.prepare("SELECT id, ordner, uid, absender, betreff, inhalt, inhalt_html, "
                  "datum, gelesen, hat_anhaenge FROM nachrichten WHERE ordner = :ordner "
                  "ORDER BY datum DESC");
    query.bindValue(":ordner", ordner);
    query.exec();

    while (query.next())
    {
        Kern::Nachricht n;
        n.id = query.value(0).toLongLong();
        n.absender = query.value(3).toString();
        n.betreff = query.value(4).toString();
        n.inhalt = query.value(5).toString();
        n.inhaltHtml = query.value(6).toString();
        n.datum = QDateTime::fromString(query.value(7).toString(), Qt::ISODate);
        n.gelesen = query.value(8).toBool();
        n.hatAnhaenge = query.value(9).toBool();
        ergebnis.append(n);
    }
    return ergebnis;
}

bool Datenbank::nachrichtAlsGelesenMarkieren(qint64 id)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE nachrichten SET gelesen = 1 WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec() && query.numRowsAffected() > 0;
}

void Datenbank::nachrichtenLoeschenFuerOrdner(const QString& ordner)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM nachrichten WHERE ordner = :ordner");
    query.bindValue(":ordner", ordner);
    query.exec();
}

QVector<Kern::Nachricht> Datenbank::sucheNachrichten(const QString& text) const
{
    QVector<Kern::Nachricht> ergebnis;
    QSqlQuery query(m_db);
    query.prepare("SELECT id, ordner, uid, absender, betreff, inhalt, inhalt_html, "
                  "datum, gelesen, hat_anhaenge FROM nachrichten "
                  "WHERE betreff LIKE :t OR absender LIKE :t2 OR inhalt LIKE :t3 "
                  "ORDER BY datum DESC LIMIT 100");
    QString muster = "%" + text + "%";
    query.bindValue(":t", muster);
    query.bindValue(":t2", muster);
    query.bindValue(":t3", muster);
    query.exec();

    while (query.next())
    {
        Kern::Nachricht n;
        n.id = query.value(0).toLongLong();
        n.absender = query.value(3).toString();
        n.betreff = query.value(4).toString();
        n.inhalt = query.value(5).toString();
        n.datum = QDateTime::fromString(query.value(7).toString(), Qt::ISODate);
        n.gelesen = query.value(8).toBool();
        ergebnis.append(n);
    }
    return ergebnis;
}

// ---------------------------------------------------------------------------
void Datenbank::erzeugeTabellen()
{
    QSqlQuery query(m_db);
    query.exec("CREATE TABLE IF NOT EXISTS konten ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "email TEXT NOT NULL UNIQUE,"
               "name TEXT,"
               "imap_server TEXT, imap_port INTEGER,"
               "smtp_server TEXT, smtp_port INTEGER,"
               "benutzer TEXT, passwort TEXT, signatur TEXT,"
               "aktiv INTEGER DEFAULT 1)");

    query.exec("CREATE TABLE IF NOT EXISTS nachrichten ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "ordner TEXT NOT NULL DEFAULT 'INBOX',"
               "uid INTEGER,"
               "absender TEXT, betreff TEXT,"
               "inhalt TEXT, inhalt_html TEXT,"
               "datum TEXT,"
               "gelesen INTEGER DEFAULT 0,"
               "hat_anhaenge INTEGER DEFAULT 0,"
               "ist_entwurf INTEGER DEFAULT 0)");
}

} // namespace Speicher
} // namespace MailAdler
