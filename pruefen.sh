#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# prüfen.sh — Lokale Schnellprüfung vor jedem Commit
# ---------------------------------------------------------------------------
# Nutzung:
#   ./pruefen.sh                  Standard: Formatierung + Bau (mit Sanitizern) + Tests
#   ./pruefen.sh sicher           Standard + Valgrind (langsam, aber gründlich)
#   ./pruefen.sh schnell          Ohne Sanitizer — schneller Bau für Iteration
#   ./pruefen.sh formatierung     Nur formatieren
#   ./pruefen.sh beheben          clang-format auf alles anwenden
#   ./pruefen.sh bau              Nur bauen
#   ./pruefen.sh test             Nur Tests
#   ./pruefen.sh alles            VON VORN: löschen + konfigurieren + bauen + testen
# ---------------------------------------------------------------------------
set -euo pipefail

PROJEKT="$(cd "$(dirname "$0")" && pwd)"
BAU="$PROJEKT/bau"

# --- Farben ---
if [ -t 1 ]; then
    ROT='\033[0;31m'; GRUEN='\033[0;32m'; GELB='\033[1;33m'; BLAU='\033[0;34m'; KEIN='\033[0m'
else
    ROT=''; GRUEN=''; GELB=''; BLAU=''; KEIN=''
fi

meldung() { echo -e "${BLAU}→${KEIN} $1"; }
erfolg()  { echo -e "${GRUEN}✓${KEIN} $1"; }
fehler()  { echo -e "${ROT}✗${KEIN} $1"; }
warnung() { echo -e "${GELB}⚠${KEIN} $1"; }

# ---------------------------------------------------------------------------
formatierung() {
    meldung "Formatierung prüfen (clang-format) …"

    local dateien
    dateien=$(find "$PROJEKT/quelltext" "$PROJEKT/pruefungen" -name '*.h' -o -name '*.cpp' 2>/dev/null)

    if [ -z "$dateien" ]; then
        warnung "Keine .h/.cpp-Dateien gefunden — überspringe"
        return 0
    fi

    if echo "$dateien" | xargs clang-format --dry-run --Werror 2>&1; then
        erfolg "Formatierung — bestanden"
    else
        fehler "Formatierung — FEHLER. So beheben:"
        echo "  ./pruefen.sh beheben"
        return 1
    fi
}

# ---------------------------------------------------------------------------
formatierung_beheben() {
    meldung "clang-format anwenden …"
    find "$PROJEKT/quelltext" "$PROJEKT/pruefungen" \
        -name '*.h' -o -name '*.cpp' 2>/dev/null \
        | xargs -r clang-format -i
    erfolg "Formatierung korrigiert"
}

# ---------------------------------------------------------------------------
konfigurieren() {
    local modus="${1:-sicher}"
    local extra_flags=""

    case "$modus" in
        sicher)
            extra_flags="-DSICHERER_BAU=ON"
            meldung "Konfigurieren (MIT Sanitizern: Pufferüberlauf, Use-after-free, Leaks) …"
            ;;
        schnell)
            extra_flags="-DSICHERER_BAU=OFF"
            meldung "Konfigurieren (OHNE Sanitizer — schnell) …"
            ;;
    esac

    cmake -B "$BAU" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DBUILD_TESTING=ON \
        -GNinja \
        $extra_flags \
        2>&1 | tail -5
}

# ---------------------------------------------------------------------------
bauen() {
    meldung "Bauen …"
    cmake --build "$BAU" --parallel "$(nproc)" 2>&1
    erfolg "Bau — erfolgreich"
}

# ---------------------------------------------------------------------------
testen() {
    meldung "Tests ausführen …"
    if ctest --test-dir "$BAU" --output-on-failure -j"$(nproc)" 2>&1; then
        erfolg "Tests — alle bestanden"
        "$PROJEKT/agent_update.sh" 2>/dev/null || true
    else
        fehler "Tests — FEHLGESCHLAGEN"
        return 1
    fi
}

# ---------------------------------------------------------------------------
valgrind_pruefen() {
    meldung "Valgrind (Speicherfehler-Tiefenprüfung) …"

    if ! command -v valgrind &>/dev/null; then
        warnung "Valgrind nicht installiert — überspringe"
        echo "  sudo apt install valgrind"
        return 0
    fi

    # Alle Test-Binaries finden
    local test_binaerien
    test_binaerien=$(find "$BAU/pruefungen" -type f -executable 2>/dev/null || true)

    if [ -z "$test_binaerien" ]; then
        warnung "Keine Test-Binaries gefunden"
        return 0
    fi

    for binaer in $test_binaerien; do
        meldung "  Valgrind: $(basename "$binaer")"
        if valgrind --leak-check=full --error-exitcode=1 -q "$binaer" 2>&1; then
            erfolg "  Valgrind — sauber"
        else
            fehler "  Valgrind — FEHLER in $(basename "$binaer")"
            return 1
        fi
    done
}

# ---------------------------------------------------------------------------
alles_frisch() {
    local modus="${1:-sicher}"
    meldung "Vollständiger Neubau …"
    rm -rf "$BAU"
    konfigurieren "$modus" && bauen && testen
}

# ---------------------------------------------------------------------------
# Hauptroutine
fall="${1:-}"

HAT_CLANG_FORMAT=true
if ! command -v clang-format &>/dev/null; then
    warnung "clang-format nicht gefunden — Formatierungsprüfung deaktiviert"
    HAT_CLANG_FORMAT=false
fi

case "$fall" in
    "")
        # Standard: formatierung + konfigurieren + bau + test (mit Sanitizern)
        if $HAT_CLANG_FORMAT; then formatierung || exit 1; fi
        konfigurieren sicher
        bauen && testen
        ;;
    sicher)
        # Mit Sanitizern + Valgrind
        if $HAT_CLANG_FORMAT; then formatierung || exit 1; fi
        konfigurieren sicher
        bauen && testen && valgrind_pruefen
        ;;
    schnell)
        # Ohne Sanitizer — schneller
        if $HAT_CLANG_FORMAT; then formatierung || exit 1; fi
        konfigurieren schnell
        bauen && testen
        ;;
    formatierung)   formatierung ;;
    beheben)        formatierung_beheben ;;
    bau)            bauen ;;
    test)           testen ;;
    alles)          alles_frisch sicher ;;
    *)
        echo "Nutzung: $0 [sicher|schnell|formatierung|beheben|bau|test|alles]"
        exit 1
        ;;
esac
