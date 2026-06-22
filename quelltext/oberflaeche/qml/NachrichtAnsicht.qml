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
            text: nachrichtAnsichtModell.istHtml
                  ? formatRichText(nachrichtAnsichtModell.inhalt)
                  : nachrichtAnsichtModell.inhalt
            textFormat: nachrichtAnsichtModell.istHtml ? Text.RichText : Text.PlainText
            Layout.fillWidth: true
            Layout.fillHeight: true
            font.pixelSize: 14
            wrapMode: Text.WordWrap

            function formatRichText(html) {
                // Simpler HTML-Cleanup: body-Extrakt
                var b = html.indexOf("<body")
                if (b >= 0) {
                    var start = html.indexOf(">", b) + 1
                    var end = html.indexOf("</body>", start)
                    if (end > start) return html.substring(start, end)
                }
                return html
            }
        }
    }
}
