import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 1.15

Item {
    Layout.fillWidth: true
    Layout.fillHeight: true

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Label { text: "RADIO"; color: "#8096a6"; font.pixelSize: 13; font.letterSpacing: 3 }

        // Station info
        Rectangle {
            Layout.fillWidth: true
            height: 100
            color: "#1c2530"; radius: 12
            ColumnLayout {
                anchors.centerIn: parent
                spacing: 4
                Label {
                    text: iCarRadioManager.station || "—"
                    color: "white"; font.pixelSize: 36; font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }
                Label {
                    text: iCarRadioManager.name || "No signal"
                    color: "#8096a6"; font.pixelSize: 14
                    Layout.alignment: Qt.AlignHCenter
                }
                Label {
                    text: "Signal: " + iCarRadioManager.signal
                    color: iCarRadioManager.signal > 0 ? "#8fd19e" : "#8096a6"; font.pixelSize: 12
                    Layout.alignment: Qt.AlignHCenter
                }
            }
        }

        // Seek
        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            Button {
                Layout.fillWidth: true; text: "◄◄ Seek"
                onClicked: iCarRadioManager.seek("backward")
            }
            Button {
                Layout.fillWidth: true; text: "Seek ►►"
                onClicked: iCarRadioManager.seek("forward")
            }
        }

        // Tune input
        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            TextField {
                id: freqField
                Layout.fillWidth: true
                placeholderText: "Frequency (e.g. 87.5)"
                color: "white"
                background: Rectangle { color: "#1c2530"; radius: 8; border.color: "#2c3944" }
            }
            Button {
                text: "Tune"
                onClicked: { if (freqField.text) iCarRadioManager.tune(freqField.text) }
            }
        }

        // FM presets
        Label { text: "PRESETS"; color: "#8096a6"; font.pixelSize: 11; font.letterSpacing: 2 }
        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            Repeater {
                model: ["87.5", "89.3", "101.1", "104.7", "107.3"]
                Button {
                    Layout.fillWidth: true
                    text: modelData
                    highlighted: iCarRadioManager.station === modelData
                    onClicked: iCarRadioManager.tune(modelData)
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
