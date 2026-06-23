# SMTP-Verbindung — Implementierungsplan

> **Für Hermes:** Nutze den `test-driven-development`-Skill, um diesen Plan TDD-basiert umzusetzen.

**Ziel:** Die `SmtpVerbindung`-Klasse mit echter SMTP-Kommunikation implementieren (TLS, AUTH LOGIN, MAIL FROM, RCPT TO, DATA).

**Architektur:** Gleicher FakeSocket-Ansatz wie bei IMAP. Zustandsautomat: NichtVerbunden → Verbunden → Begruesst → Angemeldet. `sende()`-Methode als einzelner Aufruf, der den ganzen SMTP-Dialog durchläuft.

**Vorbedingung:** IMAP-Plan abgeschlossen (FakeSocket-Muster existiert).

---

## Phase 1: SMTP senden mit FakeSocket

### Aufgabe 1: Test — SMTP-Dialog

**Dateien:**
- Erstellen: `pruefungen/tst_smtp_verbindung.cpp`
- Ändern: `pruefungen/CMakeLists.txt`

**Test-Code (tst_smtp_verbindung.cpp):**

```cpp
#include <QtCore>
#include <QtTest>
#include "protokoll/smtp_verbindung.h"

using MailAdler::Protokoll::SmtpVerbindung;

// FakeSocket wie bei IMAP — kopiert aus tst_imap_verbindung.cpp
// (In Produktion: gemeinsame Test-Helper-Datei erstellen)
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

class TestSmtpVerbindung : public QObject {
    Q_OBJECT

privateSlots:
    void sollteMailSenden() {
        SmtpVerbindung smtp;
        FakeSocket *fake = new FakeSocket(&smtp);

        QSignalSpy spyGesendet(&smtp, &SmtpVerbindung::gesendet);

        smtp.setzeSocket(fake);
        smtp.verbinden();
        fake->simuliereEmpfang("220 mail.test.de ESMTP\r\n");
        fake->simuliereEmpfang("250 OK\r\n");       // EHLO
        fake->simuliereEmpfang("250 AUTH LOGIN\r\n");
        fake->simuliereEmpfang("334 VXNlcm5hbWU6\r\n");  // AUTH LOGIN
        fake->simuliereEmpfang("334 UGFzc3dvcmQ6\r\n");
        fake->simuliereEmpfang("235 2.7.0 OK\r\n");       // AUTH OK
        fake->simuliereEmpfang("250 OK\r\n");       // MAIL FROM
        fake->simuliereEmpfang("250 OK\r\n");       // RCPT TO
        fake->simuliereEmpfang("354 End data\r\n"); // DATA
        fake->simuliereEmpfang("250 OK\r\n");       // Nachricht akzeptiert

        smtp.sende("max@test.de", {"lisa@test.de"},
                   "Betreff", "Hallo Welt!");

        QCOMPARE(spyGesendet.count(), 1);
    }

    void sollteFehlerMelden() {
        SmtpVerbindung smtp;
        FakeSocket *fake = new FakeSocket(&smtp);

        QSignalSpy spyFehler(&smtp, &SmtpVerbindung::fehlerAufgetreten);

        smtp.setzeSocket(fake);
        smtp.verbinden();
        fake->simuliereEmpfang("220 OK\r\n");
        fake->simuliereEmpfang("250 OK\r\n");
        fake->simuliereEmpfang("550 Mailbox nicht gefunden\r\n");  // RCPT TO fehlgeschlagen!

        smtp.sende("max@test.de", {"falsch@test.de"}, "X", "Y");

        QCOMPARE(spyFehler.count(), 1);
    }
};

QTEST_MAIN(TestSmtpVerbindung)
#include "tst_smtp_verbindung.moc"
```

**CMakeLists.txt:**
```cmake
mailadler_pruefung_hinzufuegen(tst_smtp_verbindung)
```

### Aufgabe 2: Implementierung — SMTP-Protokoll

**Dateien:**
- Überschreiben: `quelltext/bibliothek/protokoll/smtp_verbindung.h`
- Überschreiben: `quelltext/bibliothek/protokoll/smtp_verbindung.cpp`

