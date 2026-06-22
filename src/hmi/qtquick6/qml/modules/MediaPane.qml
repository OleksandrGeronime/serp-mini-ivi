import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 1.15

Item {
    Layout.fillWidth: true
    Layout.fillHeight: true

    readonly property bool isPlaying: iCarMediaManager.currentPlaybackState === 1
    readonly property int  durationMs: (iCarMediaManager.currentTrack && iCarMediaManager.currentTrack.duration > 0) ? iCarMediaManager.currentTrack.duration : 0
    readonly property bool hasDuration: durationMs > 0
    readonly property real progress: hasDuration ? Math.min(iCarMediaManager.currentPositionMs / durationMs, 1.0) : 0.0

    // repeat mode cycle: 0=off, 1=repeatOne, 2=repeatAll
    function nextRepeatMode(current) {
        if (current === 0) return 1;
        if (current === 1) return 2;
        return 0;
    }
    function repeatLabel(mode) {
        if (mode === 1) return "⟳¹";
        if (mode === 2) return "⟳";
        return "⟳";
    }

    function formatTime(ms) {
        var s = Math.floor(ms / 1000);
        var m = Math.floor(s / 60);
        s = s % 60;
        return m + ":" + (s < 10 ? "0" + s : s);
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // Left: player controls
        ColumnLayout {
            Layout.preferredWidth: parent.width * 0.48
            Layout.fillHeight: true
            spacing: 14

            Label { text: "MEDIA"; color: "#8096a6"; font.pixelSize: 13; font.letterSpacing: 3 }

            // Track info card
            Rectangle {
                Layout.fillWidth: true
                height: 110
                color: "#1c2530"; radius: 12
                ColumnLayout {
                    anchors.centerIn: parent
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 16
                    spacing: 3
                    Label {
                        Layout.fillWidth: true
                        text: iCarMediaManager.currentTrack && iCarMediaManager.currentTrack.title ? iCarMediaManager.currentTrack.title : "No track"
                        color: "white"; font.pixelSize: 20; font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideRight
                    }
                    Label {
                        Layout.fillWidth: true
                        text: iCarMediaManager.currentTrack && iCarMediaManager.currentTrack.artist ? iCarMediaManager.currentTrack.artist : "—"
                        color: "#8096a6"; font.pixelSize: 13
                        horizontalAlignment: Text.AlignHCenter
                    }
                    Label {
                        Layout.fillWidth: true
                        text: (iCarMediaManager.trackIndex + 1) + " / " + iCarMediaManager.playlistSize
                        color: "#5a6a76"; font.pixelSize: 11
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }

            // Progress bar with drag-to-seek
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                Item {
                    Layout.fillWidth: true
                    height: 16

                    Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        width: parent.width
                        height: 6
                        radius: 3
                        color: "#0d1820"

                        Rectangle {
                            id: progressFill
                            width: parent.width * progress
                            height: parent.height
                            radius: 3
                            color: "#4da6ff"
                            Behavior on width {
                                enabled: !seekArea.pressed
                                NumberAnimation { duration: 800; easing.type: Easing.Linear }
                            }
                        }
                    }

                    MouseArea {
                        id: seekArea
                        anchors.fill: parent
                        enabled: hasDuration
                        cursorShape: hasDuration ? Qt.PointingHandCursor : Qt.ArrowCursor
                        onPressed: function(mouse) {
                            iCarMediaManager.seekTo(Math.round((mouse.x / width) * durationMs));
                        }
                        onPositionChanged: function(mouse) {
                            if (pressed)
                                iCarMediaManager.seekTo(Math.round((Math.max(0, Math.min(mouse.x, width)) / width) * durationMs));
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Label { text: formatTime(iCarMediaManager.currentPositionMs); color: "#8096a6"; font.pixelSize: 10 }
                    Item { Layout.fillWidth: true }
                    Label { text: durationMs > 0 ? formatTime(durationMs) : "--:--"; color: "#8096a6"; font.pixelSize: 10 }
                }
            }

            // Playback controls row
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 10

                Button {
                    text: "◄◄"
                    onClicked: iCarMediaManager.previous()
                }
                Button {
                    text: isPlaying ? "⏸" : "▶"
                    highlighted: isPlaying
                    onClicked: isPlaying ? iCarMediaManager.pause() : iCarMediaManager.play("")
                }
                Button {
                    text: "▶►"
                    onClicked: iCarMediaManager.next()
                }
            }

            // Mode buttons row
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 10

                Button {
                    text: "⇌"
                    highlighted: iCarMediaManager.shuffleEnabled
                    opacity: iCarMediaManager.shuffleEnabled ? 1.0 : 0.45
                    onClicked: iCarMediaManager.setShuffleEnabled(!iCarMediaManager.shuffleEnabled)
                }

                Button {
                    text: iCarMediaManager.currentRepeatMode === 1 ? "⟳¹" : "⟳"
                    highlighted: iCarMediaManager.currentRepeatMode !== 0
                    opacity: iCarMediaManager.currentRepeatMode !== 0 ? 1.0 : 0.45
                    onClicked: iCarMediaManager.setRepeatMode(nextRepeatMode(iCarMediaManager.currentRepeatMode))
                }
            }

            Item { Layout.fillHeight: true }
        }

        // Right: scrollable tracklist
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 6

            Label { text: "PLAYLIST"; color: "#8096a6"; font.pixelSize: 11; font.letterSpacing: 2 }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#111c26"
                radius: 10
                clip: true

                ListView {
                    anchors.fill: parent
                    anchors.margins: 6
                    model: iCarMediaManager.trackList
                    spacing: 2
                    clip: true

                    delegate: Rectangle {
                        width: ListView.view.width
                        height: 52
                        radius: 8
                        color: iCarMediaManager.trackIndex === index ? "#1e3a56" : (trackHover.containsMouse ? "#1c2a38" : "transparent")

                        ColumnLayout {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.margins: 10
                            spacing: 2

                            Label {
                                Layout.fillWidth: true
                                text: modelData.title || ("Track " + (index + 1))
                                color: iCarMediaManager.trackIndex === index ? "#4da6ff" : "white"
                                font.pixelSize: 13
                                font.bold: iCarMediaManager.trackIndex === index
                                elide: Text.ElideRight
                            }
                            Label {
                                Layout.fillWidth: true
                                text: modelData.artist || "Unknown"
                                color: "#8096a6"
                                font.pixelSize: 11
                                elide: Text.ElideRight
                            }
                        }

                        HoverHandler { id: trackHover }

                        TapHandler {
                            onTapped: iCarMediaManager.playAt(index)
                        }
                    }
                }
            }
        }
    }
}
