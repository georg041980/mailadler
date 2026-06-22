# AGENTS.md — Kontext für Hermes und andere KI-Helfer

> Diese Datei wird automatisch geladen, wenn im Projektverzeichnis gearbeitet wird.
> Sie gibt jedem Agenten sofort den vollen Projektkontext.

## Projekt

**AdlerMail** — Plattformübergreifender E-Mail-Client mit Qt 6.7 und QML.
Erste Version: IMAP/SMTP, SQLite-Backend, 3 Sprachen (DE/EN/ES).

## Technologie

- **Sprache:** C++17, alles auf Deutsch (Klassen, Methoden, Variablen, Kommentare)
- **Build:** CMake + Ninja (`cmake -B bau -GNinja && cmake --build bau`)
- **UI:** QML + C++-ViewModels (keine Widgets)
- **Datenbank:** QtSql + SQLite (lokal, eingebettet)
- **Protokoll:** VMime (IMAP/SMTP) oder QtNetwork-Eigenbau — noch nicht festgelegt
- **Tests:** Qt Test (`QTest`), CTest
- **Paketierung:** Flatpak (Linux), NSIS (Windows)

## Architektur (Schichten)

```
quelltext/
├── bibliothek/          STATIC — Geschäftslogik, keine GUI-Abhängigkeit
│   ├── kern/            Datenmodelle: Konto, Postfach, Nachricht
│   ├── protokoll/       IMAP-Verbindung, SMTP-Verbindung (asynchron)
│   ├── speicher/        SQLite-Datenbank, Zwischenspeicher
│   └── dienst/          High-Level-Dienste fürs UI
├── oberflaeche/         QML + C++-AnsichtModelle (ViewModels)
└── anwendung/           main.cpp (nur Start-Logik)
```

**Regel:** `bibliothek/` hat KEINE Qt-Widgets- oder QML-Abhängigkeit.
Sie kann komplett ohne GUI getestet werden.
`oberflaeche/` macht nur Darstellung, keine Geschäftslogik.

## Konventionen (verbindlich)

Siehe `KONVENTIONEN.md` für die vollständigen Regeln.
Die wichtigsten:

| Regel | Beispiel |
|---|---|
| `m_`-Präfix für Member | `m_verbindung`, `m_port` |
| Setter: `setze` + Name | `setzeServer()`, `setzePort()` |
| Getter (bool): `ist`/`hat` | `istVerbunden()`, `hatNachrichten()` |
| Signale: Partizip Perfekt | `verbunden()`, `getrennt()` |
| Slots: `bei` + Ereignis | `beiVerbunden()`, `beiSendenGeklickt()` |
| Fehler: EIN Signal pro Klasse | `fehlerAufgetreten(QString)` |
| Namespaces: `AdlerMail::Kern`, `::Protokoll`, ... | |

**Keine Exceptions** im Qt-Code — alles über Signale.

## Entwicklungsablauf

```bash
# Tägliche Arbeit
./pruefen.sh              # Formatierung + Sanitizer-Bau + Tests

# Vor einem Merge
./pruefen.sh sicher       # Zusätzlich Valgrind-Tiefenprüfung

# Schnelle Iteration (keine Sanitizer)
./pruefen.sh schnell      # Bau in <1s

# Datei-Watcher
./beobachten.sh           # Speichern → automatisch bauen+testen
```

**Vor jedem Commit** läuft automatisch der Pre-Commit-Hook:
`clang-format`-Check → Bau mit Sanitizern → Tests.
Fehler brechen den Commit ab.

## Sicherheit (Sanitizer)

Im Standard-Build AKTIV:
- **AddressSanitizer:** Pufferüberlauf, Use-after-free, Stack-Overflow
- **UndefinedBehaviorSanitizer:** Null-Pointer, Integer-Überlauf
- **LeakSanitizer:** Speicherlecks

Fehlermeldungen zeigen exakte Datei und Zeile.

## Aktueller Stand

Projektgerüst steht. Alle CMake-Dateien und Konventionen sind definiert.
Die `.h`/`.cpp`-Stubs für `kern/`, `protokoll/`, `speicher/`, `dienst/`
müssen noch erstellt werden, damit das Projekt das erste Mal durchbaut.

## Was als Nächstes

1. Stub-Header für alle Module → erster erfolgreicher Build
2. Datenbank-Klasse (`speicher/datenbank.h/.cpp`) mit Tests
3. IMAP-Verbindung (MVP: Verbinden + Ordnerliste abrufen)
4. Erstes QML-Fenster mit Nachrichtenliste
5. …

## Verzeichnisse, die Hermes kennen muss

- `quelltext/bibliothek/` — die Geschäftslogik (hier passiert die Arbeit)
- `pruefungen/` — alle Tests
- `vorlagen/` — Templates für neue Header und Tests
- `KONVENTIONEN.md` — das vollständige Regelwerk
- `.clang-format` — nie ändern, wird von der CI erzwungen
