import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1
import "../components"
Rectangle{
    id: messagesPage
    objectName: "messagesPage"
    //    title: TROJITA_MESSAGES.mailBoxName
    color: "#ECEDED"

    signal messageClicked(int UUID)

    property alias toolbar: toolbar
    property bool loaded : false;

    Column{
        anchors.fill: parent
        ListItem.Header{
            id: title
            text: TROJITA_MESSAGES.mailBoxName
        }

        ListView{
            id: messagesView
            width: parent.width
            //toolbar.height is locked and opened in desktop mode
            height: parent.height - title.height - (isPhone ? 0 : toolbar.height) - (mailLoading.running ? mailLoading.height : 0)
            model: TROJITA_MESSAGES
            clip: true
            property var lastYTime: 0//remove vibartion
            onAtYEndChanged: {
                var time = new Date()
                if(time.getTime() - lastYTime > 1000){
                    loadMore();
                }
            }
            function loadMore(){
                if(messagesView.atYEnd && TROJITA_MESSAGES.messageCount >= TROJITA_MESSAGES.displayCount){
                    messagesView.lastYTime = new Date().getTime()
                    mailLoading.running = true
                    TROJITA_MESSAGES.displayCount += 20
                    yAtEndTimer.start()
                }
            }

            Timer{
                id: yAtEndTimer
                onTriggered: {
                    messagesView.loadMore()
                }

            }

            delegate: ListItem.Base{
                Rectangle {
                    id: listItemItem
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.fill: parent
                    color: "transparent"

                    Component.onCompleted: {
                        mailLoading.running = false;
                    }

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if(!toolbar.animating){
                                mainView.goToMessagePage();
                                messagesPage.messageClicked(uid);
                            }
                        }
                    }
                    Column {
                        id: mailColumn
                        anchors.verticalCenter: parent.verticalCenter
                        width : listItemItem.width
                        Rectangle{
                            width: parent.width
                            height: fromLabel.height
                            color: "transparent"
                            Label {
                                text: subject
                                font.bold : unread
                                anchors.left: parent.left
                                width: parent.width- favouriteImage.width-units.gu(2)
                                clip: true
                            }
                            Image{
                                id: favouriteImage
                                source: star? Qt.resolvedUrl("../img/favorite-selected.svg")
                                            : Qt.resolvedUrl("../img/favorite-unselected.svg")
                                height: parent.height
                                width: height
                                anchors.right: parent.right
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: {
                                        if(!toolbar.animating){
                                            favouriteImage.source = !star? Qt.resolvedUrl("../img/favorite-selected.svg")
                                                                         : Qt.resolvedUrl("../img/favorite-unselected.svg")
                                            console.log("star clicked");
                                            TROJITA_MESSAGES.setStar(uid, !star)
                                        }
                                    }
                                }
                            }
                        }


                        Rectangle{
                            width: parent.width
                            height: fromLabel.height
                            color: "transparent"
                            Label {
                                id: fromLabel
                                anchors.left: parent.left
                                text: 'From: ' + sender
                            }
                            Label {
                                anchors.right: parent.right
                                text: time
                            }
                        }
                    }
                }

            }
        }
        ActivityIndicator{
            id: mailLoading
            anchors.horizontalCenter: parent.horizontalCenter
            visible: running
            running: false
        }
    }
    Panel{
        id: toolbar
        anchors.bottom: parent.bottom
        width: parent.width
        height: units.gu(8)
        Rectangle{
            width: parent.width
            height: parent.height
            color: "white"
            ToolbarItems{
                id: toolbarItems
                ToolbarButton{
                    action: Action{
                        text: "Settings"
                        iconSource: Qt.resolvedUrl("../img/settings.svg")
                        onTriggered: {
                            mainView.goToSettingsPage();
                        }
                    }
                }
                ToolbarButton{
                    action: Action{
                        text: "Folders"
                        iconSource: Qt.resolvedUrl("../img/browser-tabs.svg")
                        onTriggered: {
                            mainView.goToFoldersPage();
                        }
                    }
                }
                ToolbarButton{
                    action: Action{
                        text: "Compose"
                        iconSource: Qt.resolvedUrl("../img/compose.svg")
                        onTriggered: {
                            mainView.goToComposePage();
                        }
                    }
                }
                ToolbarButton{
                    id: switchAccountButton
                    action: Action{
                        text: "Accounts"
                        iconSource: Qt.resolvedUrl("../img/switch.svg")
                        onTriggered: {
                            PopupUtils.open(switchAccountPopover, switchAccountButton)
                        }
                    }
                    Rectangle{
                        visible: TROJITA_ALERT.hasAlert
                        color: "red"
                        x: parent.width * 2 / 3
                        y: parent.height / 4
                        width: units.gu(1)
                        height: width
                        radius: units.gu(0.5)
                        Connections{
                            target: TROJITA_ALERT
                            onHasAlertChanged: if(TROJITA_ALERT.hasAlert) toolbar.open()
                        }
                    }
                }
            }
        }
        ToolbarShadow{
        }
    }
    SwitchAccountPopover{
        id: switchAccountPopover
    }
}
