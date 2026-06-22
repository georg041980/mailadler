# QML-Fenster & Integration — v0.1 MVP-Plan

> **Für Hermes:** Task für Task implementieren. TDD: erst Test, dann Code, dann Commit.

**Ziel:** Lauffähiges QML-Fenster mit echter Nachrichtenliste — alle Schichten verdrahtet: IMAP → Dienst → ViewModel → QML.

**Architektur:** C++-ViewModels (QAbstractListModel, Q_PROPERTY) beliefern QML-Deklarativ-UI. Datenfluss über Signale. Keine Logik im QML.

**Tech-Stack:** Qt 6.4.2, QML, C++17, CMake+Ninja, QtTest+Mock-Sockets.

**Stand:** ✅ ImapVerbindung (echtes Protokoll), ✅ Datenbank (CRUD), ✅ ViewModel-Stubs, ⚠ QML-Stubs (Hardcoded-Daten), ❌ Keine C++→QML-Verdrahtung.

---

## Phase 1: C++→QML-Verdrahtung (main.cpp)

### Task 1: NachrichtenListeModell in main.cpp registrieren

**Ziel:** QML kann auf `nachrichtenListeModell` zugreifen.

**Dateien:**
- Ändern: `quelltext/anwendung/main.cpp`

**Schritt 1: main.cpp um Context-Property und Testdaten erweitern**

```cpp
#include <QtCore/QUrl>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>

#include "ansichtmodelle/nachrichten_liste_modell.h"
#include "kern/nachricht.h"

using AdlerMail::NachrichtenListeModell;
using AdlerMail::Kern::Nachricht;

int main(int anzahlArgumente, char *argumente[])
{
    QGuiApplication anwendung(anzahlArgumente, argumente);
    anwendung.setApplicationName("AdlerMail");
    anwendung.setApplicationVersion("0.1.0");
    anwendung.setOrganizationName("AdlerMail");

    // ViewModel mit Testdaten
    auto *modell = new NachrichtenListeModell(&anwendung);
    QVector<Nachricht> testDaten = {
        {1, "max@beispiel.de", "Willkommen bei AdlerMail",
         "Hallo! Dies ist Ihre erste E-Mail.", "", QDateTime::currentDateTime(), false, false},
        {2, "info@qt.io", "Qt 6.7 veröffentlicht",
         "Das Qt-Team freut sich, Qt 6.7 anzukündigen…", "", QDateTime::currentDateTime().addSecs(-3600), false, false},
        {3, "github@notifications.com", "Neuer Commit in mailadler",
         "georg0480 hat einen neuen Commit gepusht.", "", QDateTime::currentDateTime().addSecs(-7200), true, false},
    };
    modell->setzeNachrichten(testDaten);

    QQmlApplicationEngine maschine;
    maschine.rootContext()->setContextProperty("nachrichtenListeModell", modell);
    maschine.load(QUrl("qrc:/AdlerMail/HauptFenster.qml"));

    if (maschine.rootObjects().isEmpty()) return -1;
    return anwendung.exec();
}
```

**Schritt 2: Bauen und prüfen**

```bash
cmake --build bau
```

Erwartet: Build OK, App startet mit 3 Nachrichten in der Liste.

**Schritt 3: Commit**

```bash
git add quelltext/anwendung/main.cpp
git commit -m "oberflaeche: C++-ViewModel in main.cpp verdrahtet — 3 Testnachrichten"
```

---

### Task 2: ErstellenAnsichtModell in main.cpp registrieren

**Ziel:** QML kann auf `erstellenAnsichtModell` zugreifen.

**Dateien:**
- Ändern: `quelltext/anwendung/main.cpp`

**Schritt 1: Zweites Context-Property**

```cpp
#include "ansichtmodelle/erstellen_ansicht_modell.h"
using AdlerMail::ErstellenAnsichtModell;

// in main():
auto *erstellenModell = new ErstellenAnsichtModell(&anwendung);
maschine.rootContext()->setContextProperty("erstellenAnsichtModell", erstellenModell);
```

