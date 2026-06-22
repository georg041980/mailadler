# Datenbank-Klasse — Implementierungsplan

> **Für Hermes:** Nutze den `test-driven-development`-Skill, um diesen Plan TDD-basiert umzusetzen.

**Ziel:** Die `Datenbank`-Klasse in `quelltext/bibliothek/speicher/datenbank.h/.cpp` vollständig implementieren, sodass Konten gespeichert, gelesen und gelöscht werden können. Mit Tests.

**Architektur:** QtSql + SQLite (eingebettet, keine externe DB). In-Memory-DB für Tests (`:memory:`). Keine GUI-Abhängigkeit. Schicht `Speicher` → wird von `Dienst` genutzt.

**Tech-Stack:** Qt 6.4.2, QtSql, QTest, CTest, C++17.

---

## Vorbereitung: Test-Build aktivieren

### Aufgabe 0: pruefungen in CMake einklinken

**Ziel:** `ctest` findet die Tests.

**Dateien:**
- Ändern: `CMakeLists.txt:40`
- Ändern: `pruefungen/CMakeLists.txt:25-29`

**Schritt 1: Root-CMake aktivieren**

In `CMakeLists.txt`, Zeile 40:
```cmake
# add_subdirectory(pruefungen)  # aktivieren, wenn erste Tests vorhanden
```
Ersetzen durch:
```cmake
add_subdirectory(pruefungen)
```

**Schritt 2: Ersten Test entkommentieren**

In `pruefungen/CMakeLists.txt`, Zeilen 25-29:
```cmake
# adlermail_pruefung_hinzufuegen(tst_datenbank)
```
Ersetzen durch:
```cmake
adlermail_pruefung_hinzufuegen(tst_datenbank)
```

**Schritt 3: Bauen + prüfen**

```bash
cmake -B bau -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DSICHERER_BAU=ON -GNinja
cmake --build bau --parallel $(nproc)
ctest --test-dir bau --output-on-failure
```

Erwartet: FEHLER — `tst_datenbank.cpp` existiert noch nicht.

**Schritt 4: Commit**

```bash
git add CMakeLists.txt pruefungen/CMakeLists.txt
git commit -m "build: pruefungen einklinken, tst_datenbank vorbereitet"
```

---

## Phase 1: Datenbank öffnen und schließen

### Aufgabe 1: Test — öffnen und schließen

**Ziel:** Datenbank lässt sich mit In-Memory-Pfad öffnen und schließen.

**Dateien:**
- Erstellen: `pruefungen/tst_datenbank.cpp`
- Vorlage: `vorlagen/test_vorlage.cpp`

**Code (komplett):**

```cpp
// pruefungen/tst_datenbank.cpp
#include <QtCore>
#include <QtTest>
#include "speicher/datenbank.h"

using AdlerMail::Speicher::Datenbank;

class TestDatenbank : public QObject {
    Q_OBJECT

privateSlots:
    void sollteOeffnenUndSchliessen() {
        Datenbank db;
        QVERIFY(db.oeffne(":memory:"));
        QVERIFY(db.istOffen());
        db.schliesse();
        QVERIFY(!db.istOffen());
    }

    void sollteFehlerBeiUngueltigemPfadMelden() {
        Datenbank db;
        QSignalSpy spion(&db, &Datenbank::fehlerAufgetreten);

        bool ok = db.oeffne("/ungueltiger/pfad/der/nicht/existiert/db.sqlite");
        QVERIFY(!ok);
        QCOMPARE(spion.count(), 1);
    }
};

QTEST_MAIN(TestDatenbank)
#include "tst_datenbank.moc"
```

**Bauen + Test:**

```bash
cmake --build bau --parallel $(nproc)
ctest --test-dir bau --output-on-failure -R datenbank
```

Erwartet: FAIL — `fehlerAufgetreten` ist kein Signal (MOC-Fehler wegen `signale:` alias).

**Schritt: Datenbank-Header fixen**

Falls Build-Fehler wegen `signale:` → `qt_alias.h` ist bereits eingebunden, sollte klappen.
Falls nicht: prüfen ob `#include "../kern/qt_alias.h"` in `datenbank.h` existiert.

### Aufgabe 2: Implementierung — öffnen/schließen vervollständigen

