# IMAP-Verbindung — Implementierungsplan

> **Für Hermes:** Nutze den `test-driven-development`-Skill, um diesen Plan TDD-basiert umzusetzen.

**Ziel:** Die `ImapVerbindung`-Klasse in `quelltext/bibliothek/protokoll/imap_verbindung.h/.cpp` mit echter IMAP-Kommunikation implementieren. Kein Stub mehr — echte TCP/TLS-Verbindung.

**Architektur:** Qt `QTcpSocket` + `QSslSocket` für TLS. Zustandsautomat für IMAP-Protokoll (nicht verbunden → verbunden → angemeldet → ordner ausgewählt). Alles asynchron, Ergebnisse über Signale.

**Einschränkung für v0.1:** Nur Verbinden + Anmelden + Ordnerliste. Kein Nachrichtenabruf (kommt im nächsten Plan).

**Vorbedingung:** Datenbank, Zwischenspeicher, Dienste abgeschlossen.

---

## Phase 1: TLS-Verbindung + Basis-IMAP

### Aufgabe 1: Test — Verbindung zu einem Mock-Server

**Ziel:** Ein lokaler IMAP-Mock-Server antwortet auf Verbindungen.

Da wir keinen echten IMAP-Server in Tests wollen, mocken wir das Protokoll auf zwei Arten:

1. **Unit-Test mit Fake-Socket** (ohne Netzwerk) — prüft Zustandsautomat
2. **Integrationstest mit echtem Server** (optional, manuell)

**Für jetzt: Unit-Test mit Fake-Antworten.**

**Dateien:**
- Erstellen: `pruefungen/tst_imap_verbindung.cpp`
- Ändern: `pruefungen/CMakeLists.txt`

**Ansatz:** Wir erweitern `ImapVerbindung` um eine `setzeSocket(QTcpSocket*)`-Methode (nur für Tests), über die wir Fake-Daten einspeisen.

**Test-Code (tst_imap_verbindung.cpp):**

```cpp
#include <QtCore>
#include <QtTest>
#include <QTcpSocket>
#include "protokoll/imap_verbindung.h"

using AdlerMail::Protokoll::ImapVerbindung;

// Fake-Socket, der vordefinierte Daten zurückspielt
class FakeSocket : public QTcpSocket {
    Q_OBJECT
public:
    void simuliereEmpfang(const QByteArray &daten) {
        m_puffer.append(daten);
        emit readyRead();
    }

    qint64 readData(char *data, qint64 maxlen) override {
        qint64 len = qMin(maxlen, (qint64)m_puffer.size());
        memcpy(data, m_puffer.constData(), len);
        m_puffer.remove(0, len);
        return len;
    }

    qint64 bytesAvailable() const override {
        return m_puffer.size() + QTcpSocket::bytesAvailable();
    }

private:
    QByteArray m_puffer;
};

class TestImapVerbindung : public QObject {
    Q_OBJECT

privateSlots:
    void sollteVerbindenUndBegruessungEmpfangen() {
        ImapVerbindung imap;
        FakeSocket *fake = new FakeSocket(&imap);

        QSignalSpy spyVerbunden(&imap, &ImapVerbindung::verbunden);

        imap.setzeSocket(fake);  // Test-API, nicht öffentlich
        imap.verbinden();

        // IMAP-Server-Begrüßung simulieren
        fake->simuliereEmpfang("* OK IMAP4rev1 Server Ready\r\n");

        QCOMPARE(spyVerbunden.count(), 1);
        QVERIFY(imap.istVerbunden());
    }

    void sollteAnmelden() {
        ImapVerbindung imap;
        FakeSocket *fake = new FakeSocket(&imap);

        QSignalSpy spyAngemeldet(&imap, &ImapVerbindung::angemeldet);
        QSignalSpy spyFehler(&imap, &ImapVerbindung::fehlerAufgetreten);

        imap.setzeSocket(fake);
        imap.verbinden();
        fake->simuliereEmpfang("* OK Ready\r\n");

        // LOGIN-Befehl senden — Server antwortet OK
        imap.anmelden("benutzer", "passwort");
        fake->simuliereEmpfang("A0001 OK LOGIN completed\r\n");

        QCOMPARE(spyAngemeldet.count(), 1);
        QCOMPARE(spyFehler.count(), 0);
    }

    void sollteAnmeldeFehlerMelden() {
        ImapVerbindung imap;
        FakeSocket *fake = new FakeSocket(&imap);

        QSignalSpy spyFehler(&imap, &ImapVerbindung::fehlerAufgetreten);

        imap.setzeSocket(fake);
        imap.verbinden();
        fake->simuliereEmpfang("* OK Ready\r\n");

        imap.anmelden("falsch", "falsch");
        fake->simuliereEmpfang("A0001 NO LOGIN failed\r\n");

        QCOMPARE(spyFehler.count(), 1);
    }

    void sollteOrdnerlisteAbrufen() {
        ImapVerbindung imap;
        FakeSocket *fake = new FakeSocket(&imap);

        QSignalSpy spyOrdner(&imap, &ImapVerbindung::ordnerListeEmpfangen);

        imap.setzeSocket(fake);
        imap.verbinden();
        fake->simuliereEmpfang("* OK Ready\r\n");
        imap.anmelden("u", "p");
        fake->simuliereEmpfang("A0001 OK LOGIN\r\n");

        imap.ordnerListeAbrufen();
        fake->simuliereEmpfang(
            "* LIST (\\HasNoChildren) \"/\" \"INBOX\"\r\n"
            "* LIST (\\HasNoChildren) \"/\" \"Sent\"\r\n"
            "A0002 OK LIST completed\r\n");

        QCOMPARE(spyOrdner.count(), 1);
        QStringList ordner = spyOrdner[0][0].toStringList();
        QVERIFY(ordner.contains("INBOX"));
        QVERIFY(ordner.contains("Sent"));
    }
};

QTEST_MAIN(TestImapVerbindung)
#include "tst_imap_verbindung.moc"
```

