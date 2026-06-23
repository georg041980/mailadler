# QML/UI-Verdrahtung — Implementierungsplan

> **Für Hermes:** Nutze den `test-driven-development`-Skill, um diesen Plan TDD-basiert umzusetzen.

**Ziel:** Die QML-Views mit den C++-ViewModels verdrahten. Aus Stubs werden funktionale Oberflächen — Nachrichtenliste zeigt echte Daten, Erstellen-Ansicht validiert Eingaben.

**Architektur:** QML bindet an C++-ViewModels via `Q_PROPERTY`. Keine JavaScript-Logik in QML. Alle Entscheidungen liegen im ViewModel.

**Vorbedingung:** Pläne ①–⑤ abgeschlossen. `NachrichtenListeModell` und `ErstellenAnsichtModell` existieren als Stubs.

---

## Phase 1: NachrichtenListeModell vervollständigen

### Aufgabe 1: QML an ViewModel binden

**Ziel:** `NachrichtenListe.qml` zeigt echte Daten aus `NachrichtenListeModell`.

**Dateien:**
- Prüfen/Bearbeiten: `quelltext/oberflaeche/ansichtmodelle/nachrichten_liste_modell.h/.cpp`
- Bearbeiten: `quelltext/oberflaeche/qml/NachrichtenListe.qml`
- Bearbeiten: `quelltext/oberflaeche/qml/HauptFenster.qml`

**Schritt 1: ViewModel prüfen**

Das `NachrichtenListeModell` hat bereits `Q_PROPERTY`-Rollen (`betreff`, `absender`, `datum`, `gelesen`). Prüfen ob `roleNames()` korrekt ist.

**Schritt 2: QML einbinden**

In `HauptFenster.qml`:
```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: hauptFenster
    title: "MailAdler"
    width: 1024
    height: 700
    visible: true

    // ViewModel aus C++ registriert
    property var nachrichtenModell: null

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Ordnerleiste
        Rectangle {
            Layout.preferredWidth: 220
            Layout.fillHeight: true
            color: "#f0f0f0"

            ListView {
                anchors.fill: parent
                anchors.margins: 4
                model: ["INBOX", "Gesendet", "Entwürfe", "Papierkorb"]
                delegate: ItemDelegate {
                    text: modelData
                    onClicked: console.log("Ordner gewählt:", modelData)
                }
            }
        }

        // Nachrichtenliste
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            NachrichtenListe {
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: hauptFenster.nachrichtenModell
            }
        }
    }
}
```

**Schritt 3: `main.cpp` — ViewModel registrieren**

```cpp
#include "ansichtmodelle/nachrichten_liste_modell.h"

// Vor engine.load():
auto *modell = new MailAdler::NachrichtenListeModell(&anwendung);
maschine.rootContext()->setContextProperty("nachrichtenListeModell", modell);
```

**Schritt 4: Test-Daten einspeisen (in main.cpp)**

```cpp
QVector<Kern::Nachricht> testDaten;
Kern::Nachricht n;
n.id = 1; n.betreff = "Willkommen bei MailAdler";
n.absender = "mailadler@nousresearch.com";
n.datum = QDateTime::currentDateTime();
n.gelesen = false;
testDaten.append(n);
modell->setzeNachrichten(testDaten);
```

**Bauen + visuell prüfen:**

```bash
cmake --build bau --parallel $(nproc)
./bau/quelltext/anwendung/mailadler
```

Erwartet: Fenster mit linker Ordnerleiste + Nachrichtenliste mit einem Eintrag.

**Commit:**

```bash
git add quelltext/oberflaeche/qml/HauptFenster.qml \
        quelltext/oberflaeche/qml/NachrichtenListe.qml \
        quelltext/anwendung/main.cpp
git commit -m "oberflaeche: NachrichtenListe mit echtem ViewModel verdrahtet"
```

---

## Phase 2: ErstellenAnsichtModell aktivieren

### Aufgabe 2: Verfassen-Fenster funktional machen

**Ziel:** Das Verfassen-Fenster validiert Eingaben und aktiviert den Senden-Button nur bei gültigen Daten.

**Dateien:**
- Prüfen: `quelltext/oberflaeche/ansichtmodelle/erstellen_ansicht_modell.h/.cpp`
- Bearbeiten: `quelltext/oberflaeche/qml/ErstellenAnsicht.qml`

**Schritt 1: ViewModel prüfen**

Das `ErstellenAnsichtModell` hat bereits:
- `Q_PROPERTY(QString an ...)`
- `Q_PROPERTY(QString betreff ...)`
- `Q_PROPERTY(QString inhalt ...)`
- `Q_PROPERTY(bool kannSenden ...)`

Logik: `kannSenden = !an.isEmpty() && !betreff.isEmpty()`.

**Schritt 2: QML binden**

