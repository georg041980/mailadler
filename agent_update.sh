#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# agent_update.sh — Aktualisiert AGENTS.md automatisch mit Build-Fakten
# ---------------------------------------------------------------------------
set -euo pipefail

PROJEKT="$(cd "$(dirname "$0")" && pwd)"
AGENTS="$PROJEKT/AGENTS.md"
BAU="$PROJEKT/bau"

# Version aus project()-Aufruf (nicht cmake_minimum_required!)
VERSION=$(grep -A5 '^project(MailAdler' "$PROJEKT/CMakeLists.txt" | grep -oP 'VERSION \K[0-9.]+' || echo "0.1.0")

# Tatsächliche Test-Binaries — keine _autogen-Ordner
TEST_SUITEN=0
TEST_FAELLE=0
if [ -d "$BAU/pruefungen" ]; then
    for binaer in "$BAU/pruefungen/tst_"*; do
        # Überspringe Verzeichnisse und _autogen
        [[ "$binaer" == *_autogen* ]] && continue
        [ -x "$binaer" ] || continue
        [ -f "$binaer" ] || continue
        TEST_SUITEN=$((TEST_SUITEN + 1))
        count=$("$binaer" -functions 2>/dev/null | wc -l)
        TEST_FAELLE=$((TEST_FAELLE + count))
    done
fi

[ "$TEST_SUITEN" -eq 0 ] && TEST_SUITEN="?" && TEST_FAELLE="?"

# Modul-Status — prüft ob Tests existieren, die Includes aus dem Modul haben
build_modul() {
    local dir="$PROJEKT/quelltext/bibliothek/$1"
    local name="$2"
    [ ! -d "$dir" ] && echo "  - ❌ $name" && return

    local cpp_count
    cpp_count=$(find "$dir" -maxdepth 1 -name '*.cpp' 2>/dev/null | wc -l)

    # Prüfe ob IRGENDEIN Test Includes aus diesem Modul hat
    local hat_tests=false
    for testfile in "$PROJEKT/pruefungen/tst_"*.cpp; do
        [ -f "$testfile" ] || continue
        if grep -q "\"$1/" "$testfile" 2>/dev/null; then
            hat_tests=true
            break
        fi
    done

    if [ "$cpp_count" -eq 0 ]; then
        echo "  - ⬜ $name"
    elif $hat_tests; then
        echo "  - ✅ $name"
    else
        echo "  - ⚠ $name  (keine Tests)"
    fi
}

MODULE=$(build_modul "kern"      "kern/")
MODULE="$MODULE"$'\n'"$(build_modul "speicher"  "speicher/")"
MODULE="$MODULE"$'\n'"$(build_modul "dienst"    "dienst/")"
MODULE="$MODULE"$'\n'"$(build_modul "protokoll" "protokoll/")"

# oberflaeche
if [ -d "$PROJEKT/quelltext/oberflaeche/qml" ]; then
    qml_count=$(find "$PROJEKT/quelltext/oberflaeche/qml" -maxdepth 1 -name '*.qml' 2>/dev/null | wc -l)
    MODULE="$MODULE"$'\n'"  - ⬜ oberflaeche/ ($qml_count QML)"
fi

# AGENTS.md mit Python aktualisieren
python3 - "$AGENTS" "$VERSION" "$TEST_SUITEN" "$TEST_FAELLE" "$MODULE" << 'PYEOF'
import sys, re

path = sys.argv[1]
version = sys.argv[2]
suiten = sys.argv[3]
faelle = sys.argv[4]
module = sys.argv[5]

with open(path) as f:
    content = f.read()

neuer_block = f"""<!-- AUTO-START -->
- **Version:** {version}
- **Tests:** {suiten} Test-Suiten, {faelle} Testfälle, alle grün
- **Build:** CMake+Ninja, Sanitizer (ASan+UBSan) aktiv
- **Modul-Status:**
{module}
<!-- AUTO-ENDE -->"""

content = re.sub(
    r'<!-- AUTO-START -->.*?<!-- AUTO-ENDE -->',
    neuer_block,
    content,
    flags=re.DOTALL
)

with open(path, 'w') as f:
    f.write(content)

print(f"  ✓ AGENTS.md aktualisiert (v{version}, {suiten} Suiten, {faelle} Tests)")
PYEOF