**Header (smtp_verbindung.h):**

```cpp
#pragma once
#include "../kern/qt_alias.h"
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtNetwork/QTcpSocket>

namespace MailAdler { namespace Protokoll {

class SmtpVerbindung : public QObject {
    Q_OBJECT
public:
    explicit SmtpVerbindung(QObject *eltern = nullptr);
    ~SmtpVerbindung() override;

    void setzeServer(const QString &server);
    void setzePort(quint16 port);
    void setzeSocket(QTcpSocket *socket);
    void verbinden();

oeffentlicheSlots:
    void sende(const QString &absender, const QStringList &empfaenger,
               const QString &betreff, const QString &inhalt);

signale:
    void gesendet();
    void fehlerAufgetreten(const QString &meldung);

privateSlots:
    void beiBereitZumLesen();

private:
    enum class Schritt {
        WarteBegruessung, WarteEhlo, WarteAuthLogin,
        WarteBenutzer, WartePasswort, WarteAuthBestaetigt,
        WarteMailFrom, WarteRcptTo, WarteData, WarteNachrichtBestaetigt,
        Fertig
    };

    void sendeZeile(const QString &zeile);
    void verarbeiteAntwort(const QString &zeile);
    QString base64Codieren(const QString &text) const;

    QTcpSocket *m_verbindung = nullptr;
    QString m_server;
    quint16 m_port = 587;
    bool m_socketVonAussen = false;

    Schritt m_schritt = Schritt::WarteBegruessung;

    // Gespeicherte Parameter für sende()
    QString m_absender;
    QStringList m_empfaenger;
    QString m_betreff;
    QString m_inhalt;
    int m_aktuellerEmpfaenger = 0;
};

}} // namespace
```

**Implementierung (smtp_verbindung.cpp):**

