# AdlerMail — Soul & Produktvision

> **Ein E-Mail-Client, der auf Deutsch gebaut wird, ab Tag eins auf Deutsch, Englisch und Spanisch spricht — und mitwächst, ohne zu wuchern.**

## Planer-Sicht

AdlerMail ist ein plattformübergreifender Desktop-E-Mail-Client. Technisches Fundament ist **C++17 mit Qt 6 und QML**. Die Entwicklungssprache des gesamten Codes, der Kommentare, der Konventionen und der Commit-Messages ist **Deutsch**. Nur Qt-/C++-Schlüsselwörter wie `signals`, `slots`, `override` oder `nullptr` bleiben aus technischen Gründen englisch.

## Zentrale Prinzipien

### 1. Mehrsprachig von Anfang an

AdlerMail spricht bei Release Deutsch, Englisch und Spanisch. Die technische Basis ist das Qt-Übersetzungssystem.

**Feste Struktur für Übersetzungen:**

- Jeder übersetzbare Text hat einen festen, eindeutigen Schlüssel.
- Im QML-Code wird `qsTr("Schlüssel")` verwendet, im C++-Code `QCoreApplication::translate("Kontext", "Schlüssel")`.
- Die Übersetzungsdateien (`.ts`) enthalten zu jedem Schlüssel die Übersetzungen für DE, EN und ES.
- Der deutsche Text ist gleichzeitig Schlüssel und Fallback. Wenn eine Übersetzung fehlt, erscheint automatisch der deutsche Originaltext.

**Beispiel:**

```qml
// QML
Button {
    text: qsTr("Konto_anlegen")
}
```

```xml
<!-- uebersetzungen/adlermail_en.ts -->
<message>
    <source>Konto_anlegen</source>
    <translation>Create account</translation>
</message>
```

```xml
<!-- uebersetzungen/adlermail_es.ts -->
<message>
    <source>Konto_anlegen</source>
    <translation>Crear cuenta</translation>
</message>
```

**Einfache Korrektur:**

Wenn ein einzelnes Wort oder eine Übersetzung falsch ist, wird nur die betroffene `.ts`-Datei geändert. Der Quelltext bleibt unverändert.

**Platzbedarf planen:**

- Übersetzungen dürfen kürzer sein, sollten aber nicht wesentlich länger als der deutsche Schlüssel sein.
- UI-Komponenten sind so zu gestalten, dass längere Übersetzungen nicht das Layout zerstören.
- Längere Texte werden in der Regel mehrzeilig oder mit automatischem Zeilenumbruch dargestellt.

**Organisation:**

- Übersetzungsdateien unter `uebersetzungen/`
- CI-Check prüft, ob alle `.ts`-Dateien vollständig sind
- Weitere Sprachen lassen sich später ergänzen, ohne bestehenden Code anzufassen

### 2. Modular statt monolithisch

Die App besteht aus klar getrennten Schichten:

```
quelltext/
├── bibliothek/          # Geschäftslogik, keine GUI-Abhängigkeit
│   ├── kern/            # Datenmodelle: Konto, Postfach, Nachricht
│   ├── protokoll/       # IMAP/SMTP-Verbindungen
│   ├── speicher/        # SQLite-Datenbank, Zwischenspeicher
│   ├── dienst/          # High-Level-Dienste fürs UI
│   └── erweiterung/     # Schnittstelle für Plugins und Zusatzmodule
├── oberflaeche/         # QML + C++-ViewModels
└── anwendung/           # main.cpp, Startlogik
```

`bibliothek/` hat keine QML- oder Widgets-Abhängigkeit und ist vollständig ohne GUI testbar. Die Oberfläche macht nur Darstellung, niemals Geschäftslogik.

### 3. Erweiterbar ohne Umbau

