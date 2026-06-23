# Startup-Flow — Implementierungsplan

> **Für Hermes:** Nutze den `test-driven-development`-Skill, um diesen Plan TDD-basiert umzusetzen.

**Ziel:** Beim Start: Konto auswählen → IMAP verbinden → Nachrichten laden und anzeigen. Keine Test-Daten mehr.

**Architektur:** `main.cpp` startet mit Konto-Auswahl-Dialog. Bei bestehenden Konten: direkt verbinden. Sonst: Konto anlegen. `PostfachDienst` orchestriert IMAP-Abruf und Nachrichtenanzeige.

**Vorbedingung:** Pläne ①–⑥ abgeschlossen. Datenbank, Dienste, IMAP, UI funktionieren einzeln.

---

## Phase 1: PostfachDienst mit IMAP verdrahten

### Aufgabe 1: PostfachDienst.ordnerLaden() implementieren

**Ziel:** `PostfachDienst` ruft Ordnerliste via `ImapVerbindung` ab und speichert sie.

**Dateien:**
- Ändern: `quelltext/bibliothek/dienst/postfach_dienst.h`
- Ändern: `quelltext/bibliothek/dienst/postfach_dienst.cpp`
- Ändern: `pruefungen/tst_postfach_dienst.cpp`

**Schritt 1: Header erweitern**

```cpp
// In postfach_dienst.h — neue Includes + Methoden
#include "../protokoll/imap_verbindung.h"

class PostfachDienst : public QObject {
    Q_OBJECT
public:
    explicit PostfachDienst(Speicher::Zwischenspeicher *cache,
                            Protokoll::ImapVerbindung *imap = nullptr,
                            QObject *eltern = nullptr);

    void verbinden(const QString &server, quint16 port,
                   const QString &benutzer, const QString &passwort);
    void ordnerLaden();
    QStringList ordner() const;

    // Bestehende Methoden bleiben:
    int anzahl() const;
    QVector<Kern::Nachricht> nachrichten() const;
    void setzeNachrichten(const QVector<Kern::Nachricht> &nachrichten);

signals:
    void verbunden();
    void ordnerGeladen(const QStringList &ordner);
    void nachrichtenGeaendert();
    void fehlerAufgetreten(const QString &meldung);

private slots:
    void beiImapVerbunden();
    void beiImapAngemeldet();
    void beiOrdnerEmpfangen(const QStringList &ordner);

private:
    Speicher::Zwischenspeicher *m_cache = nullptr;
    Protokoll::ImapVerbindung *m_imap = nullptr;
    QStringList m_ordner;
};
```

**Schritt 2: Implementierung**

```cpp
PostfachDienst::PostfachDienst(Zwischenspeicher *cache, ImapVerbindung *imap, QObject *eltern)
    : QObject(eltern), m_cache(cache), m_imap(imap) {}

void PostfachDienst::verbinden(const QString &server, quint16 port,
                                const QString &benutzer, const QString &passwort) {
    if (!m_imap) { emit fehlerAufgetreten("Keine IMAP-Verbindung gesetzt"); return; }

    QObject::connect(m_imap, &ImapVerbindung::verbunden,
                     this, &PostfachDienst::beiImapVerbunden);
    QObject::connect(m_imap, &ImapVerbindung::angemeldet,
                     this, &PostfachDienst::beiImapAngemeldet);
    QObject::connect(m_imap, &ImapVerbindung::ordnerListeEmpfangen,
                     this, &PostfachDienst::beiOrdnerEmpfangen);
    QObject::connect(m_imap, &ImapVerbindung::fehlerAufgetreten,
                     this, &PostfachDienst::fehlerAufgetreten);

    m_imap->setzeServer(server);
    m_imap->setzePort(port);
    m_imap->verbinden();
}

void PostfachDienst::beiImapVerbunden() {
    // Login folgt automatisch
}

void PostfachDienst::beiImapAngemeldet() {
    emit verbunden();
    m_imap->ordnerListeAbrufen();
}

void PostfachDienst::beiOrdnerEmpfangen(const QStringList &ordner) {
    m_ordner = ordner;
    emit ordnerGeladen(ordner);
}

void PostfachDienst::ordnerLaden() {
    if (m_imap && m_imap->istVerbunden()) {
        m_imap->ordnerListeAbrufen();
    }
}

QStringList PostfachDienst::ordner() const { return m_ordner; }
```

**Schritt 3: Test**

