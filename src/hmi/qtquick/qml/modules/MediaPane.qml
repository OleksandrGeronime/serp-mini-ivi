import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    property bool radioMode: false

    // ── MEDIA PLAYER ─────────────────────────────────────────────────────────
    RowLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16
        visible: !radioMode

        // ── Left: album art + now-playing ──────────────────────────────────
        ColumnLayout {
            Layout.preferredWidth: 260
            Layout.fillHeight: true
            spacing: 12

            // Album art placeholder with animated shimmer when playing
            Rectangle {
                Layout.preferredWidth: 240
                Layout.preferredHeight: 240
                color: "#0d1520"
                radius: 12
                border.color: "#1e3a5a"
                border.width: 2
                clip: true

                // Background gradient art
                Rectangle {
                    anchors.fill: parent
                    radius: parent.radius
                    gradient: Gradient {
                        orientation: Gradient.Diagonal
                        GradientStop {
                            position: 0.0
                            color: hmi.mediaState === "playing" ? "#1a2a4a" : "#0d1520"
                        }
                        GradientStop {
                            position: 1.0
                            color: hmi.mediaState === "playing" ? "#0a1a2a" : "#0d1520"
                        }
                    }
                }

                // Concentric circles art
                Repeater {
                    model: 4
                    Rectangle {
                        property int sz: 80 + index * 40
                        width: sz; height: sz; radius: sz / 2
                        anchors.centerIn: parent
                        color: "transparent"
                        border.color: hmi.mediaState === "playing" ? "#1e4a7a" : "#111a22"
                        border.width: 1

                        RotationAnimation on rotation {
                            running: hmi.mediaState === "playing"
                            from: 0; to: 360
                            duration: 6000 + index * 2000
                            loops: Animation.Infinite
                        }
                    }
                }

                // Center disc
                Rectangle {
                    width: 70; height: 70; radius: 35
                    anchors.centerIn: parent
                    color: "#0a1218"
                    border.color: "#2a5a8a"; border.width: 2

                    RotationAnimation on rotation {
                        running: hmi.mediaState === "playing"
                        from: 0; to: 360
                        duration: 3000
                        loops: Animation.Infinite
                    }

                    Rectangle {
                        width: 12; height: 12; radius: 6
                        anchors.centerIn: parent
                        color: "#2a5a8a"
                    }
                }

                // Music note icon when stopped
                Label {
                    anchors.centerIn: parent
                    text: hmi.mediaState === "playing" ? "" : "♪"
                    color: "#1e3a5a"
                    font.pixelSize: 48
                    visible: hmi.mediaState !== "playing"
                }
            }

            // Source badge
            Rectangle {
                Layout.preferredWidth: 240
                height: 28
                radius: 5
                color: "#0a1830"
                border.color: "#1a3a5a"
                border.width: 1
                Label {
                    anchors.centerIn: parent
                    text: hmi.mediaSource !== "" && hmi.mediaSource !== "none"
                          ? "▶  " + hmi.mediaSource.toUpperCase() : "NO SOURCE"
                    color: "#4a8ac0"
                    font.pixelSize: 10
                    font.bold: true
                    font.letterSpacing: 1
                }
            }

            Item { Layout.fillHeight: true }
        }

        // ── Right: track info + controls + playlist ────────────────────────
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 14

            // Track title + artist
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                Label {
                    Layout.fillWidth: true
                    text: hmi.mediaTrack !== "" && hmi.mediaTrack !== "No track"
                          ? hmi.mediaTrack : "No track"
                    color: "white"
                    font.pixelSize: 22
                    font.bold: true
                    elide: Text.ElideRight
                }
                Label {
                    Layout.fillWidth: true
                    text: hmi.mediaArtist !== "" && hmi.mediaArtist !== "Unknown"
                          ? hmi.mediaArtist : "Unknown Artist"
                    color: "#7aa8cc"
                    font.pixelSize: 14
                    elide: Text.ElideRight
                }
            }

            // State badge
            Rectangle {
                height: 26
                width: stateLbl.implicitWidth + 20
                radius: 13
                color: {
                    if (hmi.mediaState === "playing") return "#0d2a0d"
                    if (hmi.mediaState === "paused")  return "#1a1a0d"
                    return "#1a1a1a"
                }
                border.color: {
                    if (hmi.mediaState === "playing") return "#2a7a2a"
                    if (hmi.mediaState === "paused")  return "#7a7a2a"
                    return "#2a2a2a"
                }
                border.width: 1
                Label {
                    id: stateLbl
                    anchors.centerIn: parent
                    text: {
                        if (hmi.mediaState === "playing") return "● PLAYING"
                        if (hmi.mediaState === "paused")  return "⏸ PAUSED"
                        return "■ STOPPED"
                    }
                    color: {
                        if (hmi.mediaState === "playing") return "#00cc44"
                        if (hmi.mediaState === "paused")  return "#cccc00"
                        return "#606060"
                    }
                    font.pixelSize: 11
                    font.bold: true
                }
            }

            // Progress bar
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                // Track counter
                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        text: hmi.mediaPlaylistSize !== "0"
                              ? "Track " + (parseInt(hmi.mediaTrackIndex) + 1)
                                + " / " + hmi.mediaPlaylistSize
                              : "Track — / —"
                        color: "#4a6a8a"
                        font.pixelSize: 10
                    }
                    Item { Layout.fillWidth: true }
                    Label {
                        text: hmi.audioFocus !== " / " ? hmi.audioFocus : ""
                        color: "#2a4a6a"
                        font.pixelSize: 10
                    }
                }

                // Animated progress bar
                Rectangle {
                    Layout.fillWidth: true
                    height: 6
                    radius: 3
                    color: "#0a1830"
                    border.color: "#1a3050"
                    border.width: 1

                    Rectangle {
                        id: progressBar
                        height: parent.height
                        radius: parent.radius
                        color: hmi.mediaState === "playing" ? "#2196F3"
                             : hmi.mediaState === "paused"  ? "#88aa44"
                             : "#1a2a3a"
                        width: parent.width * progressFraction
                        property real progressFraction: hmi.mediaState === "playing" ? 1.0
                                                      : hmi.mediaState === "paused"  ? pausedProgress
                                                      : 0.0
                        property real pausedProgress: 0.3

                        Behavior on width {
                            enabled: hmi.mediaState === "playing"
                            NumberAnimation { duration: 60000; easing.type: Easing.Linear }
                        }

                        // Pulsing glow when playing
                        Rectangle {
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            width: 10; height: 10; radius: 5
                            color: "#2196F3"
                            opacity: hmi.mediaState === "playing" ? 0.9 : 0
                            SequentialAnimation on opacity {
                                running: hmi.mediaState === "playing"
                                loops: Animation.Infinite
                                NumberAnimation { to: 0.3; duration: 600 }
                                NumberAnimation { to: 0.9; duration: 600 }
                            }
                        }
                    }
                }
            }

            // Control buttons
            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                // Play / Pause
                Rectangle {
                    Layout.preferredWidth: 80
                    height: 52
                    radius: 26
                    color: playBtn.pressed ? "#1a3a6a" : "#0d2545"
                    border.color: "#2a5a9a"; border.width: 2

                    Label {
                        anchors.centerIn: parent
                        text: hmi.mediaState === "playing" ? "⏸" : "▶"
                        color: "white"
                        font.pixelSize: 22
                    }
                    MouseArea {
                        id: playBtn
                        anchors.fill: parent
                        onClicked: hmi.mediaState === "playing"
                                   ? hmi.triggerAction(1)
                                   : hmi.triggerAction(0)
                    }
                }

                // Next
                Rectangle {
                    Layout.preferredWidth: 66
                    height: 52
                    radius: 26
                    color: nextBtn.pressed ? "#0d2a1a" : "#091a10"
                    border.color: "#1a5a2a"; border.width: 2

                    Label {
                        anchors.centerIn: parent
                        text: "⏭"
                        color: "#60cc80"
                        font.pixelSize: 20
                    }
                    MouseArea {
                        id: nextBtn
                        anchors.fill: parent
                        onClicked: hmi.triggerAction(2)
                    }
                }

                // Stop
                Rectangle {
                    Layout.preferredWidth: 66
                    height: 52
                    radius: 26
                    color: stopBtn.pressed ? "#2a0d0d" : "#1a0909"
                    border.color: "#5a1a1a"; border.width: 2

                    Label {
                        anchors.centerIn: parent
                        text: "⏹"
                        color: "#cc4040"
                        font.pixelSize: 20
                    }
                    MouseArea {
                        id: stopBtn
                        anchors.fill: parent
                        onClicked: hmi.triggerAction(3)
                    }
                }

                Item { Layout.fillWidth: true }

                // Volume buttons
                Rectangle {
                    Layout.preferredWidth: 52
                    height: 36
                    radius: 6
                    color: vol1Btn.pressed ? "#0a1a30" : "#060e1a"
                    border.color: "#1a2a3a"; border.width: 1
                    Label {
                        anchors.centerIn: parent
                        text: "🔉"
                        font.pixelSize: 14
                    }
                    MouseArea {
                        id: vol1Btn
                        anchors.fill: parent
                        onClicked: hmi.triggerAction(6)
                    }
                }
                Rectangle {
                    Layout.preferredWidth: 52
                    height: 36
                    radius: 6
                    color: vol2Btn.pressed ? "#0a1a30" : "#060e1a"
                    border.color: "#1a2a3a"; border.width: 1
                    Label {
                        anchors.centerIn: parent
                        text: "🔊"
                        font.pixelSize: 14
                    }
                    MouseArea {
                        id: vol2Btn
                        anchors.fill: parent
                        onClicked: hmi.triggerAction(7)
                    }
                }
            }

            // Playlist / queue panel
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#060e18"
                radius: 8
                border.color: "#0e1e2e"
                border.width: 1
                clip: true

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 0

                    // Header
                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            text: "PLAYLIST"
                            color: "#2a5a8a"
                            font.pixelSize: 10
                            font.bold: true
                            font.letterSpacing: 2
                        }
                        Item { Layout.fillWidth: true }
                        Label {
                            text: hmi.mediaPlaylistSize !== "0" ? hmi.mediaPlaylistSize + " tracks" : ""
                            color: "#1a3a5a"
                            font.pixelSize: 10
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: "#0e1e2e"
                    }

                    Item { height: 6 }

                    // Playlist entries: show indices around current
                    Repeater {
                        model: {
                            var total = parseInt(hmi.mediaPlaylistSize) || 0
                            var cur   = parseInt(hmi.mediaTrackIndex)   || 0
                            var count = Math.min(total, 8)
                            var start = Math.max(0, cur - 2)
                            if (start + count > total) start = Math.max(0, total - count)
                            var items = []
                            for (var i = start; i < start + count && i < total; i++)
                                items.push(i)
                            return items
                        }
                        delegate: Item {
                            Layout.fillWidth: true
                            height: 30
                            property int trackNum: modelData
                            property bool isCurrent: trackNum === (parseInt(hmi.mediaTrackIndex) || 0)

                            Rectangle {
                                anchors.fill: parent
                                anchors.leftMargin: -10
                                anchors.rightMargin: -10
                                color: isCurrent ? "#0d2040" : "transparent"
                                radius: 0
                            }

                            RowLayout {
                                anchors.fill: parent
                                spacing: 8

                                // Playing indicator
                                Label {
                                    text: isCurrent ? (hmi.mediaState === "playing" ? "▶" : "◆") : " "
                                    color: isCurrent ? "#2196F3" : "#1a3a5a"
                                    font.pixelSize: 10
                                    Layout.preferredWidth: 14
                                }

                                // Track number
                                Label {
                                    text: (trackNum + 1)
                                    color: isCurrent ? "#4a8ac0" : "#1a3a5a"
                                    font.pixelSize: 11
                                    Layout.preferredWidth: 24
                                    horizontalAlignment: Text.AlignRight
                                }

                                // Track title
                                Label {
                                    Layout.fillWidth: true
                                    text: isCurrent && hmi.mediaTrack !== "No track" && hmi.mediaTrack !== ""
                                          ? hmi.mediaTrack
                                          : "Track " + (trackNum + 1)
                                    color: isCurrent ? "white" : "#2a5a8a"
                                    font.pixelSize: 12
                                    font.bold: isCurrent
                                    elide: Text.ElideRight
                                }
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }

                    // Source quick-launch
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 6

                        Label {
                            text: "SOURCE:"
                            color: "#1a3a5a"
                            font.pixelSize: 9
                            font.bold: true
                            font.letterSpacing: 1
                        }

                        Repeater {
                            model: [
                                {label: "Roadtrip", idx: 4},
                                {label: "Podcast",  idx: 5}
                            ]
                            delegate: Rectangle {
                                height: 24
                                width: srcLbl.implicitWidth + 16
                                radius: 4
                                color: srcBtn.pressed ? "#0d2040" : "#060e1a"
                                border.color: "#1a3a5a"; border.width: 1
                                Label {
                                    id: srcLbl
                                    anchors.centerIn: parent
                                    text: modelData.label
                                    color: "#4a7aaa"
                                    font.pixelSize: 10
                                }
                                MouseArea {
                                    id: srcBtn
                                    anchors.fill: parent
                                    onClicked: hmi.triggerAction(modelData.idx)
                                }
                            }
                        }

                        Item { Layout.fillWidth: true }
                    }
                }
            }
        }
    }

    // ── RADIO ─────────────────────────────────────────────────────────────────
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 10
        visible: radioMode

        // Top bar: band toggle + title
        RowLayout {
            spacing: 8

            Button {
                id: fmBtn
                text: "FM"
                Layout.preferredWidth: 60
                Layout.preferredHeight: 34
                font.bold: true
                highlighted: hmi.radioBand === "FM"
                onClicked: hmi.triggerAction(10)
            }
            Button {
                id: amBtn
                text: "AM"
                Layout.preferredWidth: 60
                Layout.preferredHeight: 34
                font.bold: true
                highlighted: hmi.radioBand === "AM"
                onClicked: hmi.triggerAction(11)
            }
            Item { Layout.fillWidth: true }
            Label {
                text: "IVI RADIO DEMO"
                color: "#3a7a3a"
                font.pixelSize: 12
                font.bold: true
                font.letterSpacing: 2
            }
        }

        // Main content row
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 14

            // ── Display panel ──────────────────────────────────────────────
            Rectangle {
                Layout.preferredWidth: 300
                Layout.fillHeight: true
                color: "#050f05"
                border.color: "#1a5c1a"
                border.width: 2
                radius: 10

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 18
                    spacing: 10

                    // Frequency
                    RowLayout {
                        spacing: 6
                        Label {
                            text: hmi.radioFreq
                            color: "#00dd00"
                            font.pixelSize: 48
                            font.bold: true
                        }
                        ColumnLayout {
                            spacing: 0
                            Item { Layout.fillHeight: true }
                            Label {
                                text: hmi.radioBand === "AM" ? "kHz" : "MHz"
                                color: "#009900"
                                font.pixelSize: 14
                                font.bold: true
                                bottomPadding: 10
                            }
                        }
                    }

                    // Station name
                    Label {
                        Layout.fillWidth: true
                        text: hmi.radioName !== "" ? hmi.radioName.toUpperCase() : "(BETWEEN STATIONS)"
                        color: "white"
                        font.pixelSize: 14
                        font.bold: true
                        wrapMode: Text.Wrap
                        maximumLineCount: 2
                    }

                    // Signal bars + status row
                    RowLayout {
                        spacing: 10

                        Row {
                            spacing: 3
                            Repeater {
                                model: 8
                                Rectangle {
                                    width: 8
                                    height: 8 + index * 4
                                    anchors.bottom: parent ? parent.bottom : undefined
                                    radius: 2
                                    color: {
                                        var litBars = hmi.radioSignal > 0 ? Math.round(hmi.radioSignal / 11) : 0
                                        if (index < litBars)
                                            return index < 5 ? "#00cc00" : "#88cc00"
                                        return "#152015"
                                    }
                                }
                            }
                        }

                        RowLayout {
                            spacing: 5
                            Rectangle {
                                width: 8; height: 8; radius: 4
                                color: hmi.radioStatus === "playing" ? "#00dd00"
                                     : hmi.radioStatus === "scan"    ? "#dddd00"
                                     : "#303030"
                            }
                            Label {
                                text: {
                                    if (hmi.radioStatus === "playing") return "PLAYING"
                                    if (hmi.radioStatus === "scan")    return "SCANNING"
                                    if (hmi.radioSignal > 0)           return "RECEIVING"
                                    return "STREAM ERR"
                                }
                                color: hmi.radioStatus === "playing" ? "#00cc00"
                                     : hmi.radioStatus === "scan"    ? "#cccc00"
                                     : "#505050"
                                font.pixelSize: 11
                                font.bold: true
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }

                    // FM frequency ruler
                    Item {
                        Layout.fillWidth: true
                        height: 28
                        visible: hmi.radioBand !== "AM"

                        Rectangle {
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width; height: 2
                            color: "#1a3a1a"
                        }
                        Repeater {
                            model: [87.5, 89.7, 91.5, 93.1, 95.5, 98.7, 101.1, 103.7, 106.1, 107.9]
                            Rectangle {
                                width: 3; height: 12; radius: 1
                                anchors.bottom: parent.bottom
                                color: "#d07020"
                                x: Math.round((modelData - 87.0) / 21.0 * parent.width) - 1
                            }
                        }
                        Rectangle {
                            width: 16; height: 16; radius: 8
                            anchors.bottom: parent.bottom
                            color: "#00cc00"
                            border.color: "#003300"; border.width: 2
                            x: {
                                var f = parseFloat(hmi.radioFreq) || 87.5
                                return Math.max(0, Math.min(parent.width - 16, (f - 87.0) / 21.0 * parent.width - 8))
                            }
                        }
                        Repeater {
                            model: [{v:88,p:0},{v:93,p:0.24},{v:98,p:0.52},{v:103,p:0.76},{v:108,p:1.0}]
                            Label {
                                text: modelData.v
                                color: "#3a5a3a"
                                font.pixelSize: 9
                                anchors.top: parent.top
                                x: Math.round(modelData.p * parent.width) - (index === 4 ? 16 : 0)
                            }
                        }
                    }
                }
            }

            // ── Right panel: controls + preset grid ───────────────────────
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 10

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Button {
                        text: "◀◀ PREV"
                        Layout.fillWidth: true
                        Layout.preferredHeight: 44
                        onClicked: hmi.triggerAction(7)
                    }
                    Button {
                        text: hmi.radioStatus === "playing" ? "⏹ STOP" : "▶ PLAY"
                        Layout.preferredWidth: 130
                        Layout.preferredHeight: 44
                        highlighted: hmi.radioStatus === "playing"
                        onClicked: hmi.radioStatus === "playing" ? hmi.triggerAction(9) : hmi.triggerAction(0)
                    }
                    Button {
                        text: "NEXT ▶▶"
                        Layout.fillWidth: true
                        Layout.preferredHeight: 44
                        onClicked: hmi.triggerAction(6)
                    }
                    Button {
                        text: "↺ SCAN"
                        Layout.fillWidth: true
                        Layout.preferredHeight: 44
                        highlighted: hmi.radioStatus === "scan"
                        onClicked: hmi.triggerAction(8)
                    }
                }

                GridLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    columns: 2
                    rowSpacing: 8
                    columnSpacing: 8

                    Repeater {
                        model: 6
                        delegate: Button {
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            property var ad: hmi.actions.length > index ? hmi.actions[index] : {"label":"","index":index,"enabled":false}
                            property bool isActive: {
                                var parts = (ad.label || "").split(" ")
                                var freq = parts.length > 0 ? parts[0] : ""
                                return hmi.radioFreq === freq && hmi.radioStatus === "playing"
                            }

                            background: Rectangle {
                                color: parent.isActive ? "#142814" : "#0e1a0e"
                                border.color: parent.isActive ? "#3a7a3a" : "#1e3a1e"
                                border.width: parent.isActive ? 2 : 1
                                radius: 8
                            }

                            contentItem: Item {
                                ColumnLayout {
                                    anchors.centerIn: parent
                                    spacing: 4

                                    Label {
                                        Layout.fillWidth: true
                                        horizontalAlignment: Text.AlignHCenter
                                        text: {
                                            var names = [
                                                "GROOVE SALAD", "SECRET AGENT", "ILL ST OBSERVER",
                                                "BEAT BLENDER", "FOLK FORWARD", "THE TRIP"
                                            ]
                                            return names[index] || (parent.parent.ad.label || "")
                                        }
                                        color: parent.parent.parent.isActive ? "#00cc00" : "white"
                                        font.bold: true
                                        font.pixelSize: 11
                                        elide: Text.ElideRight
                                    }
                                    Label {
                                        Layout.fillWidth: true
                                        horizontalAlignment: Text.AlignHCenter
                                        text: {
                                            var freqs = ["87.5 MHz","98.7 MHz","101.1 MHz","103.7 MHz","107.9 MHz","612 kHz"]
                                            return freqs[index] || ""
                                        }
                                        color: "#5a8a5a"
                                        font.pixelSize: 10
                                    }
                                }
                            }

                            onClicked: hmi.triggerAction(ad.index)
                        }
                    }
                }
            }
        }
    }
}
