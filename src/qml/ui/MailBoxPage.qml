import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
Page{
    id: messagesPage
    title: TROJITA_MESSAGES.mailBoxName
    width: mainView.width
    height: mainView.height
    signal messageClicked(int UUID)
    property bool loaded : false;
    ListView{
        anchors.fill: parent;
        model: TROJITA_MESSAGES
        delegate: ListItem.Base{
            Item {
                id: listItemItem
                anchors.verticalCenter: parent.verticalCenter
                anchors.fill: parent
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
                //            onClicked: messagesPage.messageClicked(uid)
            }
        }
    }
    tools: ToolbarItems{
        ToolbarButton{
            action: Action{
                text: "Settings"
                iconSource: Qt.resolvedUrl("../img/settings.svg")
                onTriggered: mainView.goToSettingsPage();
            }
        }
        ToolbarButton{
            action: Action{
                text: "Folders"
                iconSource: Qt.resolvedUrl("../img/browser-tabs.svg")
                onTriggered: mainView.goToFoldersPage();
            }
        }
    }
}
