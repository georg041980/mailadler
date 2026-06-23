# MailAdler

> **Ein E-Mail-Client, der auf Deutsch gebaut wird, ab Tag eins auf Deutsch, Englisch und Spanisch spricht — und mitwächst, ohne zu wuchern.**

**Repository:** [https://github.com/georg041980/mailadler](https://github.com/georg041980/mailadler)

Plattformübergreifender E-Mail-Client mit **C++17**, **Qt 6** und **QML**.

| | |
|---|---|
| **Status** | v0.0.1 — Projektgerüst und Kernmodule |
| **Tests** | 6 Suiten, 36 Testfälle, alle grün |
| **Sprachen** | Deutsch, Englisch, Spanisch |

---

## Vision

MailAdler soll der E-Mail-Client werden, den man sich selbst zusammenstellen kann: eine schlanke Basis, eine klare Architektur, mehrsprachig von Haus aus — mit wachsenden Funktionsbausteinen, aber ohne Kompromisse bei Sicherheit, Stabilität und Übersichtlichkeit. Der Funktionsumfang lässt sich je nach Anforderung erweitern oder reduzieren.

## Technologie

- **Sprache:** C++17; Quellcode, Kommentare, Konventionen und Commit-Messages auf Deutsch
- **Build:** CMake + Ninja
- **UI:** QML + C++-ViewModels, keine Widgets
- **Datenbank:** QtSql + SQLite
- **Protokolle:** IMAP/SMTP
- **Tests:** Qt Test (`QTest`), CTest
- **Paketierung:** Flatpak (Linux), NSIS (Windows), später DMG (macOS)

Nur Qt-/C++-Schlüsselwörter wie `signals`, `slots`, `override`, `nullptr` bleiben aus technischen Gründen englisch.

## Architektur

```
quelltext/
├── bibliothek/          # Geschäftslogik, keine GUI-Abhängigkeit
│   ├── kern/            # Datenmodelle
│   ├── protokoll/       # IMAP/SMTP
│   ├── speicher/        # SQLite, Zwischenspeicher
│   ├── dienst/          # High-Level-Dienste
│   └── erweiterung/     # Modulschnittstelle
├── oberflaeche/         # QML + ViewModels
└── anwendung/           # main.cpp
```

`bibliothek/` ist ohne GUI testbar. `oberflaeche/` enthält ausschließlich Darstellung.

## Kernprinzipien

### Modularität

- Neue Features werden als Module in `erweiterung/` hinzugefügt.
- Jedes Modul bringt eigene Logik, UI und Tests mit.
- Module werden über CMake-Optionen ein- und ausgeschaltet.
- Nicht benötigte Module können physisch entfernt werden.

### Mehrsprachigkeit

- Übersetzungen arbeiten mit festen Schlüsseln.
- QML: `qsTr("Schlüssel")`
- C++: `QCoreApplication::translate("Kontext", "Schlüssel")`
- Der deutsche Schlüsseltext ist gleichzeitig Fallback.
- Übersetzungsfehler werden nur in den `.ts`-Dateien korrigiert, nicht im Quelltext.

### Qualität

- Bau mit Sanitizern (AddressSanitizer, UndefinedBehaviorSanitizer, LeakSanitizer)
- Automatisierte Tests mit CTest
- `clang-format` als verbindliche Formatierung
- Keine Exceptions im Qt-Code — Fehler über Signale

## Bauen

```bash
./pruefen.sh              # Formatierung, Sanitizer, Tests
./pruefen.sh schnell      # Ohne Sanitizer, ~1s
./pruefen.sh test         # Nur Tests
./pruefen.sh alles        # Neu von Grund auf bauen und testen
```

## Versionierung

MailAdler startet mit **v0.0.1**. Es gilt Semantic Versioning:

| Stelle | Hochzählen wenn |
|---|---|
| MAJOR | Architekturbruch, API-Bruch, neuer Modulvertrag |
| MINOR | Neues Feature, neues Modul, neue Sprache |
| PATCH | Fehlerbehebung, Übersetzungskorrektur |

Tags werden auf dem Hauptzweig gesetzt: `git tag -a v0.1.0 -m "Version 0.1.0: ..."`. Auf GitHub werden Releases und Milestones für jede Version gepflegt.

## Arbeitsabläufe

### Modul hinzufügen

1. Modulnamen festlegen, z. B. `kalender`
2. Verzeichnisse anlegen:
   ```
   quelltext/bibliothek/erweiterung/kalender/
   quelltext/oberflaeche/erweiterung/kalender/
   pruefungen/erweiterung/kalender/
   ```
3. `MailAdler::Erweiterung::Modul` implementieren
4. CMake-Option aktivieren
5. Logik, ViewModel und QML implementieren
6. Tests und Übersetzungen ergänzen
7. `./pruefen.sh` ausführen

### Modul entfernen

1. CMake-Option auf `OFF` setzen
2. Modulverzeichnisse löschen
3. Abhängigkeiten prüfen
4. `./pruefen.sh alles` ausführen

### Sprache hinzufügen

1. Neue `.ts`-Datei in `uebersetzungen/` anlegen
2. Alle bestehenden Schlüssel übersetzen
3. CMake einbinden
4. Tests und CI ergänzen

### Sprache entfernen

1. `.ts`-Datei entfernen
2. CMake, Einstellungen und CI anpassen
3. Tote Referenzen im Quelltext bereinigen

### Grundlagen ändern

Änderungen am Build, der Verzeichnisstruktur, den Konventionen oder der Qt-Version dürfen nicht nebenbei in einem Feature-Branch passieren. Sie erfordern ein separates Experiment, ein Review und die Aktualisierung aller Dokumentationen.

### Funktionen reduzieren ohne Neuanfang

Manchmal entwickelt sich ein Modul oder Feature in eine Richtung, die sich nicht mehr zum Projekt passt. Dann soll das Bestehende erhalten bleiben, aber das Problem-Feature gezielt entfernt werden.

**Vorgehen:**

1. **Status festhalten**
   - Aktuellen Stand committen oder taggen, z. B. `git tag -a v0.3.0-vor-reduktion`
   - Alle Tests müssen grün sein

2. **Entscheidung dokumentieren**
   - Warum wird das Feature entfernt?
   - Was bleibt erhalten?
   - Gibt es Ersatz innerhalb eines anderen Moduls?

3. **Abhängigkeiten analysieren**
   - Welche Module oder Klassen nutzen das Feature?
   - Welche Datenbanktabellen gehören dazu?
   - Welche Übersetzungsschlüssel werden nur hier verwendet?

4. **Schrittweise entfernen**
   - CMake-Option auf `OFF` setzen
   - UI-Verweise entfernen
   - Dienst- und Logikaufrufe entkoppeln
   - Tabellen mit Modul-Präfix markieren oder migrieren
   - Modulordner schließlich löschen

5. **Bereinigung im Kern**
   - Gemeinsam genutzte Hilfsfunktionen, die nur für das entfernte Modul existierten, nach `kern/` verschieben oder löschen
   - Leere Interfaces oder Verzweigungen entfernen
   - Keine toten `if (modulAktiv)`-Zweige lassen

6. **Tests anpassen**
   - Modulspezifische Tests entfernen
   - Kern-Tests müssen weiterhin bestehen
   - Falls Daten migriert werden, Migrationstests ergänzen

7. **Neu bauen und taggen**
   - `./pruefen.sh alles`
   - Neuen Tag setzen: `git tag -a v0.4.0`

**Wichtig:** Ein Feature wird niemals einfach auskommentiert oder versteckt. Es wird bereinigt. Wer es später wiederbringen will, kann es aus der Git-Historie zurückholen oder als neues Modul neu implementieren.

## Konventionen

| Was | Regel | Beispiel |
|---|---|---|
| Klassen | PascalCase, deutsch | `Postfach`, `NachrichtenListe` |
| Member | `m_`-Präfix | `m_verbindung` |
| Getter (bool) | `ist`/`hat` | `istVerbunden()` |
| Setter | `setze` + Name | `setzeServer()` |
| Signale | Partizip Perfekt | `verbunden()` |
| Slots | `bei` + Ereignis | `beiSendenGeklickt()` |
| Fehler | ein Signal pro Klasse | `fehlerAufgetreten(QString)` |
| Dateinamen | Kleinbuchstaben, Unterstriche | `postfach.h` |

Vollständige Regeln siehe `KONVENTIONEN.md`.

## Mitwirken

1. Feature-Branch erstellen
2. `./pruefen.sh sicher` vor dem Push ausführen
3. Merge-Request auf Deutsch beschreiben
4. Merge nur bei grüner CI und ArchitekturReview bei Grundänderungen

## Support und Lebenszyklus

- Zwei aktuelle Minor-Versionen werden aktiv entwickelt
- Sicherheitsupdates für die letzten zwei Major-Versionen
- Qt-Wechsel nur bei klarer Notwendigkeit
- Veraltete Module bleiben mindestens eine Major-Version erhalten

## Anwendungsbereiche

### Entwürfe

- Nachrichtenentwürfe werden automatisch in der SQLite-Datenbank zwischengespeichert.
- Beim Beenden der App bleiben ungesendete Entwürfe erhalten.
- Entwürfe können später fortgesetzt, dupliziert oder gelöscht werden.
- Vor dem Senden prüft der Editor auf fehlende Pflichtfelder (`An`, `Betreff`).

### Suche und Filter

- Lokale Suche über Betreff, Absender, Empfänger und Nachrichtentext
- Schnellfilter: Ungelesen, Markiert, Mit Anhang, Heute, Letzte 7 Tage
- Ordnerübergreifende Suche möglich
- Suchanfragen werden nicht an den Server gesendet

### Einstellungen

Einstellungen werden lokal in einer dateibasierten Konfiguration gespeichert:

- Sprache
- Theme / Kontrastmodus
- Konten und Servereinstellungen
- Synchronisationsintervall
- Cache- und Speicherbegrenzungen

Unbekannte Einstellungsschlüssel werden ignoriert, nicht gelöscht.

### Logging und Diagnose

- Laufzeitprotokolle landen in einer rotierenden Logdatei
- Passwörter, Tokens und sensible Mailinhalte werden niemals geloggt
- Fehler werden über das zentrale Fehlersignal `fehlerAufgetreten(QString)` gemeldet
- Optionale, anonymisierte Fehlerberichte können aktiviert werden

### Backup und Wiederherstellung

- Datenbank und Einstellungen können exportiert werden
- Import überschreibt oder ergänzt bestehende Daten je nach gewähltem Modus
- Automatisches Backup beim Beenden der App kann aktiviert werden
- Sicherungen enthalten keine Passwörter im Klartext

### Onboarding

Beim ersten Start:

1. Sprache auswählen
2. Willkommensseite mit Hinweisen zur Datensicherheit
3. Erstes E-Mail-Konto anlegen
4. Verbindung testen und Ordner synchronisieren
5. Hauptfenster öffnet sich

### Bekannte Fehler melden

Fehler werden am besten über GitHub Issues gemeldet:

- Betroffene Version
- Betriebssystem
- Schritte zur Reproduktion
- Erwartetes und tatsächliches Verhalten
- Relevante Logausschnitte ohne persönliche Daten

## Entwicklungsstatus und Fehlerverwaltung in der App

MailAdler soll Nutzer und Entwickler über den aktuellen Stand direkt informieren. Dafür gibt es in der App einen Bereich **„Entwicklung & Fehler"**.

### Was Nutzer sehen

- Aktuelle Version
- Liste der geplanten Features und Module
- Liste bekannter, noch nicht behobener Fehler
- In welcher Reihenfolge daran gearbeitet wird
- Status einer jeden Aufgabe: `Geplant`, `In Arbeit`, `Getestet`, `Fertig`

### Dopplungen vermeiden trotz unterschiedlicher Worte

Ein und dieselbe Sache klingt für eine Rentnerin, einen Entwickler oder eine Schülerin oft unterschiedlich. Trotzdem sollen nicht immer neue Issues entstehen, nur weil jemand ein anderes Wort verwendet.

**Lösung: Ein einfacher Eintrag, automatisch übersetzt, intelligent vorgeschlagen.**

Jeder Eintrag besitzt:

- einen kurzen, klaren Titel auf Deutsch
- eine kurze Beschreibung auf Deutsch in einfachen Worten
- automatische Übersetzung des Titels und der Beschreibung in EN und ES

**So sucht die App:**

1. Der Nutzer beschreibt sein Ziel in freien Worten.
   - Beispiel: „Ich will wichtige Mails rot markieren."
   - Beispiel: „Farbliche Hervorhebung von Nachrichten."

2. Die App sucht in Titeln und Beschreibungen.
   - Es gibt keine handgepflegten Synonymlisten.
   - Die Suche vergleicht Wörter und Wortteile sowie die Übersetzungen.

3. Die App zeigt die 3–5 passendsten Einträge an.
   - Jeder Treffer erscheint in der Sprache der App.
   - Die Beschreibung sagt in 1–2 Sätzen, worum es geht.

4. Der Nutzer wählt:
   - „Ja, das meinte ich." → Kein neuer Eintrag, ggf. Stimme abgeben.
   - „Nein, etwas anderes." → Neuer Eintrag wird erstellt.

**Neueintrag minimal:**

Wenn wirklich nichts passt, reicht für einen neuen Vorschlag oder Fehler:

- Titel: 3–8 Wörter
- Beschreibung: 1–2 kurze Sätze
- Bereich: Schreiben, Lesen, Ordner, Konten, Synchronisation, Sicherheit …

**Kein zusätzlicher Pflegeaufwand:**

- Keine Synonymlisten
- Keine manuellen Aliase
- Keine mehrsprachigen Textpflege pro Eintrag
- Übersetzungen erfolgen automatisch, nur bei Fehlern manuell korrigiert

**Wichtig:** Die App sucht für den Nutzer. Es gibt keine Schuldfrage wie „Hast du vorher gesucht?". Der Nutzer muss nur sein Ziel beschreiben — die App prüft, ob das Anliegen schon erfasst ist.

### Intelligente Suche

Die Suche soll helfen, schnell das Richtige zu finden — ohne langes Blättern oder mehrfaches Probieren.

**Funktionen der Suche:**

- **Automatische Vorschläge**
  - Sobald der Nutzer tippt, werden passende Einträge angezeigt
  - Rechtschreibfehler werden korrigiert
  - Beispiel: „farben" findet auch „farbige Markierungen"

- **Sprachunabhängigkeit**
  - Einträge werden automatisch in die Sprache der App übersetzt
  - Suche funktioniert in DE, EN und ES gleichzeitig

- **Kategorien filterbar**
  - Fehler, Features, Module, Dokumentation, Usability
  - Status: Geplant, In Arbeit, Getestet, Fertig
  - Priorität: Hoch, Mittel, Niedrig

- **Ähnlichkeitsprüfung beim Erstellen**
  - Bevor ein neuer Eintrag angelegt wird, zeigt die App die ähnlichsten existierenden Einträge
  - Der Nutzer kann auswählen: „Das meinte ich" oder „Nein, neuen Eintrag erstellen"

- **Schnellzugriff über Nummern**
  - Jeder Eintrag hat eine eindeutige Nummer wie `FEHLER-00008` oder `FEATURE-00012`
  - Wer die Nummer kennt, gelangt direkt zum Eintrag

### Sprache und Kommunikation in der Suche

Nicht jeder Nutzer spricht dieselbe Sprache — auch nicht innerhalb einer Sprache. Ein Rentner beschreibt ein Problem anders als ein Entwickler. Die Suche muss daher mit einfachen, einheitlichen Einträgen arbeiten und trotzdem verständlich sein.

**Eintrag = einfach + automatisch übersetzt:**

- Titel und Beschreibung werden auf Deutsch in einfachen Worten formuliert.
- Übersetzungen in EN und ES entstehen automatisch aus den `.ts`-Dateien.
- Es gibt keine zusätzliche Liste von Suchbegriffen, Synonymen oder Aliasen.

**Beispiel:**

| Deutsch | Englisch | Spanisch |
|---|---|---|
| E-Mails farblich markieren | Color marking for e-mails | Marcado de color para correos |
| App friert beim Löschen von Ordnern ein | App freezes when deleting folders | La app se congela al borrar carpetas |

**Ergebnis für den Nutzer:**

- Die App zeigt die Beschreibung in der gewählten Sprache an.
- Man muss nicht wissen, wie der Entwickler die Funktion nennt.
- Der Entwickler erkennt sofort, was der Nutzer meint, weil der deutsche Originaltext eindeutig ist.

**„Meinten Sie …?"**

- Wenn die Suche wenig findet, schlägt sie Einträge mit ähnlichen Worten vor.
- Der Nutzer kann auf einen Blick prüfen, ob sein Anliegen schon erfasst ist.
- Beispiel: Suche nach „Mails sortieren" → Vorschlag: „Meinten Sie: Nachrichten filtern?"

**Keine Pflege von Wortschatzlisten:**

- Keine Synonymdateien
- Keine manuellen Aliasse
- Keine sprachspezifischen Schlüsselwörter pro Eintrag
- Alles, was gebraucht wird, steht im Titel und in der kurzen Beschreibung

**Ziel:** Jede Personengruppe versteht, was ein Eintrag bedeutet, ohne dass jemand Listen pflegen muss.

## Aktuelle Einträge in der App

Diese Liste zeigt, welche Features und Fehler aktuell in „Entwicklung & Fehler" geführt werden. Jeder Eintrag hat eine eindeutige Nummer, einen Status und eine Priorität.

### Features

| Nummer | Titel | Bereich | Status | Priorität |
|---|---|---|---|---|
| FEATURE-00001 | Grundlagen für IMAP-Konten | Konten | In Arbeit | Hoch |
| FEATURE-00002 | Lokale Nachrichtensuche | Suche | Geplant | Hoch |
| FEATURE-00003 | Entwürfe speichern und laden | Schreiben | Geplant | Mittel |
| FEATURE-00004 | Schnellfilter im Postfach | Lesen | Geplant | Mittel |
| FEATURE-00005 | Backup und Wiederherstellung | Einstellungen | Geplant | Niedrig |
| FEATURE-00012 | E-Mails farblich markieren | Lesen | Geplant | Mittel |

### Fehler

| Nummer | Titel | Bereich | Status | Priorität |
|---|---|---|---|---|
| FEHLER-00001 | Build schlägt bei fehlendem lupdate fehl | Build | In Arbeit | Hoch |
| FEHLER-00008 | App friert beim Löschen von Ordnern ein | Ordner | In Arbeit | Hoch |

### Legende

| Status | Bedeutung |
|---|---|
| Geplant | Noch nicht begonnen, steht auf der Roadmap |
| In Arbeit | Jemand arbeitet aktuell daran |
| Getestet | Wartet auf endgültige Freigabe |
| Fertig | In der aktuellen Version enthalten |

| Priorität | Bedeutung |
|---|---|
| Hoch | Blockiert wichtige Funktionen oder betrifft viele Nutzer |
| Mittel | Wichtig, aber nicht dringend |
| Niedrig | Kann warten oder ist optional |

### Wie Einträge hinzukommen

1. Nutzer oder Entwickler beschreiben ein Ziel oder einen Fehler in der App.
2. Die App prüft, ob ein ähnlicher Eintrag bereits existiert.
3. Falls nein, wird ein neuer Eintrag mit der nächsten freien Nummer erstellt.
4. Titel und Beschreibung werden auf Deutsch verfasst und automatisch übersetzt.
5. Status wird auf `Geplant` gesetzt, sofern nicht sofort gearbeitet wird.

### Priorisierung

Die Reihenfolge der Arbeit wird automatisch ermittelt.

**Gewichtung in absteigender Priorität:**

1. **Datenverlust und schwerwiegende Fehler**
   - Fehler, die zu Datenverlust führen können
   - Fehler, die die App abstürzen lassen
   - Fehler, die grundlegende Funktionen blockieren

2. **Verbreitung des Fehlers**
   - Fehler, die viele Nutzer betreffen, haben höhere Priorität
   - Bei weniger verbreiteten Fehlern sinkt die Priorität

3. **Nutzerwünsche**
   - Funktionen, für die Nutzer abgestimmt haben
   - Anfragen mit vielen Stimmen kommen früher dran

4. **Entwicklerpriorität**
   - Technische Voraussetzungen, Architekturvoraussetzungen
   - Sicherheitsverbesserungen

Die App legt daraus automatisch eine nachvollziehbare Rangfolge fest. Bei gleicher Gewichtung entscheidet das Alter: Was länger wartet, wird eher bearbeitet.

### Nutzerabstimmung

Nutzer können einmal pro Feature oder Fehler ihre Stimme abgeben.

- Abstimmungen sind anonym
- Sie fließen in die automatische Priorisierung ein
- Eine Stimme kann später geändert, aber nicht doppelt vergeben werden

### Fehlernummern

Jeder Fehler bekommt eine eindeutige Nummer, z. B. `FEHLER-00042`.

- Die Nummer bleibt über die gesamte Lebensdauer des Fehlers gleich
- Durch die Nummer weiß jeder sofort, welcher Fehler gemeint ist
- Zusätzlich wird gespeichert:
  - Wann der Fehler zuletzt bearbeitet wurde
  - Aktueller Status
  - Kurzzusammenfassung
  - Verknüpfung zu weiteren Details (lokale Datei oder GitHub-Issue)

So kann ein Fehler schnell wieder aufgenommen werden, ohne alles neu lesen zu müssen.

### Neue Fehler melden

Wenn ein Fehler das erste Mal auftritt, fragt die App den Nutzer kurz:

- Was hast du gemacht, bevor der Fehler auftrat?
- Lässt sich der Fehler wiederholen?
- Soll der Fehlerbericht gesendet werden?

Diese Information wird lokal gespeichert. Erst wenn es eine bekannte Fehlernummer gibt, wird zur zentralen Stelle verknüpft.

### Datenschutz bei Fehlerberichten

**Wird immer lokal gespeichert:**

- Fehlernummer
- Fehlerbeschreibung
- Letzte Bearbeitung
- Status
- Schritte zur Reproduktion, falls vom Nutzer angegeben

**Wird nur gesendet, wenn der Nutzer zustimmt und der Datenschutz aktiviert ist:**

- Fehlernummer
- Version der App
- Betriebssystem und Qt-Version
- Stacktrace oder Codeposition des Fehlers
- Anonymisierte Schritte zur Reproduktion

**Wird niemals gesendet oder geloggt:**

- Name oder E-Mail-Adresse des Nutzers
- Passwörter, Tokens, Schlüssel
- Mailinhalte oder Anhänge
- Persönliche Dateien oder Verzeichnisse
- Nutzerverhalten außerhalb des Fehlerfalls

### Datenschutzmodi

In den Einstellungen gibt es drei Stufen:

- **Streng:** Keine Fehlerberichte senden, nur lokal speichern
- **Standard:** Fehlerberichte senden, aber ohne zusätzliche Systeminformationen
- **Diagnose:** Detaillierte, aber weiterhin anonyme Fehlerberichte senden

Wird der Datenschutz auf **Streng** gesetzt, werden keine neuen Diagnosedaten mehr gesammelt oder übertragen. Bereits lokal vorhandene Fehlerinformationen bleiben erhalten, bis der Nutzer sie löscht.

## Beispiele für Nutzer und Entwickler

Die folgenden Beispiele zeigen, wie Rentnerinnen, Entwickler und andere Nutzer mit der App umgehen können — unabhängig von ihrer Sprache.

### Beispiel 1: Neue Funktion vorschlagen

**Situation:** Eine Nutzerin möchte gerne Farbmarkierungen für wichtige E-Mails.

**Schritte:**

1. Sie öffnet in der App den Bereich **„Entwicklung & Fehler"**.
2. Sie liest die Liste der geplanten Features.
3. Sie sucht nach „Farbe", „Markierung“ oder „wichtig".
4. Sie findet den Eintrag **„FEATURE-00012: E-Mails farblich markieren"**.
5. Weil ihr Wunsch schon existiert, stimmt sie dafür ab, anstatt einen neuen Vorschlag zu erstellen.

**Falls sie nichts Passendes findet:**

1. Sie klickt auf **„Neue Funktion vorschlagen"**.
2. Sie beschreibt in einfachen Sätzen: „Ich möchte wichtige E-Mails rot markieren können."
3. Sie wählt, ob der Vorschlag öffentlich sichtbar sein soll.
4. Die App zeigt ihr automatisch ähnliche Vorschläge an. Wenn einer passt, wird sie umgeleitet.
5. Erst wenn keiner passt, wird ein neuer Eintrag mit fester Nummer erstellt.

### Beispiel 2: Fehler melden

**Situation:** Ein Nutzer stellt fest, dass das Löschen eines Ordners manchmal die App einfrieren lässt.

**Schritte:**

1. Er öffnet den Bereich **„Entwicklung & Fehler"**.
2. Er tippt „Ordner löschen" in die Suche ein.
3. Die App zeigt den Eintrag **„FEHLER-00008: App friert beim Löschen von Ordnern ein"**.
4. Der Nutzer liest den Status: „In Arbeit". Er weiß, dass jemand daran arbeitet.
5. Er klickt auf **„Ich habe das auch"**, um die Verbreitung zu melden. Das ist anonym.

**Falls der Fehler noch nicht bekannt ist:**

1. Die App fragt kurz: „Was hast du gemacht, bevor der Fehler auftrat?"
2. Der Nutzer beschreibt es: „Ich habe einen Ordner mit vielen Unterordnern gelöscht."
3. Die App speichert das lokal und zeigt eine feste Fehlernummer an.
4. Der Nutzer entscheidet, ob er den Bericht senden möchte.

**Was passiert dann:**

- Der Entwickler sieht den Fehler mit eindeutiger Nummer.
- Er kann direkt nachvollziehen, wie viele Nutzer betroffen sind.
- Er muss nicht lange suchen, sondern kann sofort mit der Fehlerbehebung beginnen.
- Wenn der Fehler behoben ist, ändert sich der Status in der App auf „Fertig".

## Bereitstellung

### Lokal bauen

```bash
./pruefen.sh alles
```

### Flatpak (Linux)

```bash
flatpak-builder --user --install build-dir verpackung/flatpak/org.mailadler.MailAdler.yaml
flatpak run org.mailadler.MailAdler
```

### Windows-Installer

Später über NSIS aus dem Build-Verzeichnis erzeugt.

### Paketierungshinweise

- Bauen immer auf einem sauberen Build-Verzeichnis
- Übersetzungsdateien müssen vor dem Paketieren aktualisiert sein
- Debug-Symbole separat archivieren
- Release-Notes dem Paket beilegen