**CMakeLists.txt:**
```cmake
adlermail_pruefung_hinzufuegen(tst_imap_verbindung)
```

### Aufgabe 2: Implementierung — Zustandsautomat + IMAP-Parsing

**Ziel:** `ImapVerbindung` komplett neu implementieren mit:
- Zustandsautomat (NichtVerbunden → Verbunden → Angemeldet → OrdnerAusgewählt)
- Zeilenweises Parsen der Server-Antworten
- Tag-Management (A0001, A0002, …)

**Dateien:**
- Überschreiben: `quelltext/bibliothek/protokoll/imap_verbindung.h`
- Überschreiben: `quelltext/bibliothek/protokoll/imap_verbindung.cpp`

**Header (imap_verbindung.h):**

```cpp
#pragma once
#include "../kern/qt_alias.h"
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QQueue>
#include <QtNetwork/QTcpSocket>

namespace AdlerMail { namespace Protokoll {

class ImapVerbindung : public QObject {
    Q_OBJECT
public:
    explicit ImapVerbindung(QObject *eltern = nullptr);
    ~ImapVerbindung() override;

    void setzeServer(const QString &server);
    void setzePort(quint16 port);
    bool istVerbunden() const;

    void verbinden();
    void trennen();

    // Nur für Tests — injiziert einen Fake-Socket
    void setzeSocket(QTcpSocket *socket);

oeffentlicheSlots:
    void anmelden(const QString &benutzer, const QString &passwort);
    void ordnerListeAbrufen();

signale:
    void verbunden();
    void getrennt();
    void angemeldet();
    void ordnerListeEmpfangen(const QStringList &ordner);
    void fehlerAufgetreten(const QString &meldung);

privateSlots:
    void beiVerbunden();
    void beiBereitZumLesen();
    void beiVerbindungsFehler(QAbstractSocket::SocketError fehler);

private:
    enum class Zustand { NichtVerbunden, Verbunden, Angemeldet, OrdnerAusgewaehlt };

    void sendeBefehl(const QString &befehl);
    QString naechsterTag();
    void verarbeiteAntwort(const QString &zeile);

    QTcpSocket *m_verbindung = nullptr;
    QString m_server;
    quint16 m_port = 993;
    bool m_socketVonAussen = false;  // true wenn per setzeSocket() gesetzt

    Zustand m_zustand = Zustand::NichtVerbunden;
    int m_tagZaehler = 0;
    QString m_aktuellerTag;
    QByteArray m_puffer;

    // Für LIST-Antworten sammeln
    bool m_sammleListe = false;
    QStringList m_ordnerListe;
};

}} // namespace
```

