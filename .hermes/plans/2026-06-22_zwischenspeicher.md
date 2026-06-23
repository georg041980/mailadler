# Zwischenspeicher — Implementierungsplan

> **Für Hermes:** Nutze den `test-driven-development`-Skill, um diesen Plan TDD-basiert umzusetzen.

**Ziel:** Die `Zwischenspeicher`-Klasse in `quelltext/bibliothek/speicher/zwischenspeicher.h/.cpp` vollständig implementieren und testen.

**Architektur:** Einfacher Wrapper um `QVector<Kern::Nachricht>`. Hält Nachrichten im RAM für schnellen UI-Zugriff. Keine Persistenz — die kommt von `Datenbank`.

**Tech-Stack:** Qt 6.4.2, QtCore, QTest, CTest.

**Vorbedingung:** Plan `datenbank-implementierung` abgeschlossen, `pruefungen/` im Build aktiv.

---

## Phase 1: Test-Datei + Build

### Aufgabe 1: Test-Datei erstellen und Build aktivieren

**Ziel:** `tst_zwischenspeicher.cpp` existiert und wird gebaut.

**Dateien:**
- Erstellen: `pruefungen/tst_zwischenspeicher.cpp`
- Ändern: `pruefungen/CMakeLists.txt` — `mailadler_pruefung_hinzufuegen(tst_zwischenspeicher)` entkommentieren

**Code (tst_zwischenspeicher.cpp):**

```cpp
#include <QtCore>
#include <QtTest>
#include "speicher/zwischenspeicher.h"
#include "kern/nachricht.h"

using MailAdler::Speicher::Zwischenspeicher;
using MailAdler::Kern::Nachricht;

class TestZwischenspeicher : public QObject {
    Q_OBJECT

privateSlots:
    void sollteLeerStarten() {
        Zwischenspeicher cache;
        QVERIFY(cache.alle().isEmpty());
    }

    void sollteNachrichtenSetzenUndLesen() {
        Zwischenspeicher cache;

        Nachricht n1;
        n1.id = 1;
        n1.betreff = "Hallo";
        n1.absender = "max@test.de";

        Nachricht n2;
        n2.id = 2;
        n2.betreff = "Re: Hallo";
        n2.absender = "lisa@test.de";

        cache.setze({n1, n2});

        auto alle = cache.alle();
        QCOMPARE(alle.size(), 2);
        QCOMPARE(alle[0].betreff, "Hallo");
        QCOMPARE(alle[1].absender, "lisa@test.de");
    }

    void sollteLeerenFunktionieren() {
        Zwischenspeicher cache;
        cache.setze({Nachricht()});
        QCOMPARE(cache.alle().size(), 1);

        cache.leeren();
        QVERIFY(cache.alle().isEmpty());
    }

    void sollteUeberschreiben() {
        Zwischenspeicher cache;

        Nachricht alt; alt.betreff = "Alt";
        cache.setze({alt});
        QCOMPARE(cache.alle()[0].betreff, "Alt");

        Nachricht neu; neu.betreff = "Neu";
        cache.setze({neu});
        QCOMPARE(cache.alle()[0].betreff, "Neu");
        QCOMPARE(cache.alle().size(), 1);
    }
};

QTEST_MAIN(TestZwischenspeicher)
#include "tst_zwischenspeicher.moc"
```

**Build + Test:**

```bash
cmake -B bau -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DSICHERER_BAU=ON -GNinja
cmake --build bau --parallel $(nproc)
ctest --test-dir bau --output-on-failure -R zwischenspeicher -V
```

Erwartet: 4 Tests PASS (Stub-Implementierung reicht schon).

**Commit:**

```bash
git add pruefungen/tst_zwischenspeicher.cpp pruefungen/CMakeLists.txt
git commit -m "speicher: Zwischenspeicher-Tests (4 Testfälle)"
```

---

## Zusammenfassung

| Phase | Tasks | Ergebnis |
|---|---|---|
| 1 | Test + Build | 4 Tests, Stub besteht sofort |

**Warum so kurz?** Die Stub-Implementierung von `Zwischenspeicher` ist bereits vollständig — `setze()`, `alle()`, `leeren()` tun genau das, was sie sollen. Die Tests bestätigen nur, dass nichts kaputtgeht, wenn später das `Nachricht`-Struct erweitert wird.

**Dauer:** <10 Minuten.
