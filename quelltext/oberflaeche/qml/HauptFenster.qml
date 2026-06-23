import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: hauptFenster
    title: "MailAdler"
    width: 1100
    height: 700
    visible: true

    property bool darkMode: false
    property color bgFarbe: darkMode ? "#1e1e1e" : "#ffffff"
    property color sidebarFarbe: darkMode ? "#252525" : "#f5f5f5"
    property color toolbarFarbe: darkMode ? "#2d2d2d" : "#fafafa"
    property color textFarbe: darkMode ? "#e0e0e0" : "#333333"
    property color borderFarbe: darkMode ? "#444" : "#ddd"
    property color statusFarbe: darkMode ? "#333" : "#e8e8e8"

    property string letzterFehler: ""

    Connections {
        target: postfachDienst
        function onFehlerAufgetreten(meldung) { letzterFehler = meldung; fehlerDialog.open() }
    }

    Dialog {
        id: fehlerDialog; title: "Fehler"; anchors.centerIn: parent
        Text { text: letzterFehler; wrapMode: Text.WordWrap; width: 300 }
        standardButtons: Dialog.Ok
    }

    ColumnLayout {
        anchors.fill: parent; spacing: 0

        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 24
            color: statusFarbe; border.color: "#ccc"
            RowLayout {
                anchors.fill: parent; anchors.margins: 4
                Text {
                    text: "Verbunden · " + ordnerListeModell.rowCount() + " Ordner · "
                          + nachrichtenListeModell.rowCount() + " Nachrichten"
                    font.pixelSize: 11; color: textFarbe
                }
                Item { Layout.fillWidth: true }
                BusyIndicator { id: spinner; running: false; Layout.preferredWidth: 16; Layout.preferredHeight: 16; visible: running }
            }
        }

        RowLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; spacing: 0

            Rectangle {
                Layout.preferredWidth: 180; Layout.fillHeight: true
                color: sidebarFarbe; border.color: borderFarbe
                ColumnLayout {
                    anchors.fill: parent; anchors.margins: 4; spacing: 2
                    Text { text: "Ordner"; font.pixelSize: 12; font.bold: true; color: darkMode ? "#aaa" : "#888"; Layout.margins: 4 }
                    ListView {
                        id: ordnerListe; Layout.fillWidth: true; Layout.fillHeight: true
                        model: ordnerListeModell
                        delegate: Text {
                            text: model.display; font.pixelSize: 13; padding: 6; color: textFarbe; width: ListView.view.width
                            MouseArea {
                                anchors.fill: parent
                                onClicked: { spinner.running = true; postfachDienst.nachrichtenLaden(model.display) }
                            }
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.preferredWidth: 350; Layout.fillHeight: true; spacing: 0
                Rectangle {
                    Layout.fillWidth: true; Layout.preferredHeight: 36
                    color: toolbarFarbe; border.color: borderFarbe
                    RowLayout {
                        anchors.fill: parent; anchors.margins: 4
                        Text { text: "Posteingang"; font.pixelSize: 14; font.bold: true; color: textFarbe; Layout.fillWidth: true }
                        ComboBox {
                            model: kontoAuswahlModell; textRole: "display"
                            Layout.preferredWidth: 180
                            onCurrentIndexChanged: kontoAuswahlModell.beiIndexGeaendert(currentIndex)
                        }
                        Button {
                            text: darkMode ? "☀" : "☾"; font.pixelSize: 16
                            onClicked: darkMode = !darkMode
                        }
                        Button {
                            text: "↻"; font.pixelSize: 16
                            onClicked: { spinner.running = true; postfachDienst.ordnerLaden() }
                        }
                    }
                }
                NachrichtenListe { Layout.fillWidth: true; Layout.fillHeight: true }
            }

            NachrichtAnsicht { Layout.fillWidth: true; Layout.fillHeight: true }
        }
    }

    Connections {
        target: postfachDienst
        function onNachrichtenGeaendert() { spinner.running = false }
        function onOrdnerListeGeaendert()   { spinner.running = false }
    }

    // Benachrichtigungs-Banner
    Rectangle {
        id: benachrichtigung
        anchors.horizontalCenter: parent.horizontalCenter
        y: -60
        width: 400; height: 48; radius: 8
        color: darkMode ? "#2d8a4e" : "#4caf50"
        visible: false; opacity: 0

        Text {
            anchors.centerIn: parent
            text: "📬 Neue Nachricht eingetroffen!"
            color: "white"; font.pixelSize: 14; font.bold: true
        }

        Behavior on y { NumberAnimation { duration: 300; easing.type: Easing.OutCubic } }
        Behavior on opacity { NumberAnimation { duration: 300 } }

        function zeige() {
            y = 10; opacity = 1; visible = true
            zeitgeber.restart()
        }

        Timer {
            id: zeitgeber; interval: 3000
            onTriggered: { benachrichtigung.y = -60; benachrichtigung.opacity = 0; benachrichtigung.visible = false }
        }
    }

    Connections {
        target: postfachDienst
        function onNeueNachricht() { benachrichtigung.zeige() }
    }
}