**Schritt 2: Bauen**

**Schritt 3: Commit**

---

## Phase 2: QML an ViewModels binden

### Task 3: NachrichtenListe.qml — Model-Rollen korrigieren

**Ziel:** QML-Delegate nutzt die richtigen Rollennamen aus `roleNames()`.

**Problem:** `roleNames()` liefert `"betreff"`, `"absender"`, `"datum"`, `"gelesen"`. QML nutzt bereits diese Namen → läuft schon, nur mit Hardcoded-Daten.

**Dateien:**
- Ändern: `quelltext/oberflaeche/qml/NachrichtenListe.qml`

**Schritt 1: Keine Änderung nötig — Rollen passen bereits.**

Verifikation: App starten → 3 Nachrichten sichtbar.

**Commit entfällt (keine Änderung).**

---

### Task 4: ErstellenAnsicht.qml an ViewModel binden

**Ziel:** TextField-Werte sind an `erstellenAnsichtModell` Properties gebunden.

**Dateien:**
- Ändern: `quelltext/oberflaeche/qml/ErstellenAnsicht.qml`

**Schritt 1: Bindings einbauen**

```qml
TextField {
    Layout.fillWidth: true
    placeholderText: "empfaenger@beispiel.de"
    text: erstellenAnsichtModell.an
    onTextChanged: erstellenAnsichtModell.an = text
}
// Betreff:
TextField {
    Layout.fillWidth: true
    placeholderText: "Betreff"
    text: erstellenAnsichtModell.betreff
    onTextChanged: erstellenAnsichtModell.betreff = text
}
// Inhalt:
TextArea {
    placeholderText: "Nachrichtentext..."
    wrapMode: TextEdit.WordWrap
    text: erstellenAnsichtModell.inhalt
    onTextChanged: erstellenAnsichtModell.inhalt = text
}
// Senden-Button:
Button {
    text: "Senden"
    enabled: erstellenAnsichtModell.kannSenden
    Layout.alignment: Qt.AlignRight
}
```

**Schritt 2: Bauen, App starten, prüfen:** Button nur aktiv wenn An+Betreff ausgefüllt.

**Schritt 3: Commit**

---

### Task 5: HauptFenster.qml — Ordnerleiste an ViewModel binden

**Ziel:** Linke Ordnerleiste wird aus C++-Daten befüllt (nicht hardcoded).

**Dateien:**
- Ändern: `quelltext/oberflaeche/qml/HauptFenster.qml`
- Neu: `quelltext/oberflaeche/ansichtmodelle/ordner_liste_modell.h`
- Neu: `quelltext/oberflaeche/ansichtmodelle/ordner_liste_modell.cpp`

**Schritt 1: OrdnerListeModell erstellen** (QStringList als Model)

```cpp
// ordner_liste_modell.h
#pragma once
#include <QtCore/QStringListModel>

namespace AdlerMail {

class OrdnerListeModell : public QStringListModel {
    Q_OBJECT
public:
    explicit OrdnerListeModell(QObject *eltern = nullptr);
    void setzeOrdner(const QStringList &ordner);
};

} // namespace
```

```cpp
// ordner_liste_modell.cpp
#include "ordner_liste_modell.h"

namespace AdlerMail {

OrdnerListeModell::OrdnerListeModell(QObject *eltern) : QStringListModel(eltern) {}

void OrdnerListeModell::setzeOrdner(const QStringList &ordner) {
    setStringList(ordner);
}

} // namespace
```

**Schritt 2: In CMakeLists.txt registrieren**

**Schritt 3: In main.cpp registrieren + Testdaten**

```cpp
#include "ansichtmodelle/ordner_liste_modell.h"
auto *ordnerModell = new OrdnerListeModell(&anwendung);
ordnerModell->setzeOrdner({"INBOX", "Gesendet", "Entwürfe", "Papierkorb"});
maschine.rootContext()->setContextProperty("ordnerListeModell", ordnerModell);
```

