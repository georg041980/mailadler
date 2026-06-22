# AdlerMail — Roadmap v1.0 → v2.0

> Stand: 22. Juni 2026. Alle Versionen sind Zielbilder, keine Garantien.

---

## v1.0.0 — Grundgerüst (✅ released)

**Was drin ist:**
- C++17 + CMake + Ninja Build
- Sanitizer (ASan, UBSan)
- Schichten: kern → speicher → dienst → protokoll
- SQLite-Datenbank mit Konto-CRUD
- Zwischenspeicher für Nachrichten
- IMAP: LOGIN, LIST, SELECT, FETCH (Header + Body)
- SMTP: EHLO, AUTH, MAIL FROM, RCPT TO, DATA
- 6 Test-Suiten, 35 Testfälle
- Pre-Commit-Hook (Formatierung + Bau + Tests)
- GitHub Actions CI
- AGENTS.md mit Auto-Update
- clang-format / clang-tidy

**Was NICHT drin ist (sollte in v1.0 sein):**
- Keine funktionale UI (nur QML-Stubs)
- Kein Startup-Flow (App startet nicht sinnvoll)
- Keine echten Nachrichten lesen/schreiben per UI
- Kein Flatpak-Paket
- Kein Windows-Build

**Fazit:** v1.0 ist ein solides Fundament. Kein Produkt.

---

## v1.1 — Erste lauffähige App ("MVP Mail")

**Ziel:** Ich kann eine echte Mail mit der App lesen.

| Feature | Details |
|---|---|
| UI-Verdrahtung | NachrichtenListe zeigt Daten aus ViewModel, Erstellen validiert |
| Startup-Flow | App startet → DB öffnen → Konto wählen → IMAP verbinden → INBOX anzeigen |
| Mail lesen | Klick in Liste → NachrichtAnsicht zeigt Text/HTML |
| Mail schreiben | Verfassen-Fenster → SMTP senden |
| Ein Konto | Nur ein Konto, hart kodiert oder aus DB |
| Deutsch | Alle UI-Texte deutsch |

**Tests:** 10 Suiten (bestehende + UI-Integration + E2E)

**Paketierung:** Flatpak baubar (nicht auf Flathub)

---

## v1.2 — Alltagstauglich

**Ziel:** Ich benutze AdlerMail täglich statt Thunderbird.

| Feature | Details |
|---|---|
| Mehrere Konten | Konto-Verwaltung (hinzufügen, wechseln, löschen) |
| Anhänge empfangen | Download + Öffnen mit System-App |
| Volltextsuche | SQLite FTS5 über Betreff + Absender + Inhalt |
| HTML-Mail-Darstellung | QTextBrowser oder Qt WebEngine |
| Offline-Cache | Gelesene Mails ohne Netzwerk verfügbar |
| Fehlerbehandlung | Verbindungsabbruch, Timeout, Auth-Fehler → UI-Dialog statt Crash |

**Tests:** 20+ Suiten

---

## v1.3 — Verteilt unter Freunden

**Ziel:** 5 Leute benutzen es und melden Bugs.

| Feature | Details |
|---|---|
| Flatpak auf Flathub | Ein-Klick-Installation |
| Englische Übersetzung | Qt Linguist .ts-Datei |
| Anhänge senden | Datei-Dialog → SMTP MIME |
| Ordner-Verwaltung | Erstellen, Umbenennen, Löschen von IMAP-Ordnern |
| Thread-Darstellung | Conversation View (optional, experimentell) |

---

## v1.5 — Windows + Stabilität

**Ziel:** Windows-Nutzer installieren per .exe.

| Feature | Details |
|---|---|
| Windows-Installer | NSIS .exe, signiert |
| Spanische Übersetzung | Dritte Sprache |
| Automatische Updates | Flatpak auto-update, Windows: Eigenbau oder Winget |
| Dark Mode | QML System-Theme folgen |
| Tastaturkürzel | Strg+N, Strg+R, Strg+Enter, … |

---

## v2.0 — Feature-komplett

**Ziel:** Eigenständiger Client. Thunderbird-Ersatz für Power-User.

| Feature | Details |
|---|---|
| Kalender (CalDAV) | Termine anzeigen + erstellen |
| PGP/GPG | Ver- und Entschlüsselung, Signatur-Prüfung |
| Filter/Regeln | Serverseitig (Sieve) + Client-seitig |
| Adressbuch | CardDAV oder lokal |
| Benachrichtigungen | Desktop-Notification bei neuer Mail |
| Mobile (Android) | Qt für Android Build |

---

## Meilensteine im Überblick

```
v1.0 ──── Grundgerüst (Juni 2026)        ✅ HEUTE
v1.1 ──── Erste lauffähige App           ⬜
v1.2 ──── Alltagstauglich                ⬜
v1.3 ──── Verteilt (Flatpak, Englisch)   ⬜
v1.5 ──── Windows + Stabil               ⬜
v2.0 ──── Thunderbird-Ersatz             ⬜
```

## Tages-Arbeitsablauf

```
Neuen Chat öffnen
  → "Wechsle nach /home/georg/adlermail"
  → Hermes liest AGENTS.md (auto-aktualisiert)
  → "Führe Plan .hermes/plans/2026-06-22_ui-verdrahtung.md aus"
  → Bauen + Testen + Committen
  → Pre-Commit-Hook prüft alles
  → AGENTS.md aktualisiert sich selbst
```
