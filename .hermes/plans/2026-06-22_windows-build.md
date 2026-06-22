# Windows-Build — Implementierungsplan

> **Für Hermes:** Dieser Plan ist nur ausführbar auf einem Windows-Rechner mit MSVC oder MinGW.

**Ziel:** AdlerMail kompiliert und läuft unter Windows 10/11.

**Vorbedingung:** Alle vorherigen Pläne abgeschlossen. Flatpak-Paket existiert.

---

## Phase 1: CMake-Anpassungen für Windows

### Aufgabe 1: Plattform-Prüfungen in CMake

**Ziel:** CMake erkennt Windows und setzt entsprechende Flags.

**Dateien:**
- Erstellen: `cmake/Windows.cmake`
- Ändern: `CMakeLists.txt`

**cmake/Windows.cmake:**
```cmake
if(WIN32)
    # Windows-spezifische Einstellungen
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

    # Qt6 unter Windows findet oft mit vcpkg
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "vcpkg toolchain")
    endif()

    # Kein Unix-spezifischer Sanitizer unter MSVC
    if(MSVC)
        set(SICHERER_BAU OFF)
    endif()
endif()
```

**In CMakeLists.txt einbinden:**
```cmake
include(cmake/Windows.cmake)
```

**Commit:**

```bash
git add cmake/Windows.cmake CMakeLists.txt
git commit -m "build: Windows-Plattform-Erkennung in CMake"
```

---

## Phase 2: Windows-Installer (NSIS)

### Aufgabe 2: NSIS-Installer-Skript

**Ziel:** `cpack` erzeugt einen Windows-Installer.

**Dateien:**
- Erstellen: `verpackung/windows/installer.nsi`
- Ändern: `CMakeLists.txt` (CPack-Konfiguration)

**CPack in CMakeLists.txt:**
```cmake
if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "AdlerMail")
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    set(CPACK_PACKAGE_NAME "AdlerMail")
    set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
    include(CPack)
endif()
```

**Bauen (auf Windows):**

```powershell
cmake -B build -DCMAKE_BUILD_TYPE=Release -GNinja
cmake --build build --parallel
cpack -B build
```

**Commit:**

```bash
git add CMakeLists.txt
git commit -m "verpackung: CPack/NSIS Windows-Installer-Konfiguration"
```

---

## Phase 3: CI für Windows

### Aufgabe 3: GitHub Actions Windows-Job

**Ziel:** CI baut auch unter Windows.

**Dateien:**
- Bearbeiten: `.github/workflows/bauen-und-pruefen.yml`

**Neuer Job:**
```yaml
  windows-bauen:
    runs-on: windows-2022
    steps:
      - uses: actions/checkout@v4
      - uses: ilammy/msvc-dev-cmd@v1
      - name: Qt6 installieren
        run: |
          pip install aqtinstall
          aqt install-qt windows desktop 6.4.2 win64_msvc2019_64
      - name: CMake konfigurieren
        run: cmake -B bau -DCMAKE_PREFIX_PATH=${{ github.workspace }}/Qt/6.4.2/msvc2019_64
      - name: Bauen
        run: cmake --build bau --config Release
```

**Commit:**

```bash
git add .github/workflows/bauen-und-pruefen.yml
git commit -m "ci: Windows-Build-Job in GitHub Actions"
```

---

## Zusammenfassung

| Phase | Ergebnis |
|---|---|
| 1 | CMake läuft unter Windows |
| 2 | NSIS-Installer via CPack |
| 3 | CI baut automatisch unter Windows |

**Nach diesem Plan:** AdlerMail ist auf Windows baubar und verteilbar. macOS/iOS/Android folgen später.