```cpp
// In tst_postfach_dienst.cpp — neuer Test
void sollteMitImapVerbinden() {
    ImapVerbindung imap;
    FakeSocket *fake = new FakeSocket(&imap);
    imap.setzeSocket(fake);

    PostfachDienst dienst(m_cache, &imap);
    QSignalSpy spyVerbunden(&dienst, &PostfachDienst::verbunden);
    QSignalSpy spyOrdner(&dienst, &PostfachDienst::ordnerGeladen);

    dienst.verbinden("imap.test.de", 993, "u", "p");
    fake->simuliereEmpfang("* OK Ready\r\n");
    fake->simuliereEmpfang("A0001 OK LOGIN\r\n");
    fake->simuliereEmpfang("* LIST () \"/\" \"INBOX\"\r\nA0002 OK\r\n");

    QCOMPARE(spyVerbunden.count(), 1);
    QCOMPARE(spyOrdner.count(), 1);
    QCOMPARE(dienst.ordner(), QStringList{"INBOX"});
}
```

**Commit:**

```bash
git add quelltext/bibliothek/dienst/postfach_dienst.h \
        quelltext/bibliothek/dienst/postfach_dienst.cpp \
        pruefungen/tst_postfach_dienst.cpp
git commit -m "dienst: PostfachDienst mit IMAP verdrahtet (ordnerLaden)"
```

---

## Phase 2: Startup in main.cpp

### Aufgabe 2: main.cpp — echten Startup-Flow

**Ziel:** Beim Start: Datenbank öffnen → Konten prüfen → IMAP verbinden → Nachrichten zeigen.

**Dateien:**
- Ändern: `quelltext/anwendung/main.cpp`

**Neue main.cpp:**

```cpp
#include <QtCore/QUrl>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>

#include "speicher/datenbank.h"
#include "speicher/zwischenspeicher.h"
#include "dienst/konto_dienst.h"
#include "dienst/postfach_dienst.h"
#include "protokoll/imap_verbindung.h"
#include "ansichtmodelle/nachrichten_liste_modell.h"
#include "ansichtmodelle/erstellen_ansicht_modell.h"

int main(int anzahlArgumente, char *argumente[]) {
    QGuiApplication anwendung(anzahlArgumente, argumente);
    anwendung.setApplicationName("MailAdler");
    anwendung.setApplicationVersion("1.0.0");
    anwendung.setOrganizationName("MailAdler");

    // --- Schichten aufbauen ---
    auto *datenbank = new MailAdler::Speicher::Datenbank(&anwendung);
    QString dbPfad = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                     + "/mailadler.db";
    QDir().mkpath(QFileInfo(dbPfad).absolutePath());
    datenbank->oeffne(dbPfad);

    auto *kontoDienst = new MailAdler::Dienst::KontoDienst(datenbank, &anwendung);
    auto *cache = new MailAdler::Speicher::Zwischenspeicher(&anwendung);
    auto *imap = new MailAdler::Protokoll::ImapVerbindung(&anwendung);
    auto *postfachDienst = new MailAdler::Dienst::PostfachDienst(cache, imap, &anwendung);

    // --- ViewModels ---
    auto *nachrichtenModell = new MailAdler::NachrichtenListeModell(&anwendung);
    auto *erstellenModell = new MailAdler::ErstellenAnsichtModell(&anwendung);

    // --- QML-Engine ---
    QQmlApplicationEngine maschine;
    maschine.rootContext()->setContextProperty("nachrichtenListeModell", nachrichtenModell);
    maschine.rootContext()->setContextProperty("erstellenAnsichtModell", erstellenModell);

    // --- Startup: Erstes Konto verbinden ---
    auto konten = kontoDienst->alleKonten();
    if (!konten.isEmpty()) {
        auto &k = konten.first();
        QObject::connect(postfachDienst, &MailAdler::Dienst::PostfachDienst::ordnerGeladen,
                         &anwendung, [&](const QStringList &) {
            // TODO: Nachrichten für INBOX laden
        });
        postfachDienst->verbinden(k.imapServer, k.imapPort, k.benutzer, k.passwort);
    }

    maschine.load(QUrl("qrc:/MailAdler/HauptFenster.qml"));

    if (maschine.rootObjects().isEmpty()) return -1;
    return anwendung.exec();
}
```

**Bauen + testen:**

```bash
cmake --build bau --parallel $(nproc)
```

Erwartet: Build erfolgreich. App startet, verbindet mit erstem Konto (wenn vorhanden).

**Commit:**

```bash
git add quelltext/anwendung/main.cpp
git commit -m "anwendung: Startup-Flow — DB→Konto→IMAP→Nachrichten"
```

---

## Zusammenfassung

| Phase | Ergebnis |
|---|---|
| 1 | PostfachDienst orchestriert IMAP-Verbindung + Ordnerabruf |
| 2 | main.cpp: Echter Startup-Flow ohne Test-Daten |

**Nach diesem Plan:** Die App startet, verbindet mit IMAP und zeigt echte Ordner. Der nächste Plan (E2E-Tests) prüft den Gesamtfluss.
