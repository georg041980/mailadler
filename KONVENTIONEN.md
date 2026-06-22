# AdlerMail — Programmierkonventionen

> Stand: 22. Juni 2026  
> Verbindlich für alle Mitwirkenden.  
> Ziel: Quellcode, der aussieht wie von einer Person geschrieben.

---

## 1. Sprache

**Der gesamte selbstgeschriebene Code ist deutsch.**
Klassen, Methoden, Variablen, Kommentare, Commit-Messages, Fehlermeldungen — alles.

**Ausnahme:** C++/Qt-Schlüsselwörter und Makros bleiben englisch.
Der MOC-Parser und der Compiler verstehen kein Deutsch.
`signals`, `slots`, `Q_OBJECT`, `Q_PROPERTY`, `override`, `nullptr`, `explicit` — unverändert.

Kein Denglisch. Entweder ganz deutsch (`nachricht()`) oder ganz englisch (`getMessage()`),
niemals `getNachricht()`.

**Umlaute** in Bezeichnern sind erlaubt (C++11).
Wer sie nicht mag, schreibt `ae`/`oe`/`ue`/`ss` (also `setzeGroesse` statt `setzeGröße`).
Einmal im Team entscheiden, dann fürs ganze Projekt durchziehen.

---

## 2. Namenskonventionen

| Was                  | Regel                        | Beispiel                          |
|----------------------|------------------------------|-----------------------------------|
| Klassen              | PascalCase, deutsch          | `Postfach`, `NachrichtenListe`    |
| Aufzählungen         | PascalCase, deutsch          | `VerbindungsStatus`               |
| Member-Variablen     | `m_`-Präfix, camelCase       | `m_verbindung`, `m_nachrichten`   |
| Getter               | `ist`/`hat` für `bool`, sonst Nomen | `istVerbunden()`, `name()` |
| Setter               | `setze` + Name               | `setzeServer()`, `setzePort()`    |
| Signale              | Verb im Partizip Perfekt, oder Nomen + `Geaendert` | `verbunden()`, `ordnerListeGeaendert()` |
| Slots                | `bei` + Quelle + Ereignis    | `beiVerbunden()`, `beiSendenGeklickt()` |
| Dateinamen           | Kleinschreibung, Unterstriche | `postfach.h`, `nachrichten_dienst.cpp` |
| Namespaces           | PascalCase, deutsch          | `AdlerMail::Kern`, `AdlerMail::Protokoll` |
| Makros (selten)      | GROSSBUCHSTABEN              | `ADLERMAIL_VERSION`               |

---

## 3. Header-Struktur

Jeder Header folgt dieser Reihenfolge:

```cpp
#pragma once                                      // kein #ifndef-Salat

// 1. Qt-Includes (alphabetisch)
#include <QtCore/QObject>
#include <QtNetwork/QTcpSocket>

// 2. Projekt-Includes
#include "kern/nachricht.h"

// 3. Projekt-Namespace + Bereich
namespace AdlerMail {
namespace Protokoll {

class ImapVerbindung : public QObject {
    Q_OBJECT

    // --- Öffentliche Schnittstelle ---
public:
    explicit ImapVerbindung(QObject *eltern = nullptr);
    ~ImapVerbindung() override;

    // Q_PROPERTY oben, dann C++-Getter/Setter
    QString server() const;
    void setzeServer(const QString &server);
    bool istVerbunden() const;

    // API-Methoden
    void verbinden();
    void trennen();

    // --- Signale (Qt-Schlüsselwort: signals — MOC braucht das englische Original) ---
signals:
    void verbunden();
    void getrennt();
    void fehlerAufgetreten(const QString &meldung);

    // --- Öffentliche Slots ---
public slots:
    void anmelden(const QString &benutzer, const QString &passwort);

    // --- Private Slots ---
private slots:
    void beiVerbunden();
    void beiBereitZumLesen();

    // --- Privat ---
private:
    QTcpSocket *m_verbindung = nullptr;
    QString m_server;
    quint16 m_port = 993;
};

} // namespace Protokoll
} // namespace AdlerMail
```

