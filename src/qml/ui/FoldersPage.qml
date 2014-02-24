import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import "../components"
Rectangle{
    id: foldersPage
    objectName: "mailBoxPage"
    //TODO show account name
    color: "#ECEDED"

    property alias toolbar: toolbar
    signal mailBoxClicked(string name)
    //TODO gridView

    ListItem.Header{
        id: title
        text: settingsPage.activeAccount
        anchors{
            left: parent.left
            right: parent.right
            top: parent.top
        }
    }
    ListView{
        anchors{
            left: parent.left
            right: parent.right
            top: title.bottom
            bottom: parent.bottom
            bottomMargin: isPhone ? 0 : toolbar.height
        }
        clip: true
        model: TROJITA_MAILBOX
        delegate: ListItem.Standard{
            Label{
                text: name + " (" + unreadCount +")"
                font.bold: unreadCount!=0
                anchors.centerIn: parent
            }

            onClicked: {
                console.log("clicked")
                foldersPage.mailBoxClicked(name)
                mainView.goToMailboxPage();
            }
            ListView.onAdd: {
                //load first mailbox at start
                if(!mailBoxPage.loaded && index == 0){
                    mailBoxPage.loaded = true;
                    foldersPage.mailBoxClicked(name)
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
            id: toolbarRec
            width: parent.width
            height: parent.height
            color: "white"
            ToolbarItems{
                id: toolbarItems
                back:ToolbarButton{
                    action: Action{
                        text: "Back"
                        iconSource: Qt.resolvedUrl("../img/back.svg")
                        onTriggered: mainView.goToMailboxPage();
                    }
                }
            }
        }
        ToolbarShadow{
        }
    }
}
