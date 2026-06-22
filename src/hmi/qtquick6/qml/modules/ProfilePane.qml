import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Label { text: "DRIVER PROFILE"; color: "#8096a6"; font.pixelSize: 13; font.letterSpacing: 3 }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 16

            // Avatar + profile info
            Rectangle {
                Layout.preferredWidth: 220
                Layout.fillHeight: true
                color: "#1c2530"; radius: 12

                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 16

                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        width: 96; height: 96; radius: 48
                        color: "#5294e2"
                        Label {
                            anchors.centerIn: parent
                            text: iCarUserManager.currentUser.length > 0 ? iCarUserManager.currentUser.charAt(0).toUpperCase() : "?"
                            color: "white"; font.pixelSize: 44; font.bold: true
                        }
                    }

                    Label {
                        text: iCarUserManager.currentUser
                        color: "#d7e0e6"; font.pixelSize: 24; font.bold: true
                        Layout.alignment: Qt.AlignHCenter
                    }

                    Label {
                        text: "Seat: " + iCarUserManager.activeSeat
                        color: "#8096a6"; font.pixelSize: 15
                        Layout.alignment: Qt.AlignHCenter
                    }

                    RowLayout {
                        spacing: 8; Layout.alignment: Qt.AlignHCenter

                        Rectangle {
                            width: 80; height: 36; radius: 8
                            color: iCarUserManager.activeSeat === "driver" ? "#5294e2" : "#151b20"
                            border.color: iCarUserManager.activeSeat === "driver" ? "#5294e2" : "#2c3944"
                            Label { anchors.centerIn: parent; text: "Driver"; color: "white"; font.pixelSize: 13 }
                            MouseArea { anchors.fill: parent; onClicked: iCarUserManager.switchSeat("driver") }
                        }
                        Rectangle {
                            width: 80; height: 36; radius: 8
                            color: iCarUserManager.activeSeat === "codriver" ? "#5294e2" : "#151b20"
                            border.color: iCarUserManager.activeSeat === "codriver" ? "#5294e2" : "#2c3944"
                            Label { anchors.centerIn: parent; text: "CoDriver"; color: "white"; font.pixelSize: 13 }
                            MouseArea { anchors.fill: parent; onClicked: iCarUserManager.switchSeat("codriver") }
                        }
                    }
                }
            }

            // Profile list + actions
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#1c2530"; radius: 12

                ColumnLayout {
                    anchors.fill: parent; anchors.margins: 20; spacing: 12

                    Label { text: "PROFILES"; color: "#8096a6"; font.pixelSize: 12; font.letterSpacing: 2 }

                    Repeater {
                        model: iCarUserManager.userList
                        Rectangle {
                            Layout.fillWidth: true; height: 54; radius: 10
                            color: iCarUserManager.currentUser === modelData ? "#1c3040" : "#151b20"
                            border.color: iCarUserManager.currentUser === modelData ? "#5294e2" : "#2c3944"
                            border.width: iCarUserManager.currentUser === modelData ? 2 : 1

                            RowLayout {
                                anchors.fill: parent; anchors.leftMargin: 16; anchors.rightMargin: 16

                                Rectangle {
                                    width: 36; height: 36; radius: 18
                                    color: iCarUserManager.currentUser === modelData ? "#5294e2" : "#2c3944"
                                    Label {
                                        anchors.centerIn: parent; font.pixelSize: 16; font.bold: true
                                        text: modelData.charAt(0).toUpperCase(); color: "white"
                                    }
                                }

                                Label {
                                    text: modelData; color: "#d7e0e6"; font.pixelSize: 17
                                    Layout.fillWidth: true; leftPadding: 8
                                }

                                Label {
                                    visible: iCarUserManager.currentUser === modelData
                                    text: "ACTIVE"; color: "#5294e2"; font.pixelSize: 12; font.letterSpacing: 1
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: iCarUserManager.loadProfile(modelData)
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }

                    Label {
                        text: "Profile settings applied: theme / climate / media"
                        color: "#8096a6"; font.pixelSize: 12
                        wrapMode: Text.WordWrap; Layout.fillWidth: true
                    }
                }
            }
        }
    }
}
