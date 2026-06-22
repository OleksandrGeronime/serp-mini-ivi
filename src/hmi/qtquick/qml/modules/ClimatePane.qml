import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

RowLayout {
    Layout.fillWidth: true
    Layout.fillHeight: true
    Layout.margins: 32
    spacing: 24
    GroupBox { title: "Driver"; Layout.fillWidth: true; Layout.fillHeight: true; Label { anchors.centerIn: parent; text: hmi.climateDriver + " F"; color: "#d7e0e6"; font.pixelSize: 42 } }
    GroupBox { title: "Fan"; Layout.fillWidth: true; Layout.fillHeight: true; Label { anchors.centerIn: parent; text: hmi.climateFan; color: "#d7e0e6"; font.pixelSize: 42 } }
    GroupBox { title: "Passenger"; Layout.fillWidth: true; Layout.fillHeight: true; Label { anchors.centerIn: parent; text: hmi.climatePassenger + " F"; color: "#d7e0e6"; font.pixelSize: 42 } }
}
