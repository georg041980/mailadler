# AGENTS.md — Kontext für KI-Helfer

> Diese Datei wird automatisch geladen, wenn im Projektverzeichnis gearbeitet wird.

**Projekt:** MailAdler — plattformübergreifender E-Mail-Client mit C++17, Qt 6 und QML.

**Repository:** https://github.com/georg041980/mailadler

**Alle Details** siehe `README.md`. Diese Datei fasst das Wichtigste kompakt zusammen.

## Technologie

- C++17, CMake + Ninja, Qt 6, QML
- SQLite über QtSql
- IMAP/SMTP
- Qt Test + CTest
- Sanitizer aktiv im Standard-Build

## Sprache

- Quellcode, Kommentare, Konventionen und Commit-Messages auf Deutsch
- Qt-/C++-Schlüsselwörter bleiben englisch: `signals`, `slots`, `override`, `nullptr`

## Architektur

```
quelltext/
├── bibliothek/     # Geschäftslogik, keine GUI-Abhängigkeit
│   ├── kern/
│   ├── protokoll/
│   ├── speicher/
│   ├── dienst/
│   └── erweiterung/
├── oberflaeche/    # QML + ViewModels
└── anwendung/      # main.cpp
```

## Wichtigste Regeln

| Regel | Beispiel |
|---|---|
| Member | `m_verbindung` |
| Setter | `setzeServer()` |
| Getter bool | `istVerbunden()` |
| Signale | `verbunden()` |
| Slots | `beiSendenGeklickt()` |
| Fehler | `fehlerAufgetreten(QString)` |

- Keine Exceptions im Qt-Code
- Fehler über Signale
- Module dürfen nicht in `kern/`, `protokoll/`, `speicher/` oder `dienst/` eingreifen
- Module werden per CMake-Option ein- und ausgeschaltet

## Fehler- und Feature-Meldung

- Meldungen laufen über einen geleiteten Dialog in der App, nicht über eine bloße Suchleiste.
- Nutzer beschreiben ihr Ziel in einfachen Worten; die App findet semantisch passende Einträge.
- Jeder Eintrag hat mehrere Erklärungsebenen (einfach + fachlich) und eine feste Nummer.
- Doppelte Meldungen werden vermieden, indem der Nutzer „Ja, das meinte ich" bestätigen muss.
- Bei Zustimmung wird seine Wortwahl als Alias gespeichert.
- Erst danach darf ein neuer Eintrag mit eindeutiger Nummer angelegt werden.

## Bauen und Prüfen

```bash
./pruefen.sh         # Formatierung + Sanitizer + Tests
./pruefen.sh schnell # Schnell ohne Sanitizer
./pruefen.sh test    # Nur Tests
./pruefen.sh alles   # Neu bauen und testen
./beobachten.sh      # Datei-Watcher
```

## Versionierung

- Semantic Versioning: `MAJOR.MINOR.PATCH`
- Start: `v0.0.1`
- Tags auf dem Hauptzweig: `git tag -a v0.1.0`
- Releases und Milestones auf GitHub

## Was ist wo

- `README.md` — Vision, Architektur, Abläufe, Mitwirken
- `KONVENTIONEN.md` — Vollständige Programmierregeln
- `.clang-format` — Formatierung, nicht ändern
