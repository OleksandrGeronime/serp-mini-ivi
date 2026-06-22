import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "modules"

ApplicationWindow {
    id: root
    width: 1180
    height: 720
    visible: true
    title: "SERP IVI Mini Platform"
    color: "#101418"

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            height: 64
            color: "#161d23"
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 24
                anchors.rightMargin: 24
                Label {
                    text: "SERP IVI"
                    color: "white"
                    font.pixelSize: 24
                    font.bold: true
                }
                Label {
                    text: hmi.vehicleLine
                    color: "#b8c4cc"
                    font.pixelSize: 16
                    Layout.leftMargin: 20
                }
                Item { Layout.fillWidth: true }
                Button {
                    text: hmi.seat
                    onClicked: hmi.toggleSeat()
                }
                Button {
                    text: "Refresh"
                    onClicked: hmi.refresh()
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            NavRail {
                Layout.preferredWidth: 210
                Layout.fillHeight: true
            }

            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: ["home", "media", "radio", "navigation", "phone", "climate", "settings", "profile", "vehicle", "notifications"].indexOf(hmi.screen)

                HomePane {}
                MediaPane {}
                MediaPane {
                    radioMode: true
                }
                NavigationPane {}
                PhonePane {}
                ClimatePane {}
                SettingsPane {}
                SettingsPane {
                    profileMode: true
                }
                VehiclePane {}
                SettingsPane {
                    notificationsMode: true
                }
            }
        }

        ActionBar {
            Layout.fillWidth: true
            Layout.preferredHeight: 122
        }
    }
}