Neue Funktionen — zum Beispiel Kalender, Adressbuch, Verschlüsselung oder Filterregeln — werden als Module oder Plugins implementiert. Jedes Modul meldet sich über eine definierte Schnittstelle an, bringt eigene UI-Komponenten, Speicherstrukturen und Dienste mit. Die Kernanwendung bietet nur den Rahmen; sie weiß nichts über konkrete Module.

### 4. Löschen statt Ausblenden

Funktionen, die der Nutzer nicht braucht, sollen nicht nur versteckt, sondern physisch entfernt werden können:

- Modulordner löschen → CMake-Option abschalten → neu bauen
- Keine toten Codepfade
- Keine technische Schuld durch „mal schnell ausgeblendet“
- Die App lässt sich auf das wirklich Wesentliche reduzieren

Dieser Grundsatz ist entscheidend für die langfristige Wartbarkeit: AdlerMail wächst nur dort, wo es wirklich nötig ist. Unbenutzte Features werden nicht zur Belastung, sondern entfernt. So bleibt die Codebasis überschaubar und verständlich.

### 5. Variable Funktionsvielfalt je nach Anforderung

Jede Organisation und jeder Nutzer hat andere Anforderungen. Deshalb unterstützt AdlerMail einen variablen Funktionsumfang:

- **Erweitern:** Neue Module fügen Features hinzu (z. B. Kalender, Adressbuch, S/MIME, Filterregeln, Chat-Protokolle)
- **Reduzieren:** Nicht benötigte Module werden entfernt, ohne dass die Kernanwendung instabil wird
- **Anpassen:** Ein klarer Modulvertrag erlaubt es, Module auszutauschen oder Varianten gleichzeitig anzubieten

Die Architektur stellt sicher, dass Erweiterungen und Reduktionen sauber möglich sind. Ein Modul darf nicht aus Versehen fest in der Kernlogik verankert sein. Stattdessen kommuniziert es über definierte Schnittstellen mit dem Rest der Anwendung. So bleibt der Kern klein und leicht wartbar.

### 6. Deutsche Entwicklungssprache

- Klassen, Methoden, Variablen, Kommentare, Dateinamen: deutsch
- Beispiele: `m_verbindung`, `setzeServer()`, `istVerbunden()`, `beiSendenGeklickt()`
- Commit-Messages auf Deutsch, imperativ
- Ziel: Lesbarkeit für das deutschsprachige Team auf Dauer

### 7. Sicherheit und Stabilität

- Bauprozess mit AddressSanitizer, UndefinedBehaviorSanitizer und LeakSanitizer
- Automatisierte Tests mit Qt Test und CTest
- `clang-format` als verbindliche Formatierung
- Keine Exceptions im Qt-Code — Fehler werden über Signale gemeldet

### 8. Barrierefreiheit

AdlerMail soll für möglichst viele Nutzer bedienbar sein:

- Tastaturbedienung für alle wichtigen Aktionen
- Screenreader-Unterstützung durch semantische QML-Elemente und `Accessible`
- Ausreichende Kontraste und skalierbare Schriftgrößen
- Klare visuelle Rückmeldung bei Fehlern und Ladezuständen

### 9. Performance und Datensparsamkeit

- App startet auch mit großen Postfächern (10.000+ Nachrichten) flüssig
- SQLite-Indizes, paginierte Lade- und Synchronisationsvorgänge
- Speicherbegrenzung für Caches; keine unbegrenzte Anhäufung alter Daten
- Hintergrundabgleich wird begrenzt und konfigurierbar

### 10. Datenschutz und Sicherheit

- Eingegebene Passwörter und Tokens werden nicht geloggt
- Lokale Speicherung nur verschlüsselt, sobald ein Verschlüsselungsmodul aktiv ist
- Keine unsichere Telemetrie; nur optionale, anonymisierte Fehlerberichte
- TLS/STARTTLS als Standard für alle Verbindungen
- Zertifikatsprüfung aktiv, mit klarer Fehlermeldung bei Problemen

### 11. Offline-Fähigkeit

