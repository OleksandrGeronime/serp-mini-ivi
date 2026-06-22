import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

GridLayout {
    columns: 2
    rowSpacing: 16
    columnSpacing: 16
    Layout.fillWidth: true
    Layout.fillHeight: true
    Layout.margins: 24

    GroupBox {
        title: "Now Playing"
        Layout.fillWidth: true
        Layout.fillHeight: true
        Label { anchors.fill: parent; color: "#d7e0e6"; text: "State: " + hmi.mediaState + "\nSource: " + hmi.mediaSource + "\nTrack: " + hmi.mediaTrack + "\nFocus: " + hmi.audioFocus; font.pixelSize: 20 }
    }
    GroupBox {
        title: "Route"
        Layout.fillWidth: true
        Layout.fillHeight: true
        Label { anchors.fill: parent; color: "#d7e0e6"; text: "State: " + hmi.navigationState + "\nDestination: " + hmi.navigationDestination; font.pixelSize: 20 }
    }
    GroupBox {
        title: "Cabin"
        Layout.fillWidth: true
        Layout.fillHeight: true
        Label { anchors.fill: parent; color: "#d7e0e6"; text: "Driver: " + hmi.climateDriver + " F\nPassenger: " + hmi.climatePassenger + " F\nFan: " + hmi.climateFan; font.pixelSize: 20 }
    }
    GroupBox {
        title: "Vehicle"
        Layout.fillWidth: true
        Layout.fillHeight: true
        Label { anchors.fill: parent; color: "#d7e0e6"; text: hmi.vehicleLine; font.pixelSize: 20 }
    }
}
