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

        // Header with unread count + dismiss all
        RowLayout {
            Layout.fillWidth: true
            Label { text: "NOTIFICATIONS"; color: "#8096a6"; font.pixelSize: 13; font.letterSpacing: 3; Layout.fillWidth: true }
            Label {
                visible: iCarNotificationManager.unreadCount > 0
                text: iCarNotificationManager.unreadCount + " unread"
                color: "#5294e2"; font.pixelSize: 13
            }
            Button {
                text: "Dismiss all"
                enabled: iCarNotificationManager.notifications && iCarNotificationManager.notifications.length > 0
                onClicked: iCarNotificationManager.dismissAll()
            }
        }

        // Notification list
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: iCarNotificationManager.notifications
            spacing: 6

            delegate: Rectangle {
                width: ListView.view.width
                height: 64; radius: 10
                color: "#1c2530"
                border.color: "#2c3944"; border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 16; anchors.rightMargin: 12
                    spacing: 12

                    Rectangle {
                        width: 8; height: 8; radius: 4
                        color: "#5294e2"
                    }

                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 2
                        Label {
                            text: (typeof modelData === "object") ? (modelData.title || JSON.stringify(modelData)) : modelData
                            color: "white"; font.pixelSize: 14; font.bold: true
                            elide: Text.ElideRight; Layout.fillWidth: true
                        }
                        Label {
                            visible: typeof modelData === "object" && !!modelData.category
                            text: (typeof modelData === "object") ? (modelData.category || "") : ""
                            color: "#8096a6"; font.pixelSize: 12
                        }
                    }

                    Button {
                        text: "✕"
                        onClicked: {
                            var id = (typeof modelData === "object") ? modelData.id : modelData
                            iCarNotificationManager.dismiss(id)
                        }
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                visible: !iCarNotificationManager.notifications || iCarNotificationManager.notifications.length === 0
                text: "No notifications"
                color: "#8096a6"; font.pixelSize: 16
            }
        }
    }
}
