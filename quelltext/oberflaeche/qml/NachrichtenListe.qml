import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ListView {
    id: nachrichtenListe
    clip: true

    model: nachrichtenListeModell
    delegate: Rectangle {
        width: ListView.view.width
        height: 48
        color: index % 2 ? "#fafafa" : "#ffffff"
        border.color: "#e0e0e0"

        RowLayout {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 12

            Text {
                text: model.absender
                Layout.preferredWidth: 180
                font.pixelSize: 13
                font.bold: !model.gelesen
            }
            Text {
                text: model.betreff
                Layout.fillWidth: true
                font.pixelSize: 13
                font.bold: !model.gelesen
                elide: Text.ElideRight
            }
            Text {
                text: model.datum.toLocaleString()
                font.pixelSize: 12
                color: "#888888"
            }
        }
    }
}
