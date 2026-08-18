import QtQuick 1.1

import QmlHomeScreenDef 1.0
import QmlHomeScreenDefPrivate 1.0
import AppEngineQMLConstants 1.0
import QmlStatusBar 1.0

Item {
    id: root
    y:0

    property bool firstWheel: false
    property bool isMainMenu: true
    property bool animation: false
    property int subMenuFocusIndex: 0
    property bool fuelPageOpen: false
    property bool fuelEntryPending: false
    property bool fuelEntryLaunchReturned: false
    property bool fuelEntryFailed: false
    property bool fuelReturnAwaiting: false
    property bool fuelReturnAcknowledged: false
    property bool fuelForeground: true
    property bool fuelSystemPopupVisible: false
    property int fuelPageScreen: 1

    function fuelItemReady() {
        return helpMenuLoader.status == Loader.Ready && helpMenuLoader.item
    }

    function setFuelItemActive(value) {
        if (fuelItemReady()) {
            helpMenuLoader.item.backPressed = false
            helpMenuLoader.item.pageActive = value
        }
    }

    function syncFuelItemActive() {
        setFuelItemActive(fuelPageOpen && fuelForeground
                          && !fuelSystemPopupVisible)
    }

    function closeFuelPage() {
        fuelEntryTimeout.stop()
        setFuelItemActive(false)
        fuelPageOpen = false
        fuelEntryPending = false
        fuelEntryLaunchReturned = false
        fuelEntryFailed = false
        fuelReturnAwaiting = false
        helpMenuLoader.source = ""
        restoreVehicleFocus()
    }

    function restoreVehicleFocus() {
        if (subMenuListLoader.status == Loader.Ready)
            subMenuListLoader.item.setFocusVisible(true)
    }

    function launchFuelPage(appId, viewId, text) {
        if (appId != 58 || viewId != 4 || text != "Fuel Economy")
            return false
        if (fuelPageOpen)
            return true

        fuelPageOpen = true
        fuelEntryPending = true
        fuelEntryLaunchReturned = false
        fuelEntryFailed = false
        fuelReturnAwaiting = false
        fuelReturnAcknowledged = false
        fuelPageScreen = UIListener.getCurrentScreen()
        if (subMenuListLoader.status == Loader.Ready)
            subMenuListLoader.item.setFocusVisible(false)

        // The stock appId 120/viewId 7 path sets native AppEngine state to Help.
        fuelEntryTimeout.start()
        EngineListener.LaunchApplication(120, 7, fuelPageScreen,
                                         ViewControll.GetDisplay(), text)
        fuelEntryLaunchReturned = true
        if (fuelEntryFailed) {
            fuelEntryTimeout.stop()
            fuelEntryPending = false
            recoverFuelPage()
        }
        return true
    }

    function requestFuelBack() {
        if (fuelPageOpen && !fuelReturnAwaiting)
            recoverFuelPage()
    }

    function requestFuelTouchBack() {
        EngineListener.playBeep()
        requestFuelBack()
    }

    function recoverFuelPage() {
        if (!fuelPageOpen || fuelReturnAwaiting)
            return
        var screen = fuelPageScreen
        fuelReturnAwaiting = true
        fuelReturnAcknowledged = false
        EngineListener.LaunchApplication(120, 4, screen,
                                         ViewControll.GetDisplay(), "Vehicle")
        if (!fuelReturnAcknowledged) {
            closeFuelPage()
            applyStockSubMenu(screen, false, 4, 0)
        }
        else {
            fuelReturnAcknowledged = false
        }
    }

    function applyStockSubMenu(screen, animationValue, subMenuId, focusIndex) {
        root.subMenuFocusIndex = focusIndex

        //reload sub menu icon & title
        subMenuLoader.source = ""
        subMenuLoader.source = "DHAVN_AppHomeScreen_Vehicle.qml"

        if(subMenuListLoader.status == Loader.Ready)
        {
            if (root.subMenuFocusIndex != -1)
                subMenuListLoader.item.setFocusIndex(root.subMenuFocusIndex)

            else
                subMenuListLoader.item.setDefaultFocusIndex()
        }

        root.animation = animationValue
        helpMenuLoader.source = ""

        if (!animationValue)
        {
            goSubMenuAni.stop()
            goMainMenuAni.stop()

            bg_image.x = (LocTrigger.arab) ? 0 : -778
            subMenuListLoader.x = 0
            subMenuListLoader.opacity = 1
            subMenuLoader.opacity = 1
            mainMenuItems.opacity = 0
        }

        isMainMenu = false
        statusBar.homeType = "home-button"
    }

    Timer {
        id: fuelEntryTimeout
        interval: 2000
        repeat: false
        onTriggered: {
            if (root.fuelPageOpen && root.fuelEntryPending) {
                root.fuelEntryPending = false
                EngineListener.logForQML("Fuel page load timed out")
                root.recoverFuelPage()
            }
        }
    }

    Image {
        id: bg_image

        source: (LocTrigger.arab) ? "/app/share/images/AppHome/arab/bg_home.png" : View.sBG

        onStatusChanged: {
            if (status == Image.Error)
            {
                EngineListener.logForQML("Image.Error, [Main QML] file name = " + bg_image.source);
            }
        }
    }

    Connections {
        target: LocTrigger

        onArabChanged: {
            if (isMainMenu) goMainMenuNoAni()
            else goSubMenuNoAni()
        }
    }

    SequentialAnimation {
        id: goSubMenuAni
        running: false

        PauseAnimation { duration: 60 }

        ParallelAnimation {
            PropertyAnimation {
                target: bg_image
                property: "x"
                from: (LocTrigger.arab) ? -778 : 0
                to: (LocTrigger.arab) ? 0 : -778
                duration: 180
            }

            PropertyAnimation {
                target: subMenuListLoader
                property: "x"
                from: (LocTrigger.arab) ? -778 : 778
                to: 0
                duration: 180
            }

            PropertyAnimation { target: mainMenuItems; property: "opacity"; from: 1; to: 0; duration: 60 }
            PropertyAnimation { target: subMenuLoader; property: "opacity"; from: 0; to: 1; duration: 180 }
            PropertyAnimation { target: subMenuListLoader; property: "opacity"; from: 0; to: 1; duration: 60 }
        }
    }

    SequentialAnimation {
        id: goMainMenuAni
        running: false

        PauseAnimation { duration: 60 }

        ParallelAnimation {
            PropertyAnimation {
                target: bg_image
                property: "x"
                from:  (LocTrigger.arab) ? 0 : -778
                to: (LocTrigger.arab) ? -778 : 0
                duration: 180
            }

            PropertyAnimation {
                target: subMenuListLoader
                property: "x"
                from: 0
                to: (LocTrigger.arab) ? -778 : 778
                duration: 180
            }

            SequentialAnimation {
                PauseAnimation { duration: 120 }
                PropertyAnimation { target: mainMenuItems; property: "opacity"; from:0; to: 1; duration: 60 }
            }

            PropertyAnimation { target: subMenuLoader; property: "opacity"; from:1; to: 0; duration: 180 }
            PropertyAnimation { target: subMenuListLoader; property: "opacity"; from:1; to: 0; duration: 120 }
            //PropertyAction { target: root; property: "isMainMenu"; value: "true" }

        }
    }

    QmlStatusBar {
        id: statusBar
        x: 0; y: 0; z:0; width: 1280; height: 93
        homeType: "text"
        middleEast: (LocTrigger.arab) ? true : false
    }

    Connections {
        target: UIListener
        onSignalShowSystemPopup: {
            //console.log("onSignalShowSystemPopup")
            ViewControll.bFocusEnabled = false
        }
        onSignalHideSystemPopup: {
            //console.log("onSignalHideSystemPopup")
            ViewControll.bFocusEnabled = true
        }
    }

    Item {
        id: mainMenuItems

        x:0; y:0; width: 1280; height: 720

        Repeater {
            id: iconsRepeater

            property bool bFocused: ( ViewControll.bFocusEnabled && ( EHSDefP.FOCUS_INDEX_ICONS_MENU == View.nFocusIndex ) )

            model: IconsModel
            delegate: DHAVN_AppHomeScreen_VehicleI{}
        }
    }



    Loader {
        id: subMenuLoader
        //source: isMainMenu ? "" : "DHAVN_AppHomeScreen_SubMenu.qml"

        opacity: 0
    }

    Loader {
        id: subMenuListLoader
        //x: 778
        source: isMainMenu ? "" : "DHAVN_AppHomeScreen_VehicleList.qml"

        onStatusChanged: {
            if(status == Loader.Ready)
            {
                subMenuListLoader.item.fuelHost = root

                if (root.subMenuFocusIndex != -1)
                    subMenuListLoader.item.setFocusIndex(root.subMenuFocusIndex)
                else
                    subMenuListLoader.item.setDefaultFocusIndex()

                if (root.animation)
                {
                    goMainMenuAni.stop()
                    goSubMenuAni.running = true
                }
            }
        }
    }

    MouseArea {
        id: fuelInputBlocker
        x: 0; y: 93; width: 1280; height: 627
        visible: root.fuelPageOpen
        enabled: visible
        beepEnabled: false
    }

    Loader {
        id: helpMenuLoader

        //source: "HelpMenu.qml"

        onStatusChanged: {
            if (!root.fuelPageOpen)
                return
            if (status == Loader.Ready) {
                fuelEntryTimeout.stop()
                EngineListener.logForQML("Fuel page QML ready")
                item.countryVariant = UIListener.GetCountryVariantFromQML()
                item.backRequested.connect(root.requestFuelTouchBack)
                root.fuelEntryPending = false
                root.fuelEntryFailed = false
                subMenuLoader.opacity = 0
                subMenuListLoader.opacity = 0
                root.syncFuelItemActive()
            }
            else if (status == Loader.Error) {
                root.fuelEntryFailed = true
                EngineListener.logForQML("Fuel page QML failed to load")
                if (root.fuelEntryLaunchReturned) {
                    fuelEntryTimeout.stop()
                    root.fuelEntryPending = false
                    root.recoverFuelPage()
                }
            }
        }
    }

    Connections {
        target: UIListener

        onSignalJogNavigation: {
            if (!root.fuelPageOpen || !root.fuelItemReady()
                    || !root.fuelForeground || root.fuelSystemPopupVisible)
                return
            if (arrow == UIListenerEnum.JOG_CENTER) {
                if (status == UIListenerEnum.KEY_STATUS_PRESSED)
                    helpMenuLoader.item.backPressed = true
                else if (status == UIListenerEnum.KEY_STATUS_RELEASED) {
                    var activate = helpMenuLoader.item.backPressed
                    helpMenuLoader.item.backPressed = false
                    if (activate)
                        root.requestFuelBack()
                }
                else if (status == UIListenerEnum.KEY_STATUS_CANCELED)
                    helpMenuLoader.item.backPressed = false
            }
        }

        onSignalShowSystemPopup: {
            root.fuelSystemPopupVisible = true
            root.syncFuelItemActive()
            if (root.fuelItemReady())
                helpMenuLoader.item.backPressed = false
        }

        onSignalHideSystemPopup: {
            root.fuelSystemPopupVisible = false
            root.syncFuelItemActive()
            if (root.fuelPageOpen && subMenuListLoader.status == Loader.Ready)
                subMenuListLoader.item.setFocusVisible(false)
        }
    }

    MouseArea {
        id: appLaunching

        x:0; y: 93; width: 1280; height: 720 - 93

        beepEnabled: false

        visible: false
    }

    // popup -->
    Loader {
        id: popUpLoader
        source: ( EHSDefP.POPUP_INVALID != ViewControll.nPopUpType ) ? "DHAVN_AppHomeScreen_PopUp.qml" : ""

        visible: false

        onStatusChanged: {
            if( Loader.Ready == status ) {
                View.nFocusIndex = EHSDefP.FOCUS_INDEX_POPUP
                visible = true
            }
            else {
                View.nFocusIndex = EHSDefP.FOCUS_INDEX_ICONS_MENU
                visible = false
            }
        }

        Connections {
            target: popUpLoader.item
            onPopUpClosed: popUpLoader.unload()
        }
    }

    Connections {
        target: UIListener

        onSignalShowSystemPopup: {
            EngineListener.hideLocalPopup(UIListener.getCurrentScreen())
        }
    }
    // <-- popup

    Connections {
        target: EngineListener

        onGoMainMenu: {
            if(screen == UIListener.getCurrentScreen())
            {
                if (root.fuelPageOpen)
                    root.closeFuelPage()
                root.fuelReturnAcknowledged = false
                root.animation = animation

                helpMenuLoader.source = ""

                if (!animation)
                {
                    goSubMenuAni.stop()
                    goMainMenuAni.stop()

                    bg_image.x = (LocTrigger.arab) ? -778 : 0
                    subMenuListLoader.x = (LocTrigger.arab) ? -778 : 778
                    subMenuListLoader.opacity = 0
                    subMenuLoader.opacity = 0
                    mainMenuItems.opacity = 1

                    isMainMenu = true
                }
                else
                {
                    if (!isMainMenu)
                    {
                        goSubMenuAni.stop()
                        goMainMenuAni.running = true
                        isMainMenu = true
                    }
                }

                statusBar.homeType = "text"
                ViewControll.bJogPressed = false
            }
        }

        onGoSubMenu: {
            if(screen == UIListener.getCurrentScreen())
            {
                if (root.fuelPageOpen && root.fuelReturnAwaiting) {
                    if (subMenuId == 4) {
                        root.closeFuelPage()
                        root.fuelReturnAcknowledged = true
                        root.applyStockSubMenu(screen, false, subMenuId, 0)
                    }
                }
                else if (root.fuelPageOpen && subMenuId == 2) {
                    root.fuelReturnAwaiting = true
                    root.fuelReturnAcknowledged = false
                    EngineListener.LaunchApplication(120, 4, screen,
                                                     ViewControll.GetDisplay(),
                                                     "Vehicle")
                    if (!root.fuelReturnAcknowledged) {
                        root.closeFuelPage()
                        root.applyStockSubMenu(screen, false,
                                               4, 0)
                    }
                    root.fuelReturnAcknowledged = false
                }
                else {
                    if (root.fuelPageOpen)
                        root.closeFuelPage()
                    root.applyStockSubMenu(screen, animation,
                                           subMenuId, focusIndex)
                }
            }
        }

        onGoHelpMenu: {
            if(screen == UIListener.getCurrentScreen())
            {
                statusBar.homeType = "button"
                if (root.fuelPageOpen) {
                    helpMenuLoader.source =
                            "DHAVN_AppHomeScreen_FuelEconomy.qml"
                }
                else {
                    helpMenuLoader.source = "HelpMenu.qml"
                }
            }
        }

        onReceiveBGEvent: {
            if (screen == UIListener.getCurrentScreen()) {
                root.fuelForeground = false
                root.syncFuelItemActive()
            }
        }

        onReceiveFGEvent: {
            if (screen == UIListener.getCurrentScreen()) {
                root.fuelForeground = true
                root.syncFuelItemActive()
            }
        }

        onSigAppLaunching: {
            if (screen == UIListener.getCurrentScreen())
            {
                appLaunching.visible = bLaunching
            }
        }

    }

    Connections {
        target: UIListener

        onSignalJogNavigation:
        {
            if (popUpLoader.status != Loader.Ready && isMainMenu)
            {
                //console.log("DHAVN_AppHomeScreen_Main.qml::JogNavigation: arrow = " + " arrow, status = " + status );

                /*
                            if( status == UIListenerEnum.KEY_STATUS_PRESSED )
                            {
                                switch( arrow )
                                {
                                case UIListenerEnum.JOG_UP:
                                case UIListenerEnum.JOG_DOWN:
                                {
                                    changeFocus(arrow)
                                    break
                                }
                                }
                            }
                            */

                if( status == UIListenerEnum.KEY_STATUS_PRESSED )
                {

                    switch( arrow )
                    {
                    case UIListenerEnum.JOG_WHEEL_LEFT: {
                        if (firstWheel)
                            firstWheel = false
                        else
                            wheelLeft()

                        break
                    }
                    case UIListenerEnum.JOG_WHEEL_RIGHT: {
                        if (firstWheel)
                            firstWheel = false
                        else
                            wheelRight()

                        break
                    }

                    default: break
                    }
                }
            }
        }
    }

    Connections {
        target: UIListener

        onSignalJogCenterPressed: ViewControll.bJogPressed = true;
        onSignalJogCenterReleased: ViewControll.bJogPressed = false;
        onSignalJogNavigation: {
            if( status == UIListenerEnum.KEY_STATUS_PRESSED ) {
                if (!ViewControll.bFocusEnabled &&(titleLoader.status != Loader.Ready ||(titleLoader.item.getFocus() == false))) {
                    ViewControll.bFocusEnabled = true
                    firstWheel = true
                }
            }
        }
    }

    function goMainMenuNoAni()
    {
        goSubMenuAni.stop()
        goMainMenuAni.stop()

        bg_image.x = (LocTrigger.arab) ? -778 : 0
        subMenuListLoader.x = (LocTrigger.arab) ? -778 : 778
        subMenuListLoader.opacity = 0
        subMenuLoader.opacity = 0
        mainMenuItems.opacity = 1
    }

    function goSubMenuNoAni()
    {
        goSubMenuAni.stop()
        goMainMenuAni.stop()

        bg_image.x = (LocTrigger.arab) ? 0 : -778
        subMenuListLoader.x = 0
        subMenuListLoader.opacity = 1
        subMenuLoader.opacity = 1
        mainMenuItems.opacity = 0
    }

    function wheelLeft () {
        if (View.bTitleAvailable == true) {

            if (titleLoader.item.getFocus() == false) {
                if(IconsModel.nFocusIndex == 0)
                {
                    IconsModel.nFocusIndex = IconsModel.nCountItems -1;
                    ViewControll.SetFocusIconIndex( View.nViewId, IconsModel.nFocusIndex )
                }
                else
                {
                    IconsModel.nFocusIndex--;
                    ViewControll.SetFocusIconIndex( View.nViewId, IconsModel.nFocusIndex )
                }
            } else {

                titleLoader.item.setFocus(true);
                ViewControll.SetFocusEnabled(false);
            }

        } else {

            if(IconsModel.nFocusIndex == 0)
            {
                IconsModel.nFocusIndex = IconsModel.nCountItems -1;
                ViewControll.SetFocusIconIndex( View.nViewId, IconsModel.nFocusIndex )
            }
            else
            {
                IconsModel.nFocusIndex--;
                ViewControll.SetFocusIconIndex( View.nViewId, IconsModel.nFocusIndex )
            }
        }
    }

    function wheelRight () {
        if (View.bTitleAvailable == true) {
            if (titleLoader.item.getFocus() == false) {
                if(IconsModel.nFocusIndex == IconsModel.nCountItems - 1 )
                {
                    IconsModel.nFocusIndex = 0;
                    ViewControll.SetFocusIconIndex( View.nViewId, IconsModel.nFocusIndex )
                }
                else
                {
                    IconsModel.nFocusIndex++;
                    ViewControll.SetFocusIconIndex( View.nViewId, IconsModel.nFocusIndex )
                }
            } else {
                titleLoader.item.setFocus(true);
                ViewControll.SetFocusEnabled(false);
            }
        } else {
            if(IconsModel.nFocusIndex == IconsModel.nCountItems - 1 )
            {
                IconsModel.nFocusIndex = 0;
                ViewControll.SetFocusIconIndex( View.nViewId, IconsModel.nFocusIndex )
            }
            else
            {
                IconsModel.nFocusIndex++;
                ViewControll.SetFocusIconIndex( View.nViewId, IconsModel.nFocusIndex )
            }
        }
    }

    function changeFocus( arrow )
    {
        if(arrow == UIListenerEnum.JOG_UP )
        {
            if (View.bTitleAvailable == true) {
                if (titleLoader.item.getFocus() == true) {
                    titleLoader.item.setFocus(true);
                    ViewControll.SetFocusEnabled(false);
                }
                else {
                    titleLoader.item.setFocus(true);
                    ViewControll.SetFocusEnabled(false)
                }
            }
        }

        else if(arrow == UIListenerEnum.JOG_DOWN)
        {
            if (View.bTitleAvailable == true) {
                if (titleLoader.item.getFocus() == true) {

                    titleLoader.item.setFocus(false);
                    ViewControll.SetFocusEnabled(true);

                    if (ViewControll.nTitleAlign == Qt.AlignLeft) {
                        switch (IconsModel.nCountItems ){
                        case 1:
                        case 2:
                        case 3: IconsModel.nFocusIndex = 0; break;
                        case 4:
                        case 5: IconsModel.nFocusIndex = 1; break;
                        case 6:
                        case 7: IconsModel.nFocusIndex = 2; break;
                        case 8:
                        case 9: IconsModel.nFocusIndex = 3; break;
                        case 10: IconsModel.nFocusIndex = 4; break;
                        }
                    } else {
                        IconsModel.nFocusIndex = 0;
                    }
                }
                ViewControll.SetFocusIconIndex( View.nViewId, IconsModel.nFocusIndex )
            }
        }
    }
}