- Nachrichten und Metadaten werden lokal zwischengespeichert
- Lesen, Suchen und Entwürfe funktionieren ohne Netzwerk
- Ausgeführte Aktionen werden synchronisiert, sobald Verbindung besteht
- Statusanzeige zeigt Offline-Modus und ausstehende Änderungen klar an

## Langfristige Vision

AdlerMail soll der E-Mail-Client werden, den man sich selbst zusammenstellen kann: eine schlanke Basis, eine klare Architektur, mehrsprachig von Haus aus — mit wachsenden Funktionsbausteinen, aber ohne Kompromisse bei Sicherheit, Stabilität und Übersichtlichkeit. Der Funktionsumfang darf sich je nach Anforderung erweitern oder reduzieren lassen. Nur so bleibt AdlerMail auf Dauer wartbar und verständlich.

## So arbeiten wir

### Projekt erstellen

1. **Repository aufsetzen** mit der vorgegebenen Verzeichnisstruktur
2. **CMake einrichten** mit modularen Targets:
   - `adlermail_bibliothek` (statisch)
   - `adlermail_oberflaeche` (QML-Modul)
   - `adlermail_anwendung` (ausführbare Datei)
3. **CMake-Optionen definieren**, mit denen sich Module ein- oder ausschalten lassen:
   ```cmake
   option(ADLERMAIL_KALENDER "Kalendermodul aktivieren" OFF)
   option(ADLERMAIL_ADRESSBUCH "Adressbuchmodul aktivieren" OFF)
   option(ADLERMAIL_VERSCHLUESSELUNG "Verschlüsselungsmodul aktivieren" OFF)
   ```
4. **Base-Module implementieren:**
   - `kern/` — Konto, Postfach, Nachricht
   - `protokoll/` — IMAP/SMTP
   - `speicher/` — SQLite
   - `dienst/` — Kopplung von UI und Protokoll
   - `erweiterung/` — Modulschnittstelle
5. **Sprachsystem einrichten** mit `.ts`-Dateien für DE/EN/ES
6. **Prüfscript `pruefen.sh`** bereitstellen: Formatierung, Sanitizer, Tests

### Funktion hinzufügen

1. **Modul-Name festlegen**, z. B. `kalender`
2. **Neues Verzeichnis** anlegen:
   ```
   quelltext/bibliothek/erweiterung/kalender/
   quelltext/oberflaeche/erweiterung/kalender/
   pruefungen/erweiterung/kalender/
   ```
3. **Modulvertrag implementieren:**
   - Von einer gemeinsamen Schnittstelle ableiten, z. B. `AdlerMail::Erweiterung::Modul`
   - Eindeutige Kennung, Name, Version, Abhängigkeiten angeben
   - Einstiegspunkte für Speicher, UI und Dienste registrieren
4. **CMake-Option auf `ON` setzen** und Modul in den Build einbinden:
   ```cmake
   if(ADLERMAIL_KALENDER)
       add_subdirectory(quelltext/bibliothek/erweiterung/kalender)
   endif()
   ```
5. **Feature implementieren**, getrennt nach:
   - Geschäftslogik in `bibliothek/`
   - ViewModels in `oberflaeche/`
   - QML-Oberfläche in `oberflaeche/`
6. **Tests schreiben** in `pruefungen/erweiterung/kalender/`
7. **Übersetzungen ergänzen** für DE/EN/ES
8. **`./pruefen.sh`** laufen lassen
9. **Dokumentation aktualisieren** in `SOUL.md`, `AGENTS.md` und `README.md`

### Funktion entfernen

1. **CMake-Option auf `OFF` setzen**
2. **Modulverzeichnis entfernen**:
   ```bash
   rm -rf quelltext/bibliothek/erweiterung/kalender
   rm -rf quelltext/oberflaeche/erweiterung/kalender
   rm -rf pruefungen/erweiterung/kalender
   ```
3. **Abhängigkeiten prüfen:** Kein anderer Modul-Code darf auf das entfernte Modul verweisen
4. **CMake-Cache löschen und neu konfigurieren:**
   ```bash
   ./pruefen.sh alles
   ```