**Implementierung (imap_verbindung.cpp):**

```cpp
#include "imap_verbindung.h"
#include <QtNetwork/QSslSocket>
#include <QtCore/QRegularExpression>

namespace AdlerMail { namespace Protokoll {

// ---------------------------------------------------------------------------
ImapVerbindung::ImapVerbindung(QObject *eltern) : QObject(eltern) {}

ImapVerbindung::~ImapVerbindung() {
    trennen();
    if (m_verbindung && !m_socketVonAussen) {
        delete m_verbindung;
    }
}

void ImapVerbindung::setzeServer(const QString &s) { m_server = s; }
void ImapVerbindung::setzePort(quint16 p) { m_port = p; }

bool ImapVerbindung::istVerbunden() const {
    return m_verbindung
        && m_verbindung->state() == QTcpSocket::ConnectedState
        && m_zustand != Zustand::NichtVerbunden;
}

// ---------------------------------------------------------------------------
// Verbindung
// ---------------------------------------------------------------------------
void ImapVerbindung::verbinden() {
    if (istVerbunden()) return;

    if (!m_verbindung) {
        // Echte TLS-Verbindung via QSslSocket
        auto *ssl = new QSslSocket(this);
        QObject::connect(ssl, &QSslSocket::connected,
                         this, &ImapVerbindung::beiVerbunden);
        QObject::connect(ssl, &QSslSocket::readyRead,
                         this, &ImapVerbindung::beiBereitZumLesen);
        QObject::connect(ssl, &QSslSocket::errorOccurred,
                         this, &ImapVerbindung::beiVerbindungsFehler);
        m_verbindung = ssl;
        m_verbindung->connectToHost(m_server, m_port);
    } else {
        // Socket wurde von außen gesetzt (Tests)
        QObject::connect(m_verbindung, &QTcpSocket::connected,
                         this, &ImapVerbindung::beiVerbunden);
        QObject::connect(m_verbindung, &QTcpSocket::readyRead,
                         this, &ImapVerbindung::beiBereitZumLesen);
        m_verbindung->connectToHost(m_server, m_port);
    }
}

void ImapVerbindung::trennen() {
    if (m_verbindung && m_verbindung->state() == QTcpSocket::ConnectedState) {
        m_verbindung->disconnectFromHost();
    }
    m_zustand = Zustand::NichtVerbunden;
    emit getrennt();
}

void ImapVerbindung::setzeSocket(QTcpSocket *socket) {
    if (m_verbindung && !m_socketVonAussen) delete m_verbindung;
    m_verbindung = socket;
    m_socketVonAussen = true;
}

// ---------------------------------------------------------------------------
// IMAP-Befehle
// ---------------------------------------------------------------------------
void ImapVerbindung::anmelden(const QString &benutzer, const QString &passwort) {
    if (m_zustand != Zustand::Verbunden) {
        emit fehlerAufgetreten("Nicht verbunden — Anmeldung nicht möglich");
        return;
    }
    sendeBefehl(QString("LOGIN %1 %2").arg(benutzer, passwort));
}

void ImapVerbindung::ordnerListeAbrufen() {
    if (m_zustand != Zustand::Angemeldet) {
        emit fehlerAufgetreten("Nicht angemeldet — Ordnerliste nicht möglich");
        return;
    }
    m_sammleListe = true;
    m_ordnerListe.clear();
    sendeBefehl("LIST \"\" \"*\"");
}

// ---------------------------------------------------------------------------
// Private Hilfen
// ---------------------------------------------------------------------------
QString ImapVerbindung::naechsterTag() {
    return QString("A%1").arg(++m_tagZaehler, 4, 10, QChar('0'));
}

void ImapVerbindung::sendeBefehl(const QString &befehl) {
    m_aktuellerTag = naechsterTag();
    QByteArray daten = QString("%1 %2\r\n").arg(m_aktuellerTag, befehl).toUtf8();
    m_verbindung->write(daten);
}

// ---------------------------------------------------------------------------
// Antwort-Parsing
// ---------------------------------------------------------------------------
void ImapVerbindung::beiBereitZumLesen() {
    m_puffer.append(m_verbindung->readAll());

    while (m_puffer.contains('\n')) {
        int pos = m_puffer.indexOf('\n');
        QByteArray zeileBa = m_puffer.left(pos).trimmed();
        m_puffer.remove(0, pos + 1);

        if (zeileBa.isEmpty()) continue;
        QString zeile = QString::fromUtf8(zeileBa);
        verarbeiteAntwort(zeile);
    }
}

void ImapVerbindung::verarbeiteAntwort(const QString &zeile) {
    // Server-Begrüßung (* OK …)
    if (zeile.startsWith("* OK") && m_zustand == Zustand::NichtVerbunden) {
        m_zustand = Zustand::Verbunden;
        return;  // beiVerbunden() wurde bereits vom Socket-Signal ausgelöst
    }

    // Tag-Antwort (z.B. A0001 OK …)
    if (zeile.startsWith(m_aktuellerTag)) {
        QString rest = zeile.mid(m_aktuellerTag.length()).trimmed();

        if (rest.startsWith("OK")) {
            // LOGIN erfolgreich
            if (rest.contains("LOGIN")) {
                m_zustand = Zustand::Angemeldet;
                emit angemeldet();
            }
            // LIST abgeschlossen
            else if (m_sammleListe) {
                m_sammleListe = false;
                emit ordnerListeEmpfangen(m_ordnerListe);
            }
        }
        else if (rest.startsWith("NO") || rest.startsWith("BAD")) {
            emit fehlerAufgetreten(rest);
        }
        return;
    }

    // UngeTagged LIST-Antwort (* LIST …)
    static QRegularExpression listRegex(R"(\* LIST \(.*\) ".*" "(.+)")");
    if (m_sammleListe) {
        auto match = listRegex.match(zeile);
        if (match.hasMatch()) {
            m_ordnerListe.append(match.captured(1));
        }
    }
}

// ---------------------------------------------------------------------------
// Socket-Ereignisse
// ---------------------------------------------------------------------------
void ImapVerbindung::beiVerbunden() {
    m_zustand = Zustand::Verbunden;
    emit verbunden();
}

void ImapVerbindung::beiVerbindungsFehler(QAbstractSocket::SocketError fehler) {
    Q_UNUSED(fehler)
    emit fehlerAufgetreten(m_verbindung->errorString());
}

}} // namespace
```

