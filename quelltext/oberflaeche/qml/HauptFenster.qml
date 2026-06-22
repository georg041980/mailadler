import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: hauptFenster
    title: "AdlerMail"
    width: 1100
    height: 700
    visible: true

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // --- Ordnerleiste (links) ---
        Rectangle {
            Layout.preferredWidth: 180
            Layout.fillHeight: true
            color: "#f5f5f5"
            border.color: "#ddd"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 2

                Text {
                    text: "Ordner"
                    font.pixelSize: 12
                    font.bold: true
                    color: "#888"
                    Layout.margins: 4
                }

                ListView {
                    id: ordnerListe
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: ordnerListeModell
                    delegate: Text {
                        text: model.display
                        font.pixelSize: 13
                        padding: 6
                        color: "#333"
                        width: ListView.view.width

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                postfachDienst.nachrichtenLaden(model.display)
                            }
                        }
                    }
                }
            }
        }

        // --- Mittlere Spalte: Nachrichtenliste ---
        ColumnLayout {
            Layout.preferredWidth: 350
            Layout.fillHeight: true
            spacing: 0

            // Toolbar
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 36
                color: "#fafafa"
                border.color: "#ddd"

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 4
                    Text {
                        text: "Posteingang"
                        font.pixelSize: 14
                        font.bold: true
                        Layout.fillWidth: true
                    }
                    Button {
                        text: "↻"
                        font.pixelSize: 16
                        onClicked: { /* Aktualisieren (Task 15) */ }
                    }
                }
            }

            // Nachrichtenliste
            NachrichtenListe {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }

        // --- Rechte Spalte: Nachricht-Ansicht ---
        NachrichtAnsicht {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