5. **Tests laufen lassen** — die Kernanwendung muss weiterhin bestehen
6. **Dokumentation aktualisieren**

### Regeln für gute Wartbarkeit

- Ein Modul darf niemals direkt in `kern/`, `protokoll/`, `speicher/` oder `dienst/` eingreifen. Es nutzt ausschließlich die öffentlichen Schnittstellen.
- UI-Elemente eines Moduls werden nur geladen, wenn das Modul aktiv ist.
- Datenbanktabellen eines Moduls tragen ein Modul-Präfix, damit Reste nach dem Entfernen leicht erkannt und bereinigt werden können.
- Jedes Modul bringt eigene Tests mit. Wird ein Modul entfernt, entfallen auch seine Tests.
- Vor jedem Commit und jedem Merge läuft `./pruefen.sh`.

## Beispielmodul: Adressbuch

```
quelltext/bibliothek/erweiterung/adressbuch/
├── kontakt.h
├── kontakt.cpp
├── adressbuch_dienst.h
├── adressbuch_dienst.cpp
└── adressbuch_modul.h          # implementiert AdlerMail::Erweiterung::Modul

quelltext/oberflaeche/erweiterung/adressbuch/
├── AdressbuchAnsichtModell.h
├── AdressbuchAnsichtModell.cpp
└── AdressbuchSeite.qml

pruefungen/erweiterung/adressbuch/
└── tst_adressbuch_dienst.cpp
```

Im Build:

```cmake
if(ADLERMAIL_ADRESSBUCH)
    add_subdirectory(quelltext/bibliothek/erweiterung/adressbuch)
    add_subdirectory(quelltext/oberflaeche/erweiterung/adressbuch)
    add_subdirectory(pruefungen/erweiterung/adressbuch)
endif()
```

So bleibt AdlerMail erweiterbar, reduzierbar und wartbar.

## Sprachen verwalten

### Sprache hinzufügen

Neue Sprachen sollen sich ergänzen lassen, ohne Codeänderungen in der Geschäftslogik oder in der Oberfläche:

1. **Neue `.ts`-Datei anlegen** unter `uebersetzungen/`, z. B. `uebersetzungen/adlermail_fr.ts` für Französisch
2. **Für jeden bestehenden Schlüssel die Übersetzung hinzufügen**
3. **CMake aktualisieren**, damit die neue Übersetzungsdatei gebaut wird
4. **Alle Quelltexte prüfen**:
   - QML: `qsTr("Schlüssel")` verwendet?
   - C++: `QCoreApplication::translate("Kontext", "Schlüssel")` verwendet?
   - Keine festen Strings in der Oberfläche
5. **Einstellungen erweitern**, damit die neue Sprache in den Optionen auftaucht
6. **Tests ergänzen**, z. B. prüfen, ob die Sprache geladen wird und jeder Schlüssel eine Übersetzung besitzt
7. **CI anpassen**, damit die neue `.ts`-Datei im Übersetzungs-Check berücksichtigt wird
8. **Dokumentation aktualisieren** in `SOUL.md`, `README.md` und `AGENTS.md`

**Regel:** Neue Sprache = neue `.ts`-Datei + Build-Anpassung. Keine Änderung der Programmlogik. Jede Sprache hat denselben Schlüsselsatz.

### Einzelne Übersetzung korrigieren

Wenn ein Wort oder Satz in einer Sprache falsch ist:

1. Die betreffende `.ts`-Datei öffnen
2. Nur den `<translation>`-Eintrag des betroffenen Schlüssels ändern
3. `lrelease` ausführen, um neue `.qm`-Datei zu erzeugen
4. Optional: Schneller Sicht-Check in der App
5. Kein erneuter Build der Anwendung nötig

**Regel:** Übersetzungsfehler werden **immer** in der `.ts`-Datei behoben, niemals im Quelltext. Der deutsche Schlüssel bleibt stabil.

