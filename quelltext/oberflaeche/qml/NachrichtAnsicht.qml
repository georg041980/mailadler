import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: nachrichtAnsicht
    color: "white"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 8

        Text {
            text: "Betreff: Beispielbetreff"
            font.pixelSize: 18
            font.bold: true
        }
        Text {
            text: "Von: max@beispiel.de"
            font.pixelSize: 13
            color: "#555555"
        }

        Rectangle { Layout.preferredHeight: 1; Layout.fillWidth: true; color: "#ddd" }

        Text {
            text: "Nachrichteninhalt erscheint hier..."
            Layout.fillWidth: true
            Layout.fillHeight: true
            font.pixelSize: 14
            wrapMode: Text.WordWrap
        }
    }
}