**Ziel:** Der Stub-Code in `datenbank.cpp` ist bereits korrekt. Prüfen ob die Implementierung den Test besteht.

**Dateien:**
- Prüfen: `quelltext/bibliothek/speicher/datenbank.cpp`
- Prüfen: `quelltext/bibliothek/speicher/datenbank.h`

Aktueller Stand (`datenbank.cpp`):
```cpp
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
```

⚠ **Problem:** `QSqlDatabase::addDatabase("QSQLITE")` erzeugt eine Default-Connection.
Bei mehreren `Datenbank`-Instanzen (oder Test-Wiederholungen) gibt das einen Fehler.
**Fix nötig:** Eindeutigen Connection-Namen pro Instanz verwenden.

**Neue Implementierung:**

```cpp
// datenbank.h — neue Member-Variable
private:
    QString m_verbindungsName;   // eindeutiger Name für QSqlDatabase

// datenbank.cpp
Datenbank::Datenbank(QObject *eltern) : QObject(eltern) {
    // Eindeutigen Namen pro Instanz (Adresse als String)
    m_verbindungsName = QString("adlermail_%1").arg(
        reinterpret_cast<quintptr>(this), 0, 16);
}

bool Datenbank::oeffne(const QString &pfad) {
    m_db = QSqlDatabase::addDatabase("QSQLITE", m_verbindungsName);
    m_db.setDatabaseName(pfad);
    if (!m_db.open()) {
        m_db = QSqlDatabase();  // zurücksetzen
        emit fehlerAufgetreten(m_db.lastError().text());
        return false;
    }
    erzeugeTabellen();
    return true;
}

void Datenbank::schliesse() {
    if (m_db.isOpen()) {
        m_db.close();
    }
    if (QSqlDatabase::contains(m_verbindungsName)) {
        QSqlDatabase::removeDatabase(m_verbindungsName);
    }
}

bool Datenbank::istOffen() const {
    return m_db.isOpen();
}
```

**Header-Ergänzung:**
```cpp
// datenbank.h — in private: hinzufügen
    QString m_verbindungsName;
```

**Test ausführen:**

```bash
cmake --build bau --parallel $(nproc)
ctest --test-dir bau --output-on-failure -R datenbank
```

Erwartet: 2 Tests PASS.

**Commit:**

```bash
git add pruefungen/tst_datenbank.cpp quelltext/bibliothek/speicher/datenbank.h quelltext/bibliothek/speicher/datenbank.cpp
git commit -m "speicher: Datenbank mit eindeutigen Verbindungsnamen + Tests"
```

---

## Phase 2: Konten speichern und lesen

### Aufgabe 3: Test — Konto speichern

**Ziel:** Ein Konto lässt sich in der DB speichern.

**Dateien:**
- Ändern: `pruefungen/tst_datenbank.cpp` (Tests hinzufügen)
- Ändern: `quelltext/bibliothek/speicher/datenbank.h`
- Ändern: `quelltext/bibliothek/speicher/datenbank.cpp`

**Test-Code (zu tst_datenbank.cpp hinzufügen):**

```cpp
#include "kern/konto.h"
using AdlerMail::Kern::Konto;

// In class TestDatenbank, privateSlots:

    void initTestCase() {
        m_db = new Datenbank(this);
        QVERIFY(m_db->oeffne(":memory:"));
    }

    void cleanupTestCase() {
        m_db->schliesse();
    }

    void sollteKontoSpeichern() {
        Konto konto;
        konto.email = "max@beispiel.de";
        konto.name = "Max Mustermann";
        konto.imapServer = "imap.beispiel.de";
        konto.imapPort = 993;
        konto.smtpServer = "smtp.beispiel.de";
        konto.smtpPort = 587;
        konto.benutzer = "max@beispiel.de";
        konto.passwort = "geheim123";
        konto.istAktiv = true;

        qint64 id = m_db->kontoSpeichern(konto);
        QVERIFY(id > 0);
        QCOMPARE(id, 1);  // erstes Konto = ID 1
    }

    void sollteKontoLesen() {
        // Konto aus Aufgabe 3 ist noch in der DB
        auto konten = m_db->alleKonten();
        QCOMPARE(konten.size(), 1);
        QCOMPARE(konten[0].email, "max@beispiel.de");
        QCOMPARE(konten[0].imapPort, 993);
        QVERIFY(konten[0].istAktiv);
    }

    void sollteKontoLoeschen() {
        auto konten = m_db->alleKonten();
        QCOMPARE(konten.size(), 1);
        qint64 id = konten[0].id;

        QVERIFY(m_db->kontoLoeschen(id));
        QCOMPARE(m_db->alleKonten().size(), 0);
    }

    void sollteDoppelteEmailAblehnen() {
        Konto konto;
        konto.email = "eindeutig@beispiel.de";
        konto.imapServer = "imap.test.de";
        konto.smtpServer = "smtp.test.de";
        konto.benutzer = "eindeutig@beispiel.de";

        qint64 id1 = m_db->kontoSpeichern(konto);
        QVERIFY(id1 > 0);

        // Zweites Konto mit gleicher Email
        konto.imapServer = "anderer.de";
        qint64 id2 = m_db->kontoSpeichern(konto);
        QCOMPARE(id2, -1);  // Fehler: doppelte Email
    }

private:
    Datenbank *m_db = nullptr;
```

