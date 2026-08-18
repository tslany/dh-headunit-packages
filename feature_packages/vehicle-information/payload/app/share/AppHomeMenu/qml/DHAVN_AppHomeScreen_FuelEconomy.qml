import QtQuick 1.1

Item {
    id: fuelEconomy
    width: 1280
    height: 720
    visible: pageActive

    property bool pageActive: false
    property int countryVariant: -1
    signal backRequested()

    property bool profileVerified: countryVariant === 5
    property bool russianProfile: countryVariant === 7
    property bool northAmericanProfile: countryVariant === 1
                                              || countryVariant === 6
    property bool kdmProfile: countryVariant === 0
    property bool modelReady: false
    property bool backendReady: false
    property bool fuelSeen: false
    property bool fuelFresh: false
    property bool speedSeen: false
    property bool speedFresh: false
    property bool averageValid: false
    property bool dteValid: false
    property real publishedWallMs: 0
    property real observedWallMs: 0
    property bool snapshotFresh: publishedWallMs > 0 &&
                                 observedWallMs >= publishedWallMs &&
                                 observedWallMs - publishedWallMs <= 5000
    property int averageRaw: 0
    property int dteRaw: 0
    property real sessionRaw: -1.0
    property int historyMask: 0
    property variant historyValues: [0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0]
    property bool backPressed: false
    property bool stateRequestPending: false
    property int stateRequestGeneration: 0

    function numberValue(value, fallback) {
        var parsed = Number(value)
        return isNaN(parsed) ? fallback : parsed
    }

    function clearSnapshotValues() {
        modelReady = false
        backendReady = false
        fuelSeen = false
        fuelFresh = false
        speedSeen = false
        speedFresh = false
        averageValid = false
        dteValid = false
        averageRaw = 0
        dteRaw = 0
        sessionRaw = -1.0
        historyMask = 0
        historyValues = [0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0]
    }

    function clearModel() {
        clearSnapshotValues()
        publishedWallMs = 0
    }

    function attributeValue(text, name, fallback) {
        var marker = name + "=\""
        var start = text.indexOf(marker)
        if (start < 0)
            return fallback
        start += marker.length
        var end = text.indexOf("\"", start)
        return end < 0 ? fallback : text.substring(start, end)
    }

    function applyState(text) {
        if (!profileVerified || russianProfile) {
            clearModel()
            return
        }
        var stateStart = text.indexOf("<state ")
        var stateEnd = text.indexOf("/>", stateStart)
        if (stateStart < 0 || stateEnd < 0) {
            observedWallMs = new Date().getTime()
            return
        }
        var row = text.substring(stateStart, stateEnd)
        var observed = new Date().getTime()
        var published = numberValue(
                    attributeValue(row, "published_wall_ms", "0"), 0)
        var current = published > 0 && observed >= published
                && observed - published <= 5000
        observedWallMs = observed
        publishedWallMs = published
        if (!current)
            return
        modelReady = true
        backendReady = numberValue(
                    attributeValue(row, "backend_ready", "0"), 0) === 1
        fuelSeen = numberValue(
                    attributeValue(row, "fuel_seen", "0"), 0) === 1
        fuelFresh = numberValue(
                    attributeValue(row, "fuel_fresh", "0"), 0) === 1
        speedSeen = numberValue(
                    attributeValue(row, "speed_seen", "0"), 0) === 1
        speedFresh = numberValue(
                    attributeValue(row, "speed_fresh", "0"), 0) === 1
        var incomingAverageValid = numberValue(
                    attributeValue(row, "average_valid", "0"), 0) === 1
        var incomingDteValid = numberValue(
                    attributeValue(row, "dte_valid", "0"), 0) === 1
        if (incomingAverageValid) {
            averageRaw = numberValue(
                        attributeValue(row, "average_raw", "0"), 0)
            averageValid = true
        }
        if (incomingDteValid) {
            dteRaw = numberValue(attributeValue(row, "dte_raw", "0"), 0)
            dteValid = true
        }
        sessionRaw = numberValue(
                    attributeValue(row, "session_raw_tenths", "-1"), -1)
        historyMask = numberValue(
                    attributeValue(row, "history_mask", "0"), 0)
        historyValues = [numberValue(attributeValue(row, "h0", "0"), 0),
                         numberValue(attributeValue(row, "h1", "0"), 0),
                         numberValue(attributeValue(row, "h2", "0"), 0),
                         numberValue(attributeValue(row, "h3", "0"), 0),
                         numberValue(attributeValue(row, "h4", "0"), 0),
                         numberValue(attributeValue(row, "h5", "0"), 0),
                         numberValue(attributeValue(row, "h6", "0"), 0),
                         numberValue(attributeValue(row, "h7", "0"), 0),
                         numberValue(attributeValue(row, "h8", "0"), 0),
                         numberValue(attributeValue(row, "h9", "0"), 0),
                         numberValue(attributeValue(row, "h10", "0"), 0),
                         numberValue(attributeValue(row, "h11", "0"), 0)]
    }

    function requestState() {
        if (!pageActive || !profileVerified || russianProfile
                || stateRequestPending)
            return
        var generation = stateRequestGeneration
        var request = new XMLHttpRequest()
        stateRequestPending = true
        request.onreadystatechange = function() {
            if (request.readyState !== XMLHttpRequest.DONE)
                return
            if (generation !== stateRequestGeneration)
                return
            stateRequestPending = false
            if (!pageActive || russianProfile)
                return
            if ((request.status !== 0
                    && (request.status < 200 || request.status >= 300))
                    || request.responseText.length === 0) {
                return
            }
            applyState(request.responseText)
        }
        try {
            request.open("GET", "file:///tmp/dh_fuel_economy_state.xml", true)
            request.send()
        } catch (error) {
            if (generation === stateRequestGeneration) {
                stateRequestPending = false
                observedWallMs = new Date().getTime()
            }
        }
    }

    function historyValue(index) {
        return index >= 0 && index < historyValues.length
                ? historyValues[index] / 10.0 : 0.0
    }

    function historyValid(index) {
        return Math.floor(historyMask / Math.pow(2, index)) % 2 === 1
    }

    function graphHeight(value) {
        return Math.max(0, Math.min(historyGraph.plotHeight,
                                    value * historyGraph.plotHeight / 30.0))
    }

    function unavailableMarket() {
        if (northAmericanProfile)
            return qsTr("North America")
        if (kdmProfile)
            return qsTr("Korea")
        return qsTr("this market")
    }

    function unavailableDetail() {
        return qsTr("The 0x5D7 frame layout is recognized, but its units "
                    + "have not been verified on this regional vehicle profile.")
    }

    function expireStaleSnapshot() {
        if (snapshotFresh)
            return
        fuelFresh = false
        speedFresh = false
    }

    Component.onCompleted: {
        observedWallMs = new Date().getTime()
        if (pageActive && profileVerified && !russianProfile)
            requestState()
    }

    onCountryVariantChanged: {
        stateRequestGeneration += 1
        stateRequestPending = false
        if (!profileVerified || russianProfile) {
            clearModel()
        } else if (pageActive) {
            observedWallMs = new Date().getTime()
            requestState()
        }
    }

    onPageActiveChanged: {
        stateRequestGeneration += 1
        stateRequestPending = false
        backPressed = false
        if (pageActive && profileVerified && !russianProfile) {
            observedWallMs = new Date().getTime()
            requestState()
        }
    }

    Timer {
        interval: 1000
        repeat: true
        running: fuelEconomy.pageActive && fuelEconomy.profileVerified
                 && !fuelEconomy.russianProfile
        onTriggered: {
            fuelEconomy.observedWallMs = new Date().getTime()
            fuelEconomy.expireStaleSnapshot()
            fuelEconomy.requestState()
        }
    }

    Rectangle {
        x: 0
        y: 93
        width: 1280
        height: 627
        color: "#0F1318"
    }

    Image {
        x: 0
        y: 93
        width: 1280
        height: 72
        source: "/app/share/images/AppHome/bg_title.png"
    }

    Text {
        x: 46
        y: 103
        width: 850
        height: 52
        text: qsTr("Fuel Economy")
        font.family: "DH_HDB"
        font.pointSize: 40
        color: "#f2f2f2"
        verticalAlignment: Text.AlignVCenter
    }

    Item {
        id: backButton
        x: 1139
        y: 93
        width: 141
        height: 72

        Image {
            anchors.fill: parent
            source: "/app/share/images/AppHome/btn_title_back_n.png"
        }

        Image {
            anchors.fill: parent
            visible: fuelEconomy.backPressed
            source: "/app/share/images/AppHome/btn_title_back_p.png"
        }

        MouseArea {
            anchors.fill: parent
            beepEnabled: false
            onPressed: fuelEconomy.backPressed = true
            onCanceled: fuelEconomy.backPressed = false
            onReleased: {
                var activate = mouse.x >= 0 && mouse.x < width
                        && mouse.y >= 0 && mouse.y < height
                fuelEconomy.backPressed = false
                if (activate)
                    fuelEconomy.backRequested()
            }
        }
    }

    Item {
        id: verifiedFuelView
        x: 0
        y: 165
        width: 1280
        height: 555
        visible: fuelEconomy.profileVerified

        property int outerMargin: 34
        property int cardPadding: 28
        property int cardBorderWidth: 1
        property int cardGap: 46
        property int contentInset: cardPadding + cardBorderWidth
        property int leftCardWidth: Math.ceil(
                    distanceTitleMeasure.paintedWidth)
                    + 2 * contentInset

        Text {
            id: distanceTitleMeasure
            visible: false
            text: qsTr("Distance to empty")
            font.family: "DH_HDB"
            font.pointSize: 25
            wrapMode: Text.NoWrap
        }

        Rectangle {
            id: averageCard
            x: verifiedFuelView.outerMargin
            y: 28
            width: verifiedFuelView.leftCardWidth
            height: 146
            radius: 3
            color: "#151d25"
            border.width: verifiedFuelView.cardBorderWidth
            border.color: "#35434f"

            Text {
                x: verifiedFuelView.contentInset
                y: 20
                width: parent.width - 2 * verifiedFuelView.contentInset
                height: 38
                text: qsTr("Average")
                font.family: "DH_HDB"; font.pointSize: 26
                color: "#b8bec4"
            }
            Text {
                x: verifiedFuelView.contentInset
                y: 47
                width: averageUnit.x - verifiedFuelView.contentInset - 8
                height: 76
                text: fuelEconomy.averageValid
                      ? (fuelEconomy.averageRaw / 10.0).toFixed(1) : "--.-"
                font.family: "DH_HDB"; font.pointSize: 55
                color: "#f2f2f2"
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
            }
            Text {
                id: averageUnit
                anchors.right: parent.right
                anchors.rightMargin: verifiedFuelView.contentInset
                y: 51
                width: 58
                height: 68
                text: "L/100\nkm"
                font.family: "DH_HDB"; font.pointSize: 18
                color: "#b8bec4"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            id: currentDriveCard
            x: verifiedFuelView.outerMargin
            y: 191
            width: verifiedFuelView.leftCardWidth
            height: 146
            radius: 3
            color: "#151d25"
            border.width: verifiedFuelView.cardBorderWidth
            border.color: "#35434f"

            Text {
                x: verifiedFuelView.contentInset
                y: 16
                width: parent.width - 2 * verifiedFuelView.contentInset
                height: 34
                text: qsTr("Current drive")
                font.family: "DH_HDB"; font.pointSize: 25
                color: "#b8bec4"
            }
            Text {
                x: verifiedFuelView.contentInset
                y: 47
                width: currentDriveUnit.x
                       - verifiedFuelView.contentInset - 8
                height: 76
                text: fuelEconomy.sessionRaw >= 0
                      ? (fuelEconomy.sessionRaw / 10.0).toFixed(1) : "--.-"
                font.family: "DH_HDB"; font.pointSize: 55
                color: "#f2f2f2"
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
            }
            Text {
                id: currentDriveUnit
                anchors.right: parent.right
                anchors.rightMargin: verifiedFuelView.contentInset
                y: 51
                width: 58
                height: 68
                text: "L/100\nkm"
                font.family: "DH_HDB"; font.pointSize: 18
                color: "#b8bec4"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            id: distanceCard
            x: verifiedFuelView.outerMargin
            y: 354
            width: verifiedFuelView.leftCardWidth
            height: 146
            radius: 3
            color: "#151d25"
            border.width: verifiedFuelView.cardBorderWidth
            border.color: "#35434f"

            Text {
                x: verifiedFuelView.contentInset
                y: 16
                height: 34
                text: distanceTitleMeasure.text
                font.family: "DH_HDB"; font.pointSize: 25
                color: "#b8bec4"
                wrapMode: Text.NoWrap
            }
            Text {
                x: verifiedFuelView.contentInset
                y: 47
                width: dteUnit.x - verifiedFuelView.contentInset - 8
                height: 76
                text: fuelEconomy.dteValid
                      ? fuelEconomy.dteRaw.toFixed(0) : "---"
                font.family: "DH_HDB"; font.pointSize: 55
                color: "#f2f2f2"
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
            }
            Text {
                id: dteUnit
                anchors.right: parent.right
                anchors.rightMargin: verifiedFuelView.contentInset
                y: 75
                height: 35
                text: "km"
                font.family: "DH_HDB"; font.pointSize: 24
                color: "#b8bec4"
            }
        }

        Item {
            id: historyGraph
            x: verifiedFuelView.outerMargin + verifiedFuelView.leftCardWidth
               + verifiedFuelView.cardGap
            y: 18
            width: parent.width - x - verifiedFuelView.outerMargin
            height: 521

            property int plotLeft: 34
            property int plotRight: 18
            property int plotTop: 8
            property int plotHeight: 470
            property real plotWidth: width - plotLeft - plotRight
            property int axisY: plotTop + plotHeight + 13

            Repeater {
                model: 4
                Rectangle {
                    x: historyGraph.plotLeft
                    y: historyGraph.plotTop
                       + index * historyGraph.plotHeight / 3
                    width: historyGraph.plotWidth
                    height: 1
                    color: "#34414b"
                    Text {
                        x: -34; y: -15; width: 30; height: 28
                        text: (30 - index * 10).toFixed(0)
                        font.family: "DH_HDB"; font.pointSize: 16
                        color: "#777f87"
                        horizontalAlignment: Text.AlignRight
                    }
                }
            }

            Item {
                x: historyGraph.plotLeft
                y: historyGraph.plotTop
                width: historyGraph.plotWidth
                height: historyGraph.plotHeight

                Repeater {
                    model: 12
                    Item {
                        x: index * width
                        y: 0
                        width: parent.width / 12
                        height: parent.height
                        Rectangle {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.bottom
                            width: Math.max(24, parent.width * 0.7)
                            height: fuelEconomy.historyValid(index)
                                    ? fuelEconomy.graphHeight(
                                          fuelEconomy.historyValue(index)) : 2
                            color: fuelEconomy.historyValid(index)
                                   ? "#438ed1" : "#313a42"
                        }
                    }
                }

                Rectangle {
                    id: currentDriveLine
                    x: 0
                    y: fuelEconomy.sessionRaw >= 0
                       ? historyGraph.plotHeight - fuelEconomy.graphHeight(
                             fuelEconomy.sessionRaw / 10.0)
                       : historyGraph.plotHeight - 1
                    width: parent.width
                    height: 3
                    color: fuelEconomy.sessionRaw >= 0
                           ? "#55d8e7" : "transparent"
                }
            }

            Text {
                x: historyGraph.plotLeft
                y: historyGraph.axisY
                width: 100; height: 30
                text: qsTr("Now")
                font.family: "DH_HDB"; font.pointSize: 18
                color: "#777f87"
            }
            Text {
                x: historyGraph.plotLeft + historyGraph.plotWidth / 3 - 40
                y: historyGraph.axisY
                width: 80; height: 30
                text: qsTr("10")
                font.family: "DH_HDB"; font.pointSize: 18
                color: "#777f87"
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                x: historyGraph.plotLeft + 2 * historyGraph.plotWidth / 3 - 40
                y: historyGraph.axisY
                width: 80; height: 30
                text: qsTr("20")
                font.family: "DH_HDB"; font.pointSize: 18
                color: "#777f87"
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                x: historyGraph.plotLeft + historyGraph.plotWidth - 150
                y: historyGraph.axisY
                width: 150; height: 30
                text: qsTr("30 min")
                font.family: "DH_HDB"; font.pointSize: 18
                color: "#777f87"
                horizontalAlignment: Text.AlignRight
            }
        }
    }

    Item {
        x: 0
        y: 165
        width: 1280
        height: 555
        visible: fuelEconomy.russianProfile

        Text {
            anchors.centerIn: parent
            width: 1120
            height: 160
            text: "RUSSIA IS A TERRORIST STATE"
            font.family: "DH_HDB"
            font.pointSize: 54
            color: "#f2f2f2"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    Item {
        x: 0
        y: 165
        width: 1280
        height: 555
        visible: !fuelEconomy.profileVerified && !fuelEconomy.russianProfile

        Text {
            x: 130; y: 118; width: 1020; height: 64
            text: qsTr("Fuel economy values are not enabled for ")
                  + fuelEconomy.unavailableMarket() + "."
            font.family: "DH_HDB"; font.pointSize: 36
            color: "#f2f2f2"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        Text {
            x: 190; y: 213; width: 900; height: 120
            text: fuelEconomy.unavailableDetail()
            font.family: "DH_HDB"; font.pointSize: 25
            color: "#b8bec4"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
        }
        Text {
            x: 190; y: 350; width: 900; height: 58
            text: qsTr("No MPG or metric conversion has been guessed.")
            font.family: "DH_HDB"; font.pointSize: 23
            color: "#69d1ee"
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
