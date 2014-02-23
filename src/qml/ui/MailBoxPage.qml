import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
Rectangle{
    id: messagesPage
    objectName: "messagesPage"
    //    title: TROJITA_MESSAGES.mailBoxName
    color: "#ECEDED"
    property alias toolbar: toolbar
    signal messageClicked(int UUID)
    property bool loaded : false;

    Column{
        anchors.fill: parent
        ListItem.Header{
            id: title
            text: TROJITA_MESSAGES.mailBoxName
        }

        ListView{
            width: parent.width
            height: parent.height - title.height
            model: TROJITA_MESSAGES
            clip: true
            delegate: ListItem.Base{
                Rectangle {
                    id: listItemItem
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.fill: parent
                    color: "transparent"


                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            mainView.goToMessagePage();
                            messagesPage.messageClicked(uid);
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
            }
        }
    }
}