**Test ausführen (nur bauen, Tests FAILEN weil Methoden fehlen):**

```bash
cmake --build bau --parallel $(nproc) 2>&1 | grep -E 'error|FAILED'
```

Erwartet: Compiler-Fehler — `kontoSpeichern`, `alleKonten`, `kontoLoeschen` nicht deklariert.

### Aufgabe 4: Implementierung — Konto-Methoden

**Ziel:** `kontoSpeichern()`, `alleKonten()`, `kontoLoeschen()` implementieren.

**Dateien:**
- Ändern: `quelltext/bibliothek/speicher/datenbank.h`
- Ändern: `quelltext/bibliothek/speicher/datenbank.cpp`

**Header-Ergänzungen (datenbank.h):**

```cpp
#include "../kern/konto.h"
#include <QtCore/QVector>

// In public: nach istOffen()
    qint64 kontoSpeichern(const Kern::Konto &konto);
    QVector<Kern::Konto> alleKonten() const;
    bool kontoLoeschen(qint64 id);
```

**Implementierung (datenbank.cpp):**

```cpp
#include "kern/konto.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

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
            return -1;  // doppelte Email
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
```

**Test ausführen:**

```bash
cmake --build bau --parallel $(nproc)
ctest --test-dir bau --output-on-failure -R datenbank -V
```

Erwartet: 5 Tests PASS (2 aus Phase 1 + 3 neu).

**Commit:**

```bash
git add pruefungen/tst_datenbank.cpp quelltext/bibliothek/speicher/datenbank.h quelltext/bibliothek/speicher/datenbank.cpp
git commit -m "speicher: Konto-Methoden (speichern, lesen, löschen) + Tests"
```

---

## Phase 3: Valgrind-Prüfung + Abschluss

### Aufgabe 5: Sauberkeit prüfen

**Ziel:** Keine Speicherfehler, keine Lecks.

```bash
./pruefen.sh sicher
```

Erwartet: Alle Tests PASS, Valgrind sauber (ggf. Qt-Fehlalarme durch `.valgrind.supp`).

### Aufgabe 6: AGENTS.md aktualisieren

**Ziel:** Stand dokumentieren.

In `AGENTS.md`, Abschnitt "Aktueller Stand" ergänzen:
```markdown
✅ Datenbank-Klasse vollständig implementiert (speichern, lesen, löschen).
✅ 5 Tests, alle grün, Valgrind sauber.
```

**Commit:**

```bash
git add AGENTS.md
git commit -m "docs: AGENTS.md — Datenbank-Status aktualisiert"
```

---

## Zusammenfassung

| Phase | Tasks | Ergebnis |
|---|---|---|
| 0 | Build aktivieren | `ctest` läuft |
| 1 | Öffnen/Schließen | 2 Tests, In-Memory-DB |
| 2 | Konto-CRUD | 5 Tests, speichern/lesen/löschen |
| 3 | Sauberkeit | Valgrind grün |

**Nach diesem Plan:** Die Datenbank-Schicht ist fertig. Nächster Plan: `Zwischenspeicher` + `PostfachDienst`.
