# IMAP-Nachrichten & Konto-UI — v0.2 MVP-Plan

> **Für Hermes:** Task für Task implementieren. TDD: erst Test, dann Code, dann Commit.

**Ziel:** AdlerMail kann echte E-Mails von einem IMAP-Server abrufen und anzeigen — plus Konto-Verwaltung im UI.

**Architektur:** ImapVerbindung bekommt SELECT+FETCH. Datenbank speichert Nachrichten. KontoDienst wird ans UI gebunden. Startup: Konto wählen → IMAP verbinden → Nachrichten laden.

**Tech-Stack:** Qt 6.4.2, QML, C++17, CMake+Ninja, QtTest+Mock-Sockets.

**Stand:** ✅ IMAP (LOGIN, LIST), ✅ SMTP (EHLO–DATA), ✅ QML-Fenster verdrahtet, ✅ PostfachDienst, ❌ SELECT+FETCH, ❌ Konto-UI.

---

## Phase 1: IMAP SELECT + FETCH (Nachrichten abrufen)

### Task 1: ImapVerbindung — befehleAuswaehlen() + nachrichtAbrufen() hinzufügen

**Ziel:** ImapVerbindung kann Ordner selektieren und Nachrichten-Header abrufen.

**Dateien:**
- Ändern: `quelltext/bibliothek/protokoll/imap_verbindung.h`
- Ändern: `quelltext/bibliothek/protokoll/imap_verbindung.cpp`

**Schritt 1: Header erweitern — neue Slots und Signale**

```cpp
public slots:
    void ordnerAuswaehlen(const QString &ordnerName);
    void nachrichtenHeaderAbrufen(int von, int bis);  // FETCH von:bis (FLAGS BODY.PEEK[HEADER.FIELDS (FROM SUBJECT DATE)])

signals:
    void ordnerAusgewaehlt(int nachrichtenZaehler);
    void nachrichtHeaderEmpfangen(const Kern::Nachricht &nachricht);
    void nachrichtenHeaderFertig();
```

**Schritt 2: Neue Befehls-Phasen (enum erweitern)**

```cpp
enum class Befehl {
    Keiner, Verbinden, Anmelden, OrdnerListe,
    OrdnerAuswaehlen, NachrichtenHeader, Logout
};
```

**Schritt 3: Implementierung**

- `ordnerAuswaehlen()` → sendet `SELECT "INBOX"`, parsed `* 5 EXISTS` → emit `ordnerAusgewaehlt(5)`
- `nachrichtenHeaderAbrufen(1, 20)` → sendet `FETCH 1:20 (FLAGS BODY.PEEK[HEADER.FIELDS (FROM SUBJECT DATE)])`
- Parsing: `* 1 FETCH (FLAGS (\Seen) BODY[HEADER.FIELDS (...)] {…}` → Nachricht-Struct füllen
- Jede empfangene Nachricht → emit `nachrichtHeaderEmpfangen(msg)`
- `TAG OK FETCH completed` → emit `nachrichtenHeaderFertig()`

**Schritt 4: Test in tst_imap_verbindung erweitern**

Mock-Server antwortet auf SELECT und FETCH mit realistischen Daten.

```cpp
void sollteOrdnerAuswaehlen() {
    // Mock antwortet: * 3 EXISTS\r\n... TAG OK SELECT completed
    // QVERIFY(ordnerAusgewaehlt.count() == 1)
    // QCOMPARE(zähler, 3)
}

void sollteNachrichtenHeaderAbrufen() {
    // Mock sendet 2 FETCH-Antworten
    // QCOMPARE(nachrichtHeaderEmpfangen.count(), 2)
    // Erste Nachricht: betreff, absender, datum prüfen
}
```

**Schritt 5: Bauen, testen, commit**

---

### Task 2: ImapVerbindung — nachrichtInhaltAbrufen()

**Ziel:** Einzelne Nachricht komplett abrufen (BODY[TEXT] oder BODY[]).

