# MailAdler — Master-Plan v0.1 → v1.0

> Stand: 22. Juni 2026 | 13 Commits | 6 Tests grün | 0 Warnungen

---

## ✅ Fertig (v0.1)

| Bereich | Was | Dateien |
|---------|-----|---------|
| Build | CMake+Ninja, Sanitizer, clang-format/tidy | `CMakeLists.txt`, `cmake/` |
| Konventionen | Deutsche Namen, `m_`-Prefix, Doxygen | `KONVENTIONEN.md` |
| Datenbank | SQLite, Konto-CRUD, 5 Tests | `speicher/datenbank.*` |
| Zwischenspeicher | RAM-Cache, 3 Tests | `speicher/zwischenspeicher.*` |
| IMAP | Echte TLS-Verbindung, LOGIN+LIST, Mock-Tests | `protokoll/imap_verbindung.*` |
| SMTP | Echte TLS-Verbindung, EHLO..DATA, Mock-Tests | `protokoll/smtp_verbindung.*` |
| Dienste | KontoDienst (CRUD), PostfachDienst (+IMAP) | `dienst/konto_dienst.*`, `dienst/postfach_dienst.*` |
| ViewModels | Liste, Erstellen, Ordner, Detail | `ansichtmodelle/*` |
| QML | HauptFenster, NachrichtenListe, Ansicht, Erstellen | `qml/*.qml` |
| main.cpp | Alle ViewModels+Dienste verdrahtet, 3 Testnachrichten | `anwendung/main.cpp` |
| CI | GitHub Actions (ubuntu-24.04, Sanitizer) | `.github/workflows/` |
| Bereinigung | Übersetzungen entfernt, fest Deutsch | |

---

## 🔲 v0.2 — Echte Nachrichten (IMAP SELECT+FETCH)

**Ziel:** Nachrichten von echtem IMAP-Server abrufen und anzeigen.

| # | Task | Status |
|---|------|--------|
| 1 | ImapVerbindung: `ordnerAuswaehlen()` + `nachrichtenHeaderAbrufen()` | ⬜ |
| 2 | ImapVerbindung: `nachrichtInhaltAbrufen()` (FETCH BODY) | ⬜ |
| 3 | Datenbank: Nachrichten-Tabelle + CRUD | ⬜ |
| 4 | PostfachDienst: `nachrichtenLaden()` implementiert | ⬜ |
| 5 | SmtpVerbindung: echte Parameter (Absender, Empfänger, Betreff) | ⬜ |
| 6 | KontoAnsichtModell: ViewModel für Konto-Dialog | ⬜ |
| 7 | KontoDialog.qml: UI-Maske zum Anlegen/Bearbeiten | ⬜ |
| 8 | KontoAuswahlModell: Liste vorhandener Konten | ⬜ |
| 9 | main.cpp: Startup-Flow (Konto→IMAP→Nachrichten) | ⬜ |
| 10 | AGENTS.md aktualisieren | ⬜ |

**Tests:** 8 neue Testfälle (IMAP SELECT, IMAP FETCH, DB-Nachrichten, SMTP-Parameter)

---

## 🔲 v0.3 — UI-Polish & Usability

**Ziel:** App fühlt sich wie ein echter Mail-Client an.

| # | Task |
|---|------|
| 11 | Split-Pane: Ordnerliste links, Nachrichten mitte, Detail rechts |
| 12 | Nachrichtenliste: sortieren nach Datum, Betreff, Absender |
| 13 | Nachrichtenliste: gelesen/ungelesen visuell + Klick markiert als gelesen |
| 14 | Mehrere Ordner: Klick auf Ordner lädt dessen Nachrichten |
| 15 | Aktualisieren-Button: lädt Nachrichten neu vom Server |
| 16 | Statusleiste: "Verbunden mit imap.example.com · 3 Nachrichten" |
| 17 | Lade-Indikator: Spinner während IMAP-Anfragen |
| 18 | Fehler-Dialog: `fehlerAufgetreten`-Signale → QML-Meldung |

---

## 🔲 v0.4 — Konto-Verwaltung & Sicherheit

**Ziel:** Konten verwalten, Passwörter sicher speichern.

| # | Task |
|---|------|
| 19 | Passwort-Verschlüsselung: libsodium oder Qt-eigene (QPasswordDigestor) |
| 20 | Konto bearbeiten: bestehendes Konto ändern |
| 21 | Konto löschen mit Bestätigungsdialog |
| 22 | Mehrere Konten: Umschalten zwischen Konten |
| 23 | TLS-Zertifikatsprüfung: nicht mehr `ignoreSslErrors()` |
| 24 | STARTTLS für SMTP (Port 587) — aktuell nur direktes TLS |

---

## 🔲 v0.5 — E-Mail-Features

**Ziel:** Volle E-Mail-Funktionalität.

| # | Task |
|---|------|
| 25 | HTML-E-Mails rendern (QTextBrowser oder simplen HTML→Text-Konverter) |
| 26 | Anhänge: Liste anzeigen, lokal speichern |
| 27 | Anhänge senden (MIME multipart) |
| 28 | Antworten / Weiterleiten |
| 29 | CC / BCC-Felder in ErstellenAnsicht |
| 30 | Entwürfe lokal speichern |
| 31 | Papierkorb / Archivieren (IMAP MOVE/COPY) |
| 32 | Suchfunktion (lokal in SQLite) |

---

## 🔲 v0.6 — Verpackung & Deployment

**Ziel:** App ist installierbar und auf anderen Rechnern nutzbar.

| # | Task |
|---|------|
| 33 | Flatpak-Manifest: `verpackung/flatpak/` vervollständigen |
| 34 | App-Icon + .desktop-Datei |
| 35 | Linux: AppImage-Build |
| 36 | Windows: NSIS-Installer (Qt 6.8 auf Windows testen) |
| 37 | macOS: .dmg-Bundle (optional) |

---

## 🔲 v0.7 — Echte Server-Tests

**Ziel:** App funktioniert mit echten Mail-Servern.

| # | Task |
|---|------|
| 38 | Gmail IMAP-Test (App-Passwort) |
| 39 | Gmail SMTP-Test |
| 40 | Weitere Anbieter: GMX, Web.de, Outlook.com |
| 41 | Zeitüberschreitungen: reconnect-Logik |
| 42 | IDLE-Modus (Push für neue Nachrichten) — optional |

---

## 🔲 v1.0 — Release

| # | Task |
|---|------|
| 43 | README.md: Screenshots, Installations-Anleitung |
| 44 | Versionsnummer in CMakeLists.txt auf 1.0.0 |
| 45 | Changelog / Release-Notes |
| 46 | GitHub Release mit Flatpak + AppImage |
