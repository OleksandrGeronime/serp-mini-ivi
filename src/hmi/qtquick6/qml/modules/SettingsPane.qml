import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 1.15

Item {
    Layout.fillWidth: true
    Layout.fillHeight: true

    component OptionRow: RowLayout {
        property string label: ""
        property var options: []
        property string current: ""
        property var onSelect: function(v) {}
        spacing: 8
        Label { text: label; color: "#8096a6"; font.pixelSize: 13; Layout.preferredWidth: 100 }
        Repeater {
            model: options
            Button {
                text: modelData
                highlighted: current === modelData
                onClicked: onSelect(modelData)
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Label { text: "SETTINGS"; color: "#8096a6"; font.pixelSize: 13; font.letterSpacing: 3 }

        Rectangle {
            Layout.fillWidth: true
            color: "#1c2530"; radius: 12
            ColumnLayout {
                width: parent.width - 32
                spacing: 16

                OptionRow {
                    label: "Theme"
                    options: ["light", "dark", "auto"]
                    current: iCarUserManager.theme
                    onSelect: function(v) { iCarUserManager.setTheme(v) }
                }
                OptionRow {
                    label: "Units"
                    options: ["metric", "imperial"]
                    current: iCarUserManager.units
                    onSelect: function(v) { iCarUserManager.setUnits(v) }
                }
                OptionRow {
                    label: "Language"
                    options: ["en", "de", "fr", "uk"]
                    current: iCarUserManager.language
                    onSelect: function(v) { iCarUserManager.setLanguage(v) }
                }

                // Brightness slider
                RowLayout {
                    spacing: 8
                    Label { text: "Brightness"; color: "#8096a6"; font.pixelSize: 13; Layout.preferredWidth: 100 }
                    Slider {
                        id: brightnessSlider
                        Layout.fillWidth: true
                        from: 0; to: 100; stepSize: 5
                        value: iCarUserManager.brightness
                        onMoved: iCarUserManager.setBrightness(Math.round(value))
                    }
                    Label {
                        text: iCarUserManager.brightness + "%"
                        color: "white"; font.pixelSize: 13; Layout.preferredWidth: 40
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
