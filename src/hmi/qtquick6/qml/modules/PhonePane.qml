import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 1.15

Item {
    Layout.fillWidth: true
    Layout.fillHeight: true

    // callState: 0=idle, 1=ringing, 2=active
    readonly property bool callActive: iCarPhoneManager.currentCallState === 2
    readonly property bool ringing:    iCarPhoneManager.currentCallState === 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Label { text: "PHONE"; color: "#8096a6"; font.pixelSize: 13; font.letterSpacing: 3 }

        // Active call / incoming call card
        Rectangle {
            Layout.fillWidth: true
            height: 100
            color: ringing ? "#1c3020" : (callActive ? "#1c2030" : "#1c2530")
            radius: 12
            ColumnLayout {
                anchors.centerIn: parent; spacing: 4
                Label {
                    text: iCarPhoneManager.contactName || iCarPhoneManager.phoneNumber || "—"
                    color: "white"; font.pixelSize: 22; font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }
                Label {
                    text: ringing ? "Incoming call..." : (callActive ? "On call" : "Idle")
                    color: ringing ? "#8fd19e" : (callActive ? "#5294e2" : "#8096a6")
                    font.pixelSize: 14
                    Layout.alignment: Qt.AlignHCenter
                }
            }
        }

        // Call controls
        RowLayout {
            Layout.fillWidth: true; spacing: 12
            Button {
                Layout.fillWidth: true; text: "✓ Accept"
                visible: ringing
                onClicked: iCarPhoneManager.acceptCall()
            }
            Button {
                Layout.fillWidth: true; text: "✕ End"
                visible: callActive || ringing
                onClicked: iCarPhoneManager.endCall()
            }
        }

        // Dial
        RowLayout {
            Layout.fillWidth: true; spacing: 8
            TextField {
                id: dialField
                Layout.fillWidth: true
                placeholderText: "Phone number"
                color: "white"
                background: Rectangle { color: "#1c2530"; radius: 8; border.color: "#2c3944" }
            }
            Button {
                text: "Dial"
                enabled: !callActive
                onClicked: { if (dialField.text) iCarPhoneManager.dial(dialField.text) }
            }
        }

        // Contacts
        Label { text: "CONTACTS"; color: "#8096a6"; font.pixelSize: 11; font.letterSpacing: 2 }
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: iCarPhoneManager.contacts
            delegate: Rectangle {
                width: ListView.view.width; height: 44; radius: 8
                color: index % 2 === 0 ? "#151b20" : "#1c2530"
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 12
                    Label { text: modelData; color: "#d7e0e6"; font.pixelSize: 15; Layout.fillWidth: true }
                    Button {
                        text: "Call"; height: 30
                        onClicked: iCarPhoneManager.dial(modelData)
                    }
                }
            }
        }
    }
}