**Schritt 4: HauptFenster.qml umstellen**

```qml
ListView {
    anchors.fill: parent
    anchors.margins: 4
    model: ordnerListeModell
    delegate: Text {
        text: model.display
        font.pixelSize: 14
        padding: 8
    }
}
```

**Schritt 5: Bauen, prüfen, commit**

---

## Phase 3: PostfachDienst + ImapVerbindung integrieren

### Task 6: PostfachDienst um ImapVerbindung erweitern

**Ziel:** PostfachDienst kann über ImapVerbindung echte Ordnerliste und Nachrichten laden.

**Dateien:**
- Ändern: `quelltext/bibliothek/dienst/postfach_dienst.h`
- Ändern: `quelltext/bibliothek/dienst/postfach_dienst.cpp`

**Schritt 1: PostfachDienst.h erweitern**

```cpp
#pragma once
#include <QtCore/QObject>
#include <QtCore/QVector>
#include "../kern/nachricht.h"

namespace AdlerMail {

namespace Speicher { class Zwischenspeicher; }
namespace Protokoll { class ImapVerbindung; }

namespace Dienst {

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
    void nachrichtenLaden(const QString &ordnerName);

signals:
    void nachrichtenGeaendert();
    void ordnerListeGeaendert(const QStringList &ordner);
    void fehlerAufgetreten(const QString &meldung);

private slots:
    void beiOrdnerListeEmpfangen(const QStringList &ordner);

private:
    Speicher::Zwischenspeicher *m_cache = nullptr;
    Protokoll::ImapVerbindung  *m_imap = nullptr;
};

}} // namespace
```

**Schritt 2: PostfachDienst.cpp implementieren**

```cpp
#include "postfach_dienst.h"
#include "../speicher/zwischenspeicher.h"
#include "../protokoll/imap_verbindung.h"

namespace AdlerMail { namespace Dienst {

PostfachDienst::PostfachDienst(Speicher::Zwischenspeicher *cache, QObject *eltern)
    : QObject(eltern), m_cache(cache) {}

void PostfachDienst::setzeImapVerbindung(Protokoll::ImapVerbindung *imap) {
    m_imap = imap;
    if (m_imap) {
        connect(m_imap, &Protokoll::ImapVerbindung::ordnerListeEmpfangen,
                this, &PostfachDienst::beiOrdnerListeEmpfangen);
    }
}

void PostfachDienst::ordnerLaden() {
    if (!m_imap || !m_imap->istAngemeldet()) {
        emit fehlerAufgetreten("Nicht verbunden — kann Ordner nicht laden");
        return;
    }
    m_imap->ordnerListeAbrufen();
}

void PostfachDienst::nachrichtenLaden(const QString & /*ordnerName*/) {
    // TODO: IMAP SELECT + FETCH — Phase 4
    emit fehlerAufgetreten("nachrichtenLaden noch nicht implementiert");
}

void PostfachDienst::beiOrdnerListeEmpfangen(const QStringList &ordner) {
    emit ordnerListeGeaendert(ordner);
}

int PostfachDienst::anzahl() const {
    return m_cache->alle().size();
}

QVector<Kern::Nachricht> PostfachDienst::nachrichten() const {
    return m_cache->alle();
}

void PostfachDienst::setzeNachrichten(const QVector<Kern::Nachricht> &n) {
    m_cache->setze(n);
    emit nachrichtenGeaendert();
}

}} // namespace
```

**Schritt 3: Test schreiben (tst_postfach_dienst erweitern)**

