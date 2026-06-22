# AdlerMail

Plattformübergreifender E-Mail-Client mit Qt6 und QML.

## Plattformen

- **Linux** (Flatpak)
- **Windows** (MSI / Portable)
- später: macOS, Android, iOS

## Sprachen

1. Deutsch
2. Englisch
3. Spanisch
→ Weitere Sprachen über Qt Linguist einfach ergänzbar.

## Entwickeln (lokal, schnell)

```bash
# Einmaliger Bau + Test
./pruefen.sh

# Nur Tests wiederholen (nach Code-Änderung)
./pruefen.sh test

# Datei-Watcher: speichern → automatisch bauen+testen
./beobachten.sh          # Strg+C zum Beenden
```

Kein Push, kein Warten auf CI — alles läuft lokal in <5 Sekunden.

### Git-Hook (automatisch bei jedem Commit)

```bash
cp vorlagen/pre-commit.hook .git/hooks/pre-commit
chmod +x .git/hooks/pre-commit
```

Danach läuft `clang-format` + Bau + Tests **automatisch vor jedem Commit**.
Formatierungsfehler brechen den Commit ab.

## Bauen (manuell)

```bash
cmake -B bau -DCMAKE_BUILD_TYPE=Debug
cmake --build bau
./bau/quelltext/anwendung/adlermail
```

## Konventionen

Siehe [KONVENTIONEN.md](KONVENTIONEN.md).

Alle Quelldateien sind deutsch (Klassen, Methoden, Variablen, Kommentare).
C++/Qt-Schlüsselwörter bleiben englisch.