**Dateien:**
- Ändern: `quelltext/bibliothek/protokoll/imap_verbindung.h`
- Ändern: `quelltext/bibliothek/protokoll/imap_verbindung.cpp`

```cpp
public slots:
    void nachrichtInhaltAbrufen(int uid);

signals:
    void nachrichtInhaltEmpfangen(int uid, const QString &inhalt);
```

Befehl: `FETCH <uid> BODY[TEXT]` — parsed die mehrzeilige Antwort (Literal `{…}`).

**Test:** Mock sendet Text-Body, prüfen dass Signal mit korrektem Inhalt feuert.

---

## Phase 2: Datenbank — Nachrichten speichern

### Task 3: Datenbank um Nachrichten-Tabelle erweitern

**Ziel:** Nachrichten werden lokal in SQLite gespeichert.

**Dateien:**
- Ändern: `quelltext/bibliothek/speicher/datenbank.h`
- Ändern: `quelltext/bibliothek/speicher/datenbank.cpp`

```cpp
// Neue Methoden
qint64 nachrichtSpeichern(const Kern::Nachricht &nachricht);
QVector<Kern::Nachricht> nachrichtenFuerOrdner(const QString &ordner) const;
bool nachrichtAlsGelesenMarkieren(qint64 id);
void nachrichtenLoeschenFuerOrdner(const QString &ordner);
```

Tabelle `nachrichten`:
```sql
CREATE TABLE IF NOT EXISTS nachrichten (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    konto_id INTEGER NOT NULL,
    ordner TEXT NOT NULL DEFAULT 'INBOX',
    uid INTEGER,
    absender TEXT, betreff TEXT, inhalt TEXT, inhalt_html TEXT,
    datum TEXT, gelesen INTEGER DEFAULT 0, hat_anhaenge INTEGER DEFAULT 0,
    FOREIGN KEY (konto_id) REFERENCES konten(id)
);
```

**Test in tst_datenbank:** CRUD für Nachrichten, Ordnername-Filter, Löschen.

---

### Task 4: PostfachDienst — nachrichtenLaden() implementieren

**Ziel:** PostfachDienst::nachrichtenLaden() ruft IMAP FETCH auf und speichert in Datenbank+Cache.

**Dateien:**
- Ändern: `quelltext/bibliothek/dienst/postfach_dienst.h`
- Ändern: `quelltext/bibliothek/dienst/postfach_dienst.cpp`

```cpp
void PostfachDienst::nachrichtenLaden(const QString &ordner) {
    // 1. ImapVerbindung::ordnerAuswaehlen(ordner)
    // 2. ImapVerbindung::nachrichtenHeaderAbrufen(1, anzahl)
    // 3. Jede Nachricht → Datenbank + Cache
    // 4. nachrichtenGeaendert() emit
}
```

---

## Phase 3: SMTP mit echten Daten

### Task 5: SmtpVerbindung — echte Absender/Empfänger/Betreff/Inhalt

**Ziel:** SMTP sendet mit den übergebenen Parametern, nicht mit Dummy-Werten.

**Dateien:**
- Ändern: `quelltext/bibliothek/protokoll/smtp_verbindung.h`
- Ändern: `quelltext/bibliothek/protokoll/smtp_verbindung.cpp`

Aktuell: `sende()` ignoriert Parameter. Stattdessen:
- `MAIL FROM:<absender>`
- `RCPT TO:<empfaenger>` (für jeden)
- `DATA` — Betreff + Inhalt als RFC822

**Base64-Auth:** Benutzer/Passwort aus `sende()`-Parametern codiert senden (statt Dummy `dXNlcg==`).

Dafür braucht `sende()` zusätzlich Benutzer+Passwort-Parameter. Oder: SmtpVerbindung bekommt `setzeAnmeldedaten()` vor `sende()`.

---

## Phase 4: Konto-Verwaltung im UI

### Task 6: KontoAnsichtModell — ViewModel für Konto-Dialog

