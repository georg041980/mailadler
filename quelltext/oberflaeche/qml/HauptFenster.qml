import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: hauptFenster
    title: "AdlerMail"
    width: 1024
    height: 700
    visible: true

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.preferredWidth: 220
            Layout.fillHeight: true
            color: "#f0f0f0"

            ListView {
                anchors.fill: parent
                anchors.margins: 4
                model: ["INBOX", "Gesendet", "Entwürfe", "Papierkorb"]
                delegate: Text {
                    text: modelData
                    font.pixelSize: 14
                    padding: 8
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            NachrichtenListe {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
}