```cpp
void sollteOrdnerLadenMitImap() {
    // Mock-IMAP-Server starten
    MockImapServer imapServer;
    QVERIFY(imapServer.starte(0));

    ImapVerbindung imap;
    imap.setzeTls(false);
    imap.setzeServer("127.0.0.1");
    imap.setzePort(imapServer.serverPort());

    Zwischenspeicher cache;
    PostfachDienst dienst(&cache);
    dienst.setzeImapVerbindung(&imap);

    QSignalSpy verbundenSpion(&imap, &ImapVerbindung::verbunden);
    QSignalSpy angemeldetSpion(&imap, &ImapVerbindung::angemeldet);
    QSignalSpy ordnerSpion(&dienst, &PostfachDienst::ordnerListeGeaendert);

    imap.verbinden();
    QVERIFY(verbundenSpion.wait(3000));
    imap.anmelden("u", "p");
    QVERIFY(angemeldetSpion.wait(3000));

    dienst.ordnerLaden();
    QVERIFY(ordnerSpion.wait(3000));
    QCOMPARE(ordnerSpion.count(), 1);
}
```

**Schritt 4: Bauen, testen, commit**

---

### Task 7: main.cpp — PostfachDienst verdrahten

**Ziel:** OrdnerListeModell wird vom PostfachDienst befüllt.

**Dateien:**
- Ändern: `quelltext/anwendung/main.cpp`

**Schritt 1: main.cpp um PostfachDienst erweitern**

```cpp
#include "dienst/postfach_dienst.h"
#include "speicher/zwischenspeicher.h"
#include "protokoll/imap_verbindung.h"

using AdlerMail::Dienst::PostfachDienst;
using AdlerMail::Speicher::Zwischenspeicher;
using AdlerMail::Protokoll::ImapVerbindung;

// in main():
auto *cache = new Zwischenspeicher(&anwendung);
auto *postfachDienst = new PostfachDienst(cache, &anwendung);

// Optional: IMAP (wenn ein Server konfiguriert ist)
// auto *imap = new ImapVerbindung(&anwendung);
// imap->setzeServer("imap.beispiel.de");
// imap->setzePort(993);
// postfachDienst->setzeImapVerbindung(imap);
// imap->verbinden();

// Ordner aus Dienst (oder Fallback-Testdaten)
connect(postfachDienst, &PostfachDienst::ordnerListeGeaendert,
        ordnerModell, &OrdnerListeModell::setzeOrdner);
```

**Schritt 2: Bauen, prüfen, commit**

---

## Phase 4: NachrichtAnsicht dynamisch

### Task 8: NachrichtAnsicht — Properties statt Hardcoding

**Ziel:** NachrichtAnsicht zeigt die aktuell ausgewählte Nachricht.

**Dateien:**
- Neu: `quelltext/oberflaeche/ansichtmodelle/nachricht_ansicht_modell.h`
- Neu: `quelltext/oberflaeche/ansichtmodelle/nachricht_ansicht_modell.cpp`
- Ändern: `quelltext/oberflaeche/qml/NachrichtAnsicht.qml`
- Ändern: `quelltext/oberflaeche/CMakeLists.txt`
- Ändern: `quelltext/anwendung/main.cpp`

**Schritt 1: NachrichtAnsichtModell erstellen**

```cpp
// nachricht_ansicht_modell.h
#pragma once
#include <QtCore/QObject>
#include <QtCore/QString>
#include "../../bibliothek/kern/nachricht.h"

namespace AdlerMail {

class NachrichtAnsichtModell : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString absender READ absender NOTIFY nachrichtGeaendert)
    Q_PROPERTY(QString betreff  READ betreff  NOTIFY nachrichtGeaendert)
    Q_PROPERTY(QString inhalt   READ inhalt   NOTIFY nachrichtGeaendert)
    Q_PROPERTY(QString datum    READ datum    NOTIFY nachrichtGeaendert)
    Q_PROPERTY(bool hatNachricht READ hatNachricht NOTIFY nachrichtGeaendert)

public:
    explicit NachrichtAnsichtModell(QObject *eltern = nullptr);

    QString absender() const;
    QString betreff() const;
    QString inhalt() const;
    QString datum() const;
    bool hatNachricht() const;

    void setzeNachricht(const Kern::Nachricht &nachricht);
    void leeren();

signals:
    void nachrichtGeaendert();

private:
    Kern::Nachricht m_nachricht;
    bool m_hatNachricht = false;
};

} // namespace
```

