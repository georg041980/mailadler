import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    color: "white"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 8

        Text { text: "Neue Nachricht"; font.pixelSize: 18; font.bold: true }

        RowLayout {
            Text { text: "An:"; Layout.preferredWidth: 60 }
            TextField { Layout.fillWidth: true; placeholderText: "empfaenger@beispiel.de" }
        }
        RowLayout {
            Text { text: "Betreff:"; Layout.preferredWidth: 60 }
            TextField { Layout.fillWidth: true; placeholderText: "Betreff" }
        }

        Rectangle { Layout.preferredHeight: 1; Layout.fillWidth: true; color: "#ddd" }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            TextArea {
                placeholderText: "Nachrichtentext..."
                wrapMode: TextEdit.WordWrap
            }
        }

        Button {
            text: "Senden"
            Layout.alignment: Qt.AlignRight
        }
    }
}
