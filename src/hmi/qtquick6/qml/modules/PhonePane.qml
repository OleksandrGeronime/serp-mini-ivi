import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 1.15

Item {
    Layout.fillWidth: true
    Layout.fillHeight: true

    // CallState: 0=idle, 1=dialing, 2=ringing, 3=active, 4=ended
    readonly property int  callState:   iCarCallManager.currentCallState
    readonly property bool isIdle:      callState === 0
    readonly property bool isDialing:   callState === 1
    readonly property bool isRinging:   callState === 2
    readonly property bool isActive:    callState === 3
    readonly property bool callOngoing: isDialing || isRinging || isActive

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Label { text: "PHONE"; color: "#8096a6"; font.pixelSize: 13; font.letterSpacing: 3 }

        // ── Active / Incoming call card ──────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            height: 110
            color: isRinging ? "#1c3020" : (isActive ? "#1c2030" : (isDialing ? "#1a2025" : "#1c2530"))
            radius: 12
            visible: callOngoing

            ColumnLayout {
                anchors.centerIn: parent; spacing: 6
                Label {
                    text: iCarCallManager.contactName || iCarCallManager.phoneNumber || "—"
                    color: "white"; font.pixelSize: 22; font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }
                Label {
                    text: isDialing  ? "Calling..."
                        : isRinging  ? "Incoming call..."
                        : isActive   ? "On call"
                        : ""
                    color: isRinging ? "#8fd19e" : (isActive ? "#5294e2" : "#8096a6")
                    font.pixelSize: 14
                    Layout.alignment: Qt.AlignHCenter
                }
            }
        }

        // ── Call controls ────────────────────────────────────────────────────
        RowLayout {
            Layout.fillWidth: true; spacing: 12
            visible: callOngoing

            Button {
                Layout.fillWidth: true; text: "✓ Accept"
                visible: isRinging
                onClicked: iCarCallManager.acceptCall()
            }
            Button {
                Layout.fillWidth: true; text: "✕ Decline"
                visible: isRinging
                onClicked: iCarCallManager.declineCall()
            }
            Button {
                Layout.fillWidth: true
                text: iCarCallManager.muteActive ? "🔇 Unmute" : "🔈 Mute"
                visible: isActive
                onClicked: iCarCallManager.muteActive ? iCarCallManager.unmuteCall()
                                                      : iCarCallManager.muteCall()
            }
            Button {
                Layout.fillWidth: true; text: "✕ End"
                visible: isActive || isDialing
                onClicked: iCarCallManager.endCall()
            }
        }

        // ── Dial pad ─────────────────────────────────────────────────────────
        RowLayout {
            Layout.fillWidth: true; spacing: 8
            visible: !callOngoing

            TextField {
                id: dialField
                Layout.fillWidth: true
                placeholderText: "Phone number"
                color: "white"
                background: Rectangle { color: "#1c2530"; radius: 8; border.color: "#2c3944" }
                onTextChanged: iCarContactsManager.setDialQuery(text)
            }
            Button {
                text: "Dial"
                enabled: dialField.text.length > 0
                onClicked: { iCarCallManager.dial(dialField.text) }
            }
        }

        // ── Tabs: Favorites / Recents / Contacts ─────────────────────────────
        RowLayout {
            Layout.fillWidth: true; spacing: 0
            visible: !callOngoing

            Repeater {
                model: ["Favorites", "Recents", "Contacts"]
                Button {
                    Layout.fillWidth: true
                    text: modelData
                    flat: phoneTabBar.currentIndex !== index
                    onClicked: phoneTabBar.currentIndex = index
                }
            }
        }

        // hidden tab state
        Item { id: phoneTabBar; property int currentIndex: 0 }

        // ── Favorites ────────────────────────────────────────────────────────
        ListView {
            Layout.fillWidth: true; Layout.fillHeight: true
            clip: true; spacing: 4
            visible: !callOngoing && phoneTabBar.currentIndex === 0
            model: iCarContactsManager.favoriteContacts
            delegate: contactDelegate
        }

        // ── Recents ──────────────────────────────────────────────────────────
        ListView {
            Layout.fillWidth: true; Layout.fillHeight: true
            clip: true; spacing: 4
            visible: !callOngoing && phoneTabBar.currentIndex === 1
            model: iCarContactsManager.recentCalls
            delegate: recentDelegate
        }

        // ── Contacts (full) ──────────────────────────────────────────────────
        ColumnLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; spacing: 8
            visible: !callOngoing && phoneTabBar.currentIndex === 2

            TextField {
                Layout.fillWidth: true
                placeholderText: "Search contacts..."
                color: "white"
                background: Rectangle { color: "#1c2530"; radius: 8; border.color: "#2c3944" }
                onTextChanged: iCarContactsManager.setContactSearchQuery(text)
            }

            ListView {
                Layout.fillWidth: true; Layout.fillHeight: true
                clip: true; spacing: 4
                model: iCarContactsManager.contactSearchQuery.length > 0
                       ? iCarContactsManager.contactSearchResults
                       : iCarContactsManager.contacts
                delegate: contactDelegate
            }
        }
    }

    // ── Delegates ─────────────────────────────────────────────────────────────

    Component {
        id: contactDelegate
        Rectangle {
            width: ListView.view.width; height: 44; radius: 8
            color: index % 2 === 0 ? "#151b20" : "#1c2530"
            RowLayout {
                anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 12
                Label {
                    text: modelData["name"] || modelData["number"] || "—"
                    color: "#d7e0e6"; font.pixelSize: 15; Layout.fillWidth: true
                }
                Label {
                    text: modelData["number"] || ""
                    color: "#8096a6"; font.pixelSize: 12
                }
                Button {
                    text: "Call"; height: 30
                    onClicked: iCarCallManager.dial(modelData["number"] || "")
                }
            }
        }
    }

    Component {
        id: recentDelegate
        Rectangle {
            width: ListView.view.width; height: 48; radius: 8
            color: index % 2 === 0 ? "#151b20" : "#1c2530"
            RowLayout {
                anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 12

                Label {
                    // direction: 0=incoming, 1=outgoing, 2=missed
                    text: modelData["direction"] === 0 ? "←" : (modelData["direction"] === 2 ? "✗" : "→")
                    color: modelData["direction"] === 2 ? "#c0392b" : "#8096a6"
                    font.pixelSize: 16
                }
                ColumnLayout {
                    Layout.fillWidth: true; spacing: 2
                    Label {
                        text: modelData["name"] || modelData["number"] || "—"
                        color: "#d7e0e6"; font.pixelSize: 14
                    }
                    Label {
                        text: modelData["timestamp"] || ""
                        color: "#8096a6"; font.pixelSize: 11
                    }
                }
                Button {
                    text: "Call"; height: 30
                    onClicked: iCarCallManager.dial(modelData["number"] || "")
                }
            }
        }
    }
}
