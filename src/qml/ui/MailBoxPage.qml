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
        delegate: ListItem.Standard{
            text: title
            onClicked: messagesPage.messageClicked(uid)
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