### Sprache entfernen

Sprachen, die nicht mehr benötigt werden, sollen komplett und rückstandsfrei aus dem Projekt verschwinden:

1. **Entscheidung dokumentieren** — Warum wird die Sprache entfernt?
2. **`.ts`-Datei löschen** unter `uebersetzungen/`, z. B. `uebersetzungen/adlermail_fr.ts`
3. **CMake aktualisieren** — Datei aus dem Build-Prozess entfernen
4. **Einstellungen anpassen** — Sprache aus der Auswahlliste entfernen
5. **Quelltext auf hartkodierte Sprache prüfen**:
   - Keine `if (sprache == "fr")`-Sonderfälle
   - Keine sprachabhängigen Icons, URLs oder Formate
   - Keine Fallback-Texte für genau diese Sprache
6. **Datenbank oder Einstellungen bereinigen** — Falls Nutzer diese Sprache als Voreinstellung hatten, zum neuen Fallback migrieren
7. **Tests anpassen** — Sprache aus Sprach-Checks entfernen, ggf. Migrationstest ergänzen
8. **CI anpassen** — Übersetzungsprüfung prüft nicht mehr diese Sprache
9. **Dokumentation aktualisieren** in `SOUL.md`, `README.md`, `AGENTS.md`

**Regel:** Sprache entfernen = Datei löschen + Build + Einstellungen + CI. Keine toten Sprachreferenzen im Code lassen.

## Grundlagen ändern

„Grundlagen“ meint alles, was das gesamte Projekt betrifft:
- Build-System (CMake-Targets, Optionen, Sanitizer)
- Verzeichnisstruktur
- Konventionen (Namensgebung, Header-Struktur, Fehlerbehandlung)
- C++-Standard oder Qt-Version
- Prüfscript (`pruefen.sh`)
- Architekturprinzipien (z. B. neue Modulschnittstelle)

Diese Änderungen dürfen niemals leichtfertig oder im laufenden Feature-Zweig passieren. Der Prozess ist:

1. **Problem klar beschreiben** — Was ist der Grund? Was ist das Ziel?
2. **Alternativen prüfen** — Gibt es eine Lösung, die ohne Grundänderung funktioniert?
3. **Kleines Experiment** im Branch oder in einem Spike-Verzeichnis
4. **Auswirkungen prüfen:**
   - Welche Module sind betroffen?
   - Welche Tests müssen angepasst werden?
   - Welche Dokumentationen veralten?
   - Braucht es ein Migrationsskript?
5. **Änderung in einem separaten Branch** umsetzen — nicht zusammen mit einem Feature
6. **Vollständiges `./pruefen.sh alles`** laufen lassen
7. **Review anfordern** mit Fokus auf Architektur und Konventionen
8. **Dokumentation anpassen:**
   - `AGENTS.md` für Agenten-Kontext
   - `KONVENTIONEN.md` für Programmierregeln
   - `SOUL.md` für Architekturprinzipien
   - `README.md` für Benutzer- und Mitwirkenden-Infos
9. **Migration für andere Mitwirkende dokumentieren**, falls notwendig

**Regel:** Grundlagen-Änderungen sind Änderungen der Spielregeln. Sie werden einzeln, bewusst und mit ArchitekturReview entschieden. Nicht nebenbei.

## Versionierung und Release-Prozess

AdlerMail startet mit Version **0.0.1**. Die führende `0` zeigt an, dass sich das Projekt noch in der frühen Entwicklung befindet und die öffentliche API noch nicht stabil ist.

### Semantic Versioning

Schema: `MAJOR.MINOR.PATCH`

| Stelle | Hochzählen wenn | Beispiel |
|---|---|---|
| MAJOR | Umbruch in Architektur, öffentlicher API oder Modulvertrag; Bruch der Rückwärtskompatibilität | Qt 7, neuer Speicherdienst, neuer Modulvertrag |
| MINOR | Neues Feature, neues Modul, sichtbare Erweiterung der Funktionalität | Kalender-Modul hinzugefügt, neue Sprache, neuer Dienst |
| PATCH | Fehlerbehebung, Übersetzungskorrektur, kleines Refactoring ohne Funktionsänderung | Button-Text korrigiert, IMAP-Timeout-Fix, Lücken in Übersetzungen |