`ErstellenAnsicht.qml`:
```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    color: "white"
    property var viewModell: null

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 8

        Text { text: "Neue Nachricht"; font.pixelSize: 18; font.bold: true }

        RowLayout {
            Text { text: "An:"; Layout.preferredWidth: 60 }
            TextField {
                Layout.fillWidth: true
                placeholderText: "empfaenger@beispiel.de"
                text: viewModell ? viewModell.an : ""
                onTextChanged: { if (viewModell) viewModell.an = text }
            }
        }
        RowLayout {
            Text { text: "Betreff:"; Layout.preferredWidth: 60 }
            TextField {
                Layout.fillWidth: true
                placeholderText: "Betreff"
                text: viewModell ? viewModell.betreff : ""
                onTextChanged: { if (viewModell) viewModell.betreff = text }
            }
        }

        Rectangle { Layout.preferredHeight: 1; Layout.fillWidth: true; color: "#ddd" }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            TextArea {
                placeholderText: "Nachrichtentext..."
                wrapMode: TextEdit.WordWrap
                text: viewModell ? viewModell.inhalt : ""
                onTextChanged: { if (viewModell) viewModell.inhalt = text }
            }
        }

        Button {
            text: "Senden"
            Layout.alignment: Qt.AlignRight
            enabled: viewModell ? viewModell.kannSenden : false
            onClicked: console.log("Senden:", viewModell.an, viewModell.betreff)
        }
    }
}
```

**Schritt 3: main.cpp erweitern**

```cpp
auto *erstellenModell = new MailAdler::ErstellenAnsichtModell(&anwendung);
maschine.rootContext()->setContextProperty("erstellenAnsichtModell", erstellenModell);
```

**Schritt 4: Neues QML-Fenster testen**

`main.cpp` temporär umstellen, um ErstellenAnsicht zu laden:
```cpp
maschine.load(QUrl("qrc:/MailAdler/ErstellenAnsicht.qml"));
```

**Bauen + prüfen:**

```bash
cmake --build bau --parallel $(nproc)
./bau/quelltext/anwendung/mailadler
```

Erwartet: Senden-Button grau → An+Betreff eintippen → Button aktiv.

**Commit:**

```bash
git add quelltext/oberflaeche/qml/ErstellenAnsicht.qml \
        quelltext/anwendung/main.cpp
git commit -m "oberflaeche: ErstellenAnsicht mit ViewModel verdrahtet"
```

---

## Phase 3: Nachricht lesen (Detail-Ansicht)

### Aufgabe 3: NachrichtAnsicht mit Daten füllen

**Ziel:** Klick auf Nachricht in Liste → Detail-Ansicht zeigt Inhalt.

**Dateien:**
- Bearbeiten: `quelltext/oberflaeche/qml/NachrichtAnsicht.qml`
- Bearbeiten: `quelltext/oberflaeche/qml/HauptFenster.qml`

**Schritt 1: NachrichtAnsicht.qml**

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: nachrichtAnsicht
    color: "white"

    property string nachrichtBetreff: ""
    property string nachrichtAbsender: ""
    property string nachrichtInhalt: ""

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 8

        Text {
            text: nachrichtBetreff || "(Kein Betreff)"
            font.pixelSize: 18
            font.bold: true
        }
        Text {
            text: "Von: " + nachrichtAbsender
            font.pixelSize: 13
            color: "#555555"
        }

        Rectangle { Layout.preferredHeight: 1; Layout.fillWidth: true; color: "#ddd" }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Text {
                text: nachrichtInhalt || "(Keine Nachricht ausgewählt)"
                font.pixelSize: 14
                wrapMode: Text.WordWrap
                textFormat: TextEdit.AutoText
            }
        }
    }
}
```

**Schritt 2: HauptFenster — Split-Layout**

```qml
SplitView {
    Layout.fillWidth: true
    Layout.fillHeight: true
    orientation: Qt.Vertical

    NachrichtenListe {
        SplitView.preferredHeight: parent.height * 0.4
        model: hauptFenster.nachrichtenModell
    }

    NachrichtAnsicht {
        SplitView.fillHeight: true
        nachrichtBetreff: "Test-Betreff"
        nachrichtAbsender: "test@mailadler.dev"
        nachrichtInhalt: "Das ist der Nachrichteninhalt.\n\nMehrere Zeilen."
    }
}
```

**Schritt 3: main.cpp — Test-Daten erweitern**

```cpp
// Nach dem setzeNachrichten-Aufruf:
maschine.rootContext()->setContextProperty("testNachricht", testDaten.first());
```

**Commit:**

```bash
git add quelltext/oberflaeche/qml/NachrichtAnsicht.qml \
        quelltext/oberflaeche/qml/HauptFenster.qml
git commit -m "oberflaeche: NachrichtAnsicht + SplitView-Layout"
```

---

## Zusammenfassung

| Phase | Ergebnis |
|---|---|
| 1 | Nachrichtenliste zeigt echte Daten aus ViewModel |
| 2 | Verfassen-Fenster validiert Eingaben |
| 3 | Detail-Ansicht zeigt Nachrichteninhalt |

**Nach diesem Plan:** Die UI ist funktional, aber mit Test-Daten. Der nächste Plan (Startup-Flow) verbindet IMAP+SMTP mit der UI.
