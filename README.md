# AdlerMail

Plattformübergreifender E-Mail-Client mit Qt 6 und QML.

**Status:** v1.0.0 | 6 Tests | 0 Warnungen | C++17

## Features

- **IMAP:** Verbinden, Anmelden, Ordnerliste, SELECT, FETCH (Header+Body)
- **SMTP:** Senden mit TLS/STARTTLS, Authentifizierung
- **Oberfläche:** 3-Spalten (Ordner | Nachrichten | Detail), QML+C++
- **Konten:** Anlegen, Bearbeiten, Löschen, Mehrere Konten
- **Nachrichten:** Suchen, Entwürfe speichern, Antworten, Löschen
- **Anhänge:** Anzeige (Empfang), Auswahl (Senden)
- **Sicherheit:** Passwort-Base64, TLS-Zertifikatsprüfung
- **Sprache:** Deutsch (fest, keine Übersetzungsdateien)

## Bauen (lokal)

```bash
./pruefen.sh              # Formatierung + Sanitizer-Bau + Tests
./pruefen.sh schnell      # Ohne Sanitizer, ~1s
./pruefen.sh test         # Nur Tests
```

## Mit echtem Server testen

1. App starten: `./bau/quelltext/anwendung/adlermail`
2. Konto anlegen mit echten Zugangsdaten
3. App verbindet automatisch beim Start

**Getestet mit:**
- Gmail (App-Passwort erforderlich)
- GMX / Web.de
- Eigener IMAP/SMTP-Server

## Flatpak bauen

```bash
flatpak-builder --user --install build-dir verpackung/flatpak/org.adlermail.AdlerMail.yaml
flatpak run org.adlermail.AdlerMail
```

## Konventionen

Siehe [KONVENTIONEN.md](KONVENTIONEN.md). Alle Quelldateien deutsch.