### Wann wird was hochgezählt?

- **0.0.1** — Erster commit-tauglicher Zustand: Projektgerüst steht, Build läuft, Kern-Tests bestehen.
- **0.0.2** — Kleine Korrektur am Gerüst, Build-Skript, Formatierung.
- **0.1.0** — Erstes sichtbares Feature (z. B. Konto anlegen, Datenbank läuft, UI-Grundgerüst).
- **0.2.0** — Nächstes Feature (z. B. IMAP-Verbindung, Postfachanzeige).
- **1.0.0** — Erste stabile Version für Endnutzer, API und Modulvertrag gelten als stabil.

### GitHub-Integration

Auf GitHub wird jede Version durch ein **Git-Tag** und ein **GitHub-Release** markiert:

1. **Tag setzen** lokal:
   ```bash
   git tag -a v0.1.0 -m "Version 0.1.0: Konto- und Postfachansicht"
   git push origin v0.1.0
   ```
2. **GitHub-Release** aus dem Tag erstellen mit:
   - Versionsnummer
   - Kurzzusammenfassung auf Deutsch
   - Changelog der wichtigsten Änderungen
   - Hinweis auf Breaking Changes, falls vorhanden
   - Angehängte Pakete (Flatpak, Windows-Installer, Debug-Symbole)
3. **Milestones** für geplante Versionen anlegen, z. B. `v0.2.0`. Issues und Pull-Requests dem passenden Milestone zuordnen. So sieht man sofort, welche Features für welche Version geplant sind.

### Version sichtbar machen

- In `SOUL.md`, `README.md` und `AGENTS.md` steht die aktuelle Version im Header.
- In der App selbst (`Hilfe → Über AdlerMail`) wird die Versionsnummer angezeigt.
- `adlermail_hilfe.sh --version` gibt die aktuelle Version aus.
- CMake definiert `ADLERMAIL_VERSION_MAJOR`, `ADLERMAIL_VERSION_MINOR`, `ADLERMAIL_VERSION_PATCH`.

### Versions-Workflow für eine neue Funktion

Angenommen, aktuell ist `0.1.0` und das Kalender-Modul soll hinzukommen:

1. Branch `feature/kalender-grundgeruest` erstellen
2. Feature implementieren und testen
3. Merge-Request stellen; CI läuft grün
4. Nach dem Merge in den Hauptzweig:
   ```bash
   git checkout hauptzweig
   git pull origin hauptzweig
   git tag -a v0.2.0 -m "Version 0.2.0: Kalender-Grundgerüst"
   git push origin v0.2.0
   ```
5. GitHub-Release für `v0.2.0` erstellen

### Keine Mischzahlen innerhalb eines Branches

- Innerhalb eines Feature-Branches ändert sich die Versionsnummer **nicht**.
- Erst beim Merge auf den Hauptzweig wird getaggt.
- Mehrere kleine Features können zusammen in ein Minor-Release fließen, wenn sie im selben Milestone liegen.

### Vorbereitung für 1.0.0

Version `1.0.0` wird erst erreicht, wenn:

- Alle Basis-Features funktionieren (Konto, IMAP, SMTP, Lesen, Schreiben, Suchen, Löschen)
- Die Modulschnittstelle stabil ist
- Mehrsprachigkeit (DE/EN/ES) vollständig ist
- Alle geplanten Basistests grün sind
- Dokumentation (`SOUL.md`, `AGENTS.md`, `README.md`, `KONVENTIONEN.md`) aktuell ist


## Update- und Migrationsstrategie