**Schritt 2: NachrichtAnsicht.qml binden**

```qml
Text {
    text: nachrichtAnsichtModell.betreff ? "Betreff: " + nachrichtAnsichtModell.betreff : ""
    font.pixelSize: 18
    font.bold: true
}
Text {
    text: nachrichtAnsichtModell.absender ? "Von: " + nachrichtAnsichtModell.absender : ""
    font.pixelSize: 13
    color: "#555555"
}
Text {
    text: nachrichtAnsichtModell.inhalt
    Layout.fillWidth: true
    Layout.fillHeight: true
    font.pixelSize: 14
    wrapMode: Text.WordWrap
}
```

**Schritt 3: Bauen, prüfen, commit**

---

## Phase 5: SMTP-Verbindung (wie IMAP)

### Task 9: SMTP-Stub durch echte SMTP-Logik ersetzen

**Ziel:** Wie IMAP, nur für SMTP — Verbindung + AUTH + Senden.

**Analog zu IMAP-Implementierung, mit diesen Unterschieden:**
- Port 587 (STARTTLS) oder 465 (SSL)
- Befehle: EHLO, AUTH LOGIN, MAIL FROM, RCPT TO, DATA, QUIT
- Signal `gesendet()` statt `ordnerListeEmpfangen()`

**Dateien:**
- Ändern: `quelltext/bibliothek/protokoll/smtp_verbindung.h`
- Ändern: `quelltext/bibliothek/protokoll/smtp_verbindung.cpp`
- Neu: `pruefungen/tst_smtp_verbindung.cpp`

**Gleiches Muster wie `tst_imap_verbindung` — Mock-SMTP-Server mit QTcpServer.**

---

## Phase 6: Roundtrip — ErstellenAnsicht senden

### Task 10: Senden-Button verdrahten

**Ziel:** Button in ErstellenAnsicht sendet E-Mail über SMTP.

**Dateien:**
- Ändern: `quelltext/oberflaeche/qml/ErstellenAnsicht.qml`
- Ändern: `quelltext/anwendung/main.cpp`

**Schritt 1: main.cpp — KontoDienst für erstes aktives Konto abrufen**

```cpp
// SMTP-Verbindung aus KontoDaten erstellen
auto konten = kontoDienst->alleKonten();
if (!konten.isEmpty()) {
    auto *smtp = new SmtpVerbindung(&anwendung);
    smtp->setzeServer(konten[0].smtpServer);
    smtp->setzePort(konten[0].smtpPort);
    smtp->setzeTls(false); // Test-Modus
    // Verbindung im ViewModel hinterlegen
}
```

**Schritt 2: QML-Senden-Button onClicked → C++-Signal**

---

## Zusammenfassung: Reihenfolge

| # | Task | Neu/Ändern |
|---|------|-----------|
| 1 | main.cpp: NachrichtenListeModell registrieren | Ändern |
| 2 | main.cpp: ErstellenAnsichtModell registrieren | Ändern |
| 3 | NachrichtenListe.qml: Rollen prüfen | Ändern |
| 4 | ErstellenAnsicht.qml: Properties binden | Ändern |
| 5 | OrdnerListeModell + QML-Bindung | Neu |
| 6 | PostfachDienst + ImapVerbindung | Ändern |
| 7 | main.cpp: PostfachDienst verdrahten | Ändern |
| 8 | NachrichtAnsichtModell + QML | Neu |
| 9 | SMTP-Verbindung (echt) | Ändern |
| 10 | ErstellenAnsicht senden | Ändern |

**MVP nach Phase 3:** Fenster zeigt Ordnerliste + Nachrichtenliste mit echten Daten.  
**MVP nach Phase 6:** Volle Runde: Nachrichten lesen und senden.
