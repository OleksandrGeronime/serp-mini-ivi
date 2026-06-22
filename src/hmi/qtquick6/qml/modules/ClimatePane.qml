import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 1.15

Item {
    Layout.fillWidth: true
    Layout.fillHeight: true

    component ClimateButton: Button {
        Layout.fillWidth: true
        Layout.preferredHeight: 46
        font.pixelSize: 15
    }

    component ToggleTile: Button {
        id: toggleTile
        property bool active: false
        Layout.fillWidth: true
        Layout.preferredHeight: 62
        highlighted: active
        font.pixelSize: 15
        background: Rectangle {
            radius: 8
            color: toggleTile.active ? "#17304a" : "#1c2530"
            border.color: toggleTile.active ? "#4aa3ff" : "#2c3944"
            border.width: 1
        }
        contentItem: Label {
            text: toggleTile.text
            color: toggleTile.active ? "#d8efff" : "#d7dde3"
            font.pixelSize: 15
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }

    component TempZone: Rectangle {
        property string title: ""
        property string note: ""
        property string zone: ""
        property real temp: 0

        function applyDelta(delta) {
            if (zone === "driver") {
                iCarClimateManager.setDriverTemp(iCarClimateManager.driverTemp + delta)
            } else if (zone === "passenger") {
                iCarClimateManager.setPassengerTemp(iCarClimateManager.passengerTemp + delta)
            }
        }

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumHeight: 190
        radius: 10
        color: "#16202a"
        border.color: "#283746"
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 18
            spacing: 10

            Label {
                Layout.fillWidth: true
                text: title
                color: "#8fb3cc"
                font.pixelSize: 16
                elide: Text.ElideRight
            }

            Label {
                Layout.fillWidth: true
                visible: note.length > 0
                text: note
                color: "#6db0ff"
                font.pixelSize: 12
                elide: Text.ElideRight
            }

            Label {
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: temp.toFixed(1) + " deg"
                color: "white"
                font.pixelSize: 44
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                minimumPixelSize: 26
                fontSizeMode: Text.Fit
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                ClimateButton {
                    text: "-"
                    onClicked: applyDelta(-1)
                }

                ClimateButton {
                    text: "+"
                    onClicked: applyDelta(1)
                }
            }
        }
    }

    component FanLevel: Rectangle {
        property bool active: false

        Layout.fillWidth: true
        Layout.preferredHeight: 18
        radius: 5
        color: active ? "#4aa3ff" : "#24313d"
        border.color: active ? "#8cc9ff" : "#2f4050"
    }

    Rectangle {
        anchors.fill: parent
        color: "#0d1217"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 28
            spacing: 18

            RowLayout {
                Layout.fillWidth: true
                spacing: 16

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4

                    Label {
                        text: "CLIMATE"
                        color: "#8096a6"
                        font.pixelSize: 13
                        font.letterSpacing: 3
                    }

                    Label {
                        text: iCarClimateManager.autoOn ? "Automatic cabin control" : "Manual cabin control"
                        color: "#d7dde3"
                        font.pixelSize: 24
                        font.bold: true
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 150
                    Layout.preferredHeight: 44
                    radius: 8
                    color: iCarClimateManager.acOn ? "#123425" : "#1c2530"
                    border.color: iCarClimateManager.acOn ? "#35c46b" : "#2c3944"
                    Label {
                        anchors.centerIn: parent
                        text: iCarClimateManager.acOn ? "A/C ON" : "A/C OFF"
                        color: iCarClimateManager.acOn ? "#b9ffd2" : "#9aa8b4"
                        font.pixelSize: 14
                        font.bold: true
                    }
                }
            }

            GridLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                columns: 3
                columnSpacing: 18
                rowSpacing: 18

                TempZone {
                    title: "Driver"
                    zone: "driver"
                    temp: iCarClimateManager.driverTemp
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.minimumHeight: 190
                    radius: 10
                    color: "#16202a"
                    border.color: "#283746"
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 18
                        spacing: 12

                        RowLayout {
                            Layout.fillWidth: true
                            Label {
                                Layout.fillWidth: true
                                text: "Fan"
                                color: "#8fb3cc"
                                font.pixelSize: 16
                            }
                            Label {
                                text: iCarClimateManager.fanSpeed.toString()
                                color: "white"
                                font.pixelSize: 28
                                font.bold: true
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8
                            Repeater {
                                model: 7
                                FanLevel {
                                    active: index < iCarClimateManager.fanSpeed
                                }
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 10
                            ClimateButton {
                                text: "-"
                                onClicked: iCarClimateManager.setFanSpeed(Math.max(0, iCarClimateManager.fanSpeed - 1))
                            }
                            ClimateButton {
                                text: "+"
                                onClicked: iCarClimateManager.setFanSpeed(Math.min(7, iCarClimateManager.fanSpeed + 1))
                            }
                        }

                        Label {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            text: iCarClimateManager.syncOn ? "Zones synchronized" : "Independent zones"
                            color: "#9aa8b4"
                            font.pixelSize: 13
                            verticalAlignment: Text.AlignBottom
                            elide: Text.ElideRight
                        }
                    }
                }

                TempZone {
                    title: "Passenger"
                    note: iCarClimateManager.syncOn ? "Adjusting breaks sync" : ""
                    zone: "passenger"
                    temp: iCarClimateManager.passengerTemp
                }
            }

            GridLayout {
                Layout.fillWidth: true
                columns: 5
                uniformCellWidths: true
                columnSpacing: 12
                rowSpacing: 12

                ToggleTile { text: "A/C"; active: iCarClimateManager.acOn; onClicked: iCarClimateManager.toggleAc() }
                ToggleTile { text: "Sync"; active: iCarClimateManager.syncOn; onClicked: iCarClimateManager.toggleSync() }
                ToggleTile { text: "Auto"; active: iCarClimateManager.autoOn; onClicked: iCarClimateManager.toggleAuto() }
                ToggleTile { text: "Front Def"; active: iCarClimateManager.frontDefrost; onClicked: iCarClimateManager.toggleFrontDefrost() }
                ToggleTile { text: "Rear Def"; active: iCarClimateManager.rearDefrost; onClicked: iCarClimateManager.toggleRearDefrost() }
            }
        }
    }
}
