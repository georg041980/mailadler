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
            text: nachrichtAnsichtModell.hatNachricht
                  ? "Betreff: " + nachrichtAnsichtModell.betreff
                  : ""
            font.pixelSize: 18
            font.bold: true
        }
        Text {
            text: nachrichtAnsichtModell.hatNachricht
                  ? "Von: " + nachrichtAnsichtModell.absender
                    + "  —  " + nachrichtAnsichtModell.datum
                  : ""
            font.pixelSize: 13
            color: "#555555"
        }

        Rectangle { Layout.preferredHeight: 1; Layout.fillWidth: true; color: "#ddd" }

        Text {
            text: nachrichtAnsichtModell.inhalt
            Layout.fillWidth: true
            Layout.fillHeight: true
            font.pixelSize: 14
            wrapMode: Text.WordWrap
        }
    }
}