**Merksatz:** `public` → `signale` → `oeffentlicheSlots` → `privateSlots` → `private`.
Immer diese Reihenfolge. Immer dieser Aufbau.

---

## 4. Funktions-Signaturen

```cpp
// ✓ const-&-Parameter für Qt-Typen
void setzeName(const QString &name);
void setzeListe(const QVector<Nachricht> &nachrichten);

// ✓ by-value für primitive Typen
void setzePort(int port);
void setzeAktiv(bool aktiv);

// ✓ Rückgabe von Containern — return by value (RVO/NRVO)
QVector<Nachricht> nachrichtenFuerOrdner(const QString &ordner) const;

// ✗ VERBOTEN — Output-Parameter (versteckte Seiteneffekte)
void nachrichtenFuerOrdner(const QString &ordner, QVector<Nachricht> &ausgabe);

// ✗ VERBOTEN — raw-Pointer für Besitzübergabe
void setzeDatenbank(Datenbank *db);

// ✓ Korrekt — Besitz klar geregelt
void setzeDatenbank(std::unique_ptr<Datenbank> db);  // Besitz geht an diese Klasse
void setzeDatenbank(Datenbank &db);                  // Besitz bleibt beim Aufrufer
```

---

## 5. Fehlerbehandlung

**Keine Exceptions im Qt-Projekt.** Qt selbst wirft keine.
Mischmasch aus Exceptions und Signal/Slot führt zu schwer auffindbaren Fehlern.

**Einheitlich über Signale:**

```cpp
// In der Implementierung:
void ImapVerbindung::anmelden(const QString &benutzer, const QString &passwort) {
    if (!m_verbindung || !m_verbindung->isOpen()) {
        emit fehlerAufgetreten("Keine Verbindung zum Server");
        return;
    }
    // ...
}

// Beim Aufrufer:
verbinde(verbindung, &ImapVerbindung::fehlerAufgetreten,
         this, [](const QString &meldung) {
    qWarning() << "IMAP-Fehler:" << meldung;
    // UI-Benachrichtigung anzeigen
});
```

**Pro Klasse maximal ein Fehlersignal** (`fehlerAufgetreten(QString)`).
Ein `enum` für Fehlerkategorien erst dann einführen, wenn die Aufrufer wirklich
unterscheiden müssen. Vorher ist es YAGNI.

---

## 6. Q_PROPERTY (ViewModel-Bindung an QML)

```cpp
class ErstellenAnsichtModell : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString an    READ an    WRITE setzeAn    NOTIFY anGeaendert)
    Q_PROPERTY(QString betreff READ betreff WRITE setzeBetreff NOTIFY betreffGeaendert)
    Q_PROPERTY(QString inhalt READ inhalt WRITE setzeInhalt NOTIFY inhaltGeaendert)
    Q_PROPERTY(bool kannSenden  READ kannSenden  NOTIFY kannSendenGeaendert)

public:
    QString an() const { return m_an; }
    void setzeAn(const QString &an) {
        if (m_an != an) {              // Guard — kein unnötiges Signal
            m_an = an;
            emit anGeaendert();
            aktualisiereKannSenden();
        }
    }

    bool kannSenden() const { return m_kannSenden; }

signale:
    void anGeaendert();
    void betreffGeaendert();
    void inhaltGeaendert();
    void kannSendenGeaendert();

private:
    void aktualisiereKannSenden() {
        bool ok = !m_an.isEmpty() && !m_betreff.isEmpty();
        if (m_kannSenden != ok) {
            m_kannSenden = ok;
            emit kannSendenGeaendert();
        }
    }

    QString m_an, m_betreff, m_inhalt;
    bool m_kannSenden = false;
};
```

**Kernidee:** QML macht nur Darstellung. Alle Logik sitzt im C++-ViewModel.
Testbar, debugbar, von einer Person in 6 Monaten noch verständlich.

---

## 7. Kommentare (Doxygen light)

