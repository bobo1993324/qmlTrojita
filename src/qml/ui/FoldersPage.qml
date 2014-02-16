import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
Page{
    id: foldersPage
    objectName: "mailBoxPage"
    width: mainView.width
    height: mainView.height
    title:"Folders"
    visible: false
    signal mailBoxClicked(string name)
    //TODO gridView
    ListView{
        anchors.fill: parent;
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
                pageStack.pop();
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
}
