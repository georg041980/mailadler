#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# beobachten.sh — Datei-Watcher: speichern → automatisch bauen+testen
# ---------------------------------------------------------------------------
# Nutzung: ./beobachten.sh
# Beendet mit Strg+C
# ---------------------------------------------------------------------------
# Benötigt: entr  (sudo apt install entr)
# ---------------------------------------------------------------------------

PROJEKT="$(cd "$(dirname "$0")" && pwd)"

if ! command -v entr &>/dev/null; then
    echo "entr ist nicht installiert."
    echo "  sudo apt install entr"
    exit 1
fi

echo "Beobachte $PROJEKT …  (Strg+C zum Beenden)"
echo ""

find "$PROJEKT/quelltext" "$PROJEKT/pruefungen" \
    -name '*.cpp' -o -name '*.h' -o -name '*.qml' -o -name 'CMakeLists.txt' \
    2>/dev/null \
| entr -c "$PROJEKT/pruefen.sh"
