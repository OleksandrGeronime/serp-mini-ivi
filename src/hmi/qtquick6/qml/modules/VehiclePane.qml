import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 1.15

Item {
    Layout.fillWidth: true
    Layout.fillHeight: true

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 8

        Text {
            text: "Vehicle"
            color: "white"
            font.pixelSize: 28
            font.bold: true
        }

        Text { text: "gear: " + iCarPropertyService.gear; color: "white" }
        Text { text: "speed: " + iCarPropertyService.speed; color: "white" }
        Text { text: "ignition: " + iCarPropertyService.ignition; color: "white" }
        Text { text: "moving: " + iCarPropertyService.moving; color: "white" }
        Text { text: "odometer: " + iCarPropertyService.odometer; color: "white" }
        Text { text: "fuelLevel: " + iCarPropertyService.fuelLevel; color: "white" }
    }
}