**Ziel:** QML-Dialog zum Anlegen/Bearbeiten eines Kontos.

**Dateien:**
- Neu: `quelltext/oberflaeche/ansichtmodelle/konto_ansicht_modell.h`
- Neu: `quelltext/oberflaeche/ansichtmodelle/konto_ansicht_modell.cpp`

Q_PROPERTY für: email, name, imapServer, imapPort, smtpServer, smtpPort, benutzer, passwort.

Q_INVOKABLE `speichern()` → emit `kontoGespeichert(Konto)`.

---

### Task 7: KontoDialog.qml — QML-Maske

**Ziel:** QML-Dialog mit TextField für alle Konto-Felder + Speichern/Abbrechen.

**Dateien:**
- Neu: `quelltext/oberflaeche/qml/KontoDialog.qml`

Ähnlich wie ErstellenAnsicht — TextField für jedes Feld, Button "Speichern".

---

### Task 8: KontoAuswahlModell — ViewModel für Konto-Auswahl beim Start

**Ziel:** Wenn mehrere Konten existieren, zeigt die App eine Auswahlliste.

**Dateien:**
- Neu: `quelltext/oberflaeche/ansichtmodelle/konto_auswahl_modell.h`
- Neu: `quelltext/oberflaeche/ansichtmodelle/konto_auswahl_modell.cpp`

Einfaches QStringListModel mit Konto-Namen/Emails.

---

## Phase 5: Startup-Flow

### Task 9: main.cpp — Startup-Flow: Konto wählen → verbinden → Nachrichten laden

**Ziel:** App-Start: Wenn Konten existieren → erstes Konto wählen → IMAP verbinden → Ordnerliste laden → erster Ordner (INBOX) selektieren → Nachrichten anzeigen.

**Dateien:**
- Ändern: `quelltext/anwendung/main.cpp`

```cpp
// In main():
auto *kontoDienst = new KontoDienst(&datenbank, &anwendung);
auto konten = kontoDienst->alleKonten();

if (!konten.isEmpty()) {
    auto &k = konten[0];
    auto *imap = new ImapVerbindung(&anwendung);
    imap->setzeServer(k.imapServer);
    imap->setzePort(k.imapPort);
    postfachDienst->setzeImapVerbindung(imap);

    connect(imap, &ImapVerbindung::verbunden, [imap, k]() {
        imap->anmelden(k.benutzer, k.passwort);
    });
    connect(imap, &ImapVerbindung::angemeldet, [postfachDienst]() {
        postfachDienst->ordnerLaden();
    });
    // ... weitere Signale ...
    imap->verbinden();
}
```

---

## Phase 6: Aufräumen

### Task 10: AGENTS.md aktualisieren

**Ziel:** AGENTS.md zeigt aktuellen Stand und nächste Schritte.

**Dateien:**
- Ändern: `AGENTS.md`

Neue Einträge unter ✅ und "Was als Nächstes".

---

## Zusammenfassung: Reihenfolge

| # | Task | Phase |
|---|------|-------|
| 1 | ImapVerbindung: SELECT + FETCH-Header | IMAP |
| 2 | ImapVerbindung: FETCH BODY (Inhalt) | IMAP |
| 3 | Datenbank: Nachrichten-Tabelle | Speicher |
| 4 | PostfachDienst: nachrichtenLaden() | Dienst |
| 5 | SmtpVerbindung: echte Parameter | SMTP |
| 6 | KontoAnsichtModell | UI |
| 7 | KontoDialog.qml | UI |
| 8 | KontoAuswahlModell | UI |
| 9 | main.cpp: Startup-Flow | Integration |
| 10 | AGENTS.md aktualisieren | Doku |

**MVP nach Task 4:** App lädt echte Nachrichten von IMAP-Server (mit Mock testbar).  
**MVP nach Task 9:** App kann mit echtem Server verbinden und E-Mails senden/empfangen.
