import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: hauptFenster
    title: "AdlerMail"
    width: 1100
    height: 700
    visible: true

    // Fehler-Dialog
    property string letzterFehler: ""

    Connections {
        target: postfachDienst
        function onFehlerAufgetreten(meldung) {
            letzterFehler = meldung
            fehlerDialog.open()
        }
    }

    Dialog {
        id: fehlerDialog
        title: "Fehler"
        anchors.centerIn: parent
        Text { text: letzterFehler; wrapMode: Text.WordWrap; width: 300 }
        standardButtons: Dialog.Ok
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // --- Statusleiste ---
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 24
            color: "#e8e8e8"
            border.color: "#ccc"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 4
                Text {
                    text: "Verbunden · " + ordnerListeModell.rowCount() + " Ordner · "
                          + nachrichtenListeModell.rowCount() + " Nachrichten"
                    font.pixelSize: 11
                    color: "#666"
                }
                Item { Layout.fillWidth: true }
                BusyIndicator {
                    id: spinner
                    running: false
                    Layout.preferredWidth: 16
                    Layout.preferredHeight: 16
                    visible: running
                }
            }
        }

        // --- Hauptbereich ---
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // Ordnerleiste
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
                                    spinner.running = true
                                    postfachDienst.nachrichtenLaden(model.display)
                                }
                            }
                        }
                    }
                }
            }

            // Mittlere Spalte: Nachrichtenliste
            ColumnLayout {
                Layout.preferredWidth: 350
                Layout.fillHeight: true
                spacing: 0

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
                            onClicked: {
                                spinner.running = true
                                postfachDienst.ordnerLaden()
                            }
                        }
                    }
                }

                NachrichtenListe {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }

            // Rechte Spalte: Nachricht-Ansicht
            NachrichtAnsicht {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }

    // Spinner aus nach Laden
    Connections {
        target: postfachDienst
        function onNachrichtenGeaendert() { spinner.running = false }
        function onOrdnerListeGeaendert()   { spinner.running = false }
    }
}