```cpp
/**
 * Stellt eine IMAP-Verbindung zu einem Mail-Server her.
 *
 * Alle Vorgänge laufen asynchron. Ergebnisse werden über Signale gemeldet.
 *
 * @thread Nicht threadsicher — nur im GUI-Strang verwenden.
 */
class ImapVerbindung : public QObject {

    /**
     * Startet den Anmeldevorgang.
     *
     * @param benutzer  Benutzername (meist E-Mail-Adresse)
     * @param passwort  Passwort oder App-Passwort
     *
     * @signale verbunden() bei Erfolg
     * @signale fehlerAufgetreten() bei Fehlschlag
     */
    void anmelden(const QString &benutzer, const QString &passwort);
};
```

**Regeln für Kommentare:**

- Jede **öffentliche** Methode bekommt `@brief` (erste Zeile nach `/**`).
- Private Methoden nur kommentieren, wenn die Logik nicht-trivial ist.
- Wenn eine Methode mehr als 5 Zeilen Kommentar braucht: Methode ist zu lang → aufteilen.
- Kommentare beschreiben WAS und WARUM, nicht WIE (das steht im Code).
- Umgangssprachlich, aber präzise. Kein Geschwafel.

---

## 8. Projektstruktur

```
adlermail/
├── CMakeLists.txt
├── cmake/
│   └── KompilierWarnungen.cmake
├── vorlagen/                          # Templates für neue Dateien
│   ├── header_vorlage.h
│   └── test_vorlage.cpp
├── quelltext/                         # src/
│   ├── CMakeLists.txt
│   ├── bibliothek/                    # lib/ — Geschäftslogik (statisch)
│   │   ├── CMakeLists.txt
│   │   ├── kern/                      # Datenmodelle
│   │   ├── protokoll/                 # IMAP, SMTP
│   │   ├── speicher/                  # SQLite
│   │   └── dienst/                    # High-Level-Dienste
│   ├── anwendung/                     # app/ — main.cpp
│   └── oberflaeche/                   # ui/ — QML + ViewModels
├── uebersetzungen/                    # .ts-Dateien
├── pruefungen/                        # tests/
├── verpackung/                        # Flatpak, Windows-Installer
├── KONVENTIONEN.md                    # Diese Datei
├── .clang-format
├── .clang-tidy
└── README.md
```

Deutsche Verzeichnisnamen sind Teil der Konvention.
Wer das Projekt öffnet, sieht sofort: Hier wird deutsch programmiert.

---

## 9. Formatierung (automatisch)

**Kein Mensch diskutiert über Einrückungen.**
`clang-format` formatiert alles. Die Konfiguration liegt in `.clang-format` (Allman-Stil, 4 Leerzeichen).

Vor jedem Commit:
```bash
clang-format -i quelltext/**/*.cpp quelltext/**/*.h pruefungen/**/*.cpp
```

In der CI wird `clang-format --dry-run --Werror` ausgeführt.
Wer falsch formatierten Code pusht, bekommt keinen Merge.

---

## 10. Commit-Messages

```
Bereich: Imperative Kurzfassung (max 72 Zeichen)

- Was geändert wurde
- Warum geändert (nicht: Wie)
- BREAKING: markieren, wenn vorhanden
```

Beispiele:
```
protokoll: TLS-Verbindungsabbruch-Zeit auf 30s erhöht

Gmail trennt bei langsamen Verbindungen nach 20s.
Die alte 10s-Zeit führte zu falschen fehlerAufgetreten-Signalen.
```

```
speicher: EINDEUTIG-Einschränkung auf email-Spalte hinzugefügt

BREAKING: Doppelte Konten müssen vor dem Update entfernt werden.
```

---

## 11. Automatische Prüfungen (CI)

In der CI-Pipeline laufen **vor jedem Merge**:

1. `clang-format --dry-run --Werror` — Formatierung
2. `cmake --build bau --target all_ts` — Übersetzungen aktuell?
3. `ctest --test-dir bau --output-on-failure` — Alle Tests bestanden?

Alle drei müssen grün sein. Keine Ausnahmen. Kein "mach ich später".