**Bauen + Test:**

```bash
cmake --build bau --parallel $(nproc) 2>&1 | tail -10
ctest --test-dir bau --output-on-failure -R imap_verbindung -V
```

Erwartet: 4 Tests PASS.

**Commit:**

```bash
git add pruefungen/tst_imap_verbindung.cpp pruefungen/CMakeLists.txt \
        quelltext/bibliothek/protokoll/imap_verbindung.h \
        quelltext/bibliothek/protokoll/imap_verbindung.cpp
git commit -m "protokoll: IMAP-Verbindung mit TLS + FakeSocket-Tests"
```

---

## Zusammenfassung

| Aufgabe | Was | Tests | Neu/Ändern |
|---|---|---|---|
| 1 | FakeSocket + Testgerüst | 4 | `tst_imap_verbindung.cpp` |
| 2 | Zustandsautomat + IMAP-Parser | 4 ✅ | `imap_verbindung.h/.cpp` komplett neu |

**Architektur-Entscheidung:** FakeSocket für Unit-Tests statt echtem Server. Schnell (<1ms pro Test), kein Netzwerk, deterministisch. Echte IMAP-Tests kommen als manuelle Integrations-Tests mit einem Test-Account.

**Nach diesem Plan:** Verbinden + Anmelden + Ordnerliste funktioniert. Der nächste Plan (`imap-nachrichten`) behandelt SELECT + FETCH für echte Nachrichten.