- **Datenbank-Versionierung:** Jedes Release definiert eine Schema-Version. Beim Start wird die Datenbank automatisch schrittweise migriert.
- **Migrationsregeln:**
  - Migrationen liegen in `quelltext/bibliothek/speicher/migrationen/`
  - Jede Migration ist reversibel oder dokumentiert absichtlich irreversible Schritte
  - Neue Spalten sind nullable oder haben sinnvolle Defaults
  - Gelöschte Modul-Tabellen werden als `alt_<modul>_<tabelle>` markiert und nach erfolgreichem Start bereinigt
- **Einstellungen:** Einstellungsdateien sind versioniert. Unbekannte Schlüssel werden ignoriert, nicht gelöscht.
- **Rückwärtskompatibilität:** Ein Release kann mindestens die Einstellungen und Datenbankschema der vorherigen Minor-Version öffnen.

## Teststrategie

| Ebene | Werkzeug | Ziel |
|---|---|---|
| Unit | Qt Test (`QTest`) | Logik und Modelle isoliert testen |
| Integration | CTest + lokale Testserver | IMAP/SMTP-Funktionen mit `QTcpServer` |
| GUI-Logik | ViewModel-Tests in Qt Test | QML-Bridging ohne echte UI |
| End-to-End | Flatpak-Test oder manuell | Kritische Nutzerpfade |
| Übersetzungen | CI-Check | Alle Sprachen vollständig |
| Sicherheit | Sanitizer + Valgrind | Speicher- und Laufzeitfehler |

- Jede öffentliche Klasse hat zugehörige Tests oder eine explizite Begründung im Code-Review.
- Neue Features werden nur akzeptiert, wenn sie ihre eigenen Tests mitbringen.

## Code-Review und Merge-Prozess

1. **Feature-Branch** mit aussagekräftigem Namen erstellen, z. B. `feature/kalender-grundgeruest`
2. Vor dem Push: `./pruefen.sh sicher` erfolgreich
3. **Merge-Request** mit Beschreibung auf Deutsch:
   - Was wurde geändert?
   - Warum?
   - Welche Module sind betroffen?
   - Welche Tests wurden ergänzt?
   - Gibt es Breaking Changes?
4. **ArchitekturReview** bei Grundänderungen oder neuen Modulen
5. **Sprach-Review** bei UI-Texten durch Muttersprachler oder Language-Tool
6. Merge nur bei grüner CI, zwei Zustimmungen und ohne unbeantwortete Threads

## Architekturentscheidungen festhalten

Jede wesentliche Architekturentscheidung wird in einer kurzen Entscheidungsnotiz (ADR-Stil) festgehalten:

- Ort: `docs/architektur/` oder im `SOUL.md`-Anhang
- Inhalt: Kontext, Optionen, Entscheidung, Begründung, Konsequenzen
- Beispiele: Wahl von SQLite vs. externer DB, QML vs. Widgets, IMAP-Implementierung via QtNetwork

Ziel ist nicht Bürokratie, sondern Nachvollziehbarkeit für neue Mitwirkende und zukünftige Agenten.

## Dokumentationspflichten

- **Neues Modul:** Eintrag in `SOUL.md` unter Beispielmodul oder Tabelle der aktiven Module
- **Neue Konvention:** Eintrag in `KONVENTIONEN.md` mit Beispiel
- **Änderung am Build oder CI:** Eintrag in `AGENTS.md` und `README.md`
- **Sprachänderung:** Eintrag in `README.md` und `AGENTS.md`
- **API-Bruch:** Dokumentieren und Breaking-Change-Notiz im Changelog

## Support- und Lebenszyklus

- **Aktive Entwicklung:** Zwei aktuelle Minor-Versionen
- **Sicherheitsupdates:** Für die letzten zwei Major-Versionen
- **Qt-Versionswechsel:** Nur bei klarer Notwendigkeit; Anpassung aller Plattform-Builds vor dem Umstieg
- **Modul-Verwerfung:** Ein als veraltet markiertes Modul bleibt mindestens eine Major-Version erhalten, bevor es entfernt wird