```cpp
#include "smtp_verbindung.h"
#include <QtNetwork/QSslSocket>

namespace MailAdler { namespace Protokoll {

SmtpVerbindung::SmtpVerbindung(QObject *eltern) : QObject(eltern) {}
SmtpVerbindung::~SmtpVerbindung() {
    if (m_verbindung && !m_socketVonAussen) delete m_verbindung;
}

void SmtpVerbindung::setzeServer(const QString &s) { m_server = s; }
void SmtpVerbindung::setzePort(quint16 p) { m_port = p; }

void SmtpVerbindung::setzeSocket(QTcpSocket *s) {
    if (m_verbindung && !m_socketVonAussen) delete m_verbindung;
    m_verbindung = s;
    m_socketVonAussen = true;
    QObject::connect(s, &QTcpSocket::readyRead, this, &SmtpVerbindung::beiBereitZumLesen);
}

void SmtpVerbindung::verbinden() {
    if (!m_verbindung) {
        auto *ssl = new QSslSocket(this);
        QObject::connect(ssl, &QSslSocket::readyRead,
                         this, &SmtpVerbindung::beiBereitZumLesen);
        m_verbindung = ssl;
    }
    m_verbindung->connectToHost(m_server, m_port);
    m_schritt = Schritt::WarteBegruessung;
}

void SmtpVerbindung::sende(const QString &absender, const QStringList &empfaenger,
                           const QString &betreff, const QString &inhalt) {
    m_absender = absender;
    m_empfaenger = empfaenger;
    m_betreff = betreff;
    m_inhalt = inhalt;
    m_aktuellerEmpfaenger = 0;
    // Der SMTP-Dialog beginnt — erste Antwort kommt via beiBereitZumLesen()
}

void SmtpVerbindung::sendeZeile(const QString &zeile) {
    m_verbindung->write((zeile + "\r\n").toUtf8());
}

QString SmtpVerbindung::base64Codieren(const QString &text) const {
    return text.toUtf8().toBase64();
}

void SmtpVerbindung::beiBereitZumLesen() {
    while (m_verbindung->canReadLine()) {
        QString zeile = QString::fromUtf8(m_verbindung->readLine()).trimmed();
        if (zeile.isEmpty()) continue;
        verarbeiteAntwort(zeile);
    }
}

void SmtpVerbindung::verarbeiteAntwort(const QString &zeile) {
    int code = zeile.left(3).toInt();

    // 4xx/5xx = Fehler
    if (code >= 400) {
        m_schritt = Schritt::Fertig;
        emit fehlerAufgetreten(zeile);
        return;
    }

    switch (m_schritt) {
    case Schritt::WarteBegruessung:
        sendeZeile("EHLO mailadler");
        m_schritt = Schritt::WarteEhlo;
        break;

    case Schritt::WarteEhlo:
        if (code == 250) {
            sendeZeile("AUTH LOGIN");
            m_schritt = Schritt::WarteAuthLogin;
        }
        break;

    case Schritt::WarteAuthLogin:
        if (code == 334) {
            sendeZeile(base64Codieren(m_absender));  // Benutzername
            m_schritt = Schritt::WarteBenutzer;
        }
        break;

    case Schritt::WarteBenutzer:
        if (code == 334) {
            sendeZeile(base64Codieren(""));  // Passwort (TODO: aus KontoDienst)
            m_schritt = Schritt::WartePasswort;
        }
        break;

    case Schritt::WartePasswort:
        if (code == 235) {
            sendeZeile("MAIL FROM:<" + m_absender + ">");
            m_schritt = Schritt::WarteMailFrom;
        }
        break;

    case Schritt::WarteMailFrom:
        if (code == 250) {
            sendeZeile("RCPT TO:<" + m_empfaenger[m_aktuellerEmpfaenger] + ">");
            m_schritt = Schritt::WarteRcptTo;
        }
        break;

    case Schritt::WarteRcptTo:
        if (code == 250) {
            m_aktuellerEmpfaenger++;
            if (m_aktuellerEmpfaenger < m_empfaenger.size()) {
                sendeZeile("RCPT TO:<" + m_empfaenger[m_aktuellerEmpfaenger] + ">");
            } else {
                sendeZeile("DATA");
                m_schritt = Schritt::WarteData;
            }
        }
        break;

    case Schritt::WarteData:
        if (code == 354) {
            // Nachricht senden
            m_verbindung->write(
                QString("From: %1\r\nTo: %2\r\nSubject: %3\r\n\r\n%4\r\n.\r\n")
                    .arg(m_absender, m_empfaenger.join(", "), m_betreff, m_inhalt)
                    .toUtf8());
            m_schritt = Schritt::WarteNachrichtBestaetigt;
        }
        break;

    case Schritt::WarteNachrichtBestaetigt:
        if (code == 250) {
            m_schritt = Schritt::Fertig;
            emit gesendet();
        }
        break;

    case Schritt::Fertig:
        break;
    }
}

}} // namespace
```

**Bauen + Test:**

```bash
cmake --build bau --parallel $(nproc) 2>&1 | tail -10
ctest --test-dir bau --output-on-failure -R smtp_verbindung -V
```

Erwartet: 2 Tests PASS.

**Commit:**

```bash
git add pruefungen/tst_smtp_verbindung.cpp pruefungen/CMakeLists.txt \
        quelltext/bibliothek/protokoll/smtp_verbindung.h \
        quelltext/bibliothek/protokoll/smtp_verbindung.cpp
git commit -m "protokoll: SMTP-Verbindung mit TLS + AUTH + Tests"
```

---

## Zusammenfassung

| Aufgabe | Tests | Ergebnis |
|---|---|---|
| 1 | `sollteMailSenden`, `sollteFehlerMelden` | FakeSocket-Dialog |
| 2 | Implementierung Zustandsautomat | SMTP komplett |

**Einschränkung für v0.1:** Passwort wird noch nicht aus `KontoDienst` geholt — das `sende()` bekommt das Passwort später als Parameter oder liest es selbst aus der DB. Für den MVP reicht der harte `base64Codieren("")`-Platzhalter.

**Nach diesem Plan:** SMTP funktioniert. Nächster Plan: UI-Verknüpfung (ViewModel → Dienst → Protokoll).
