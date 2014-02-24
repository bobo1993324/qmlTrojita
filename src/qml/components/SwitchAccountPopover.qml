import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1

Component {
    Popover {
        id: popover
        Column{
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            Repeater{
                model: settingsPage.getAccountNames()
                delegate: ListItem.Standard{
                    text: index == settingsPage.getActiveAccountIndex() ? "<b>" + modelData + "</b>" : modelData
                    onClicked: {
                        settingsPage.setActiveAccountIndex(index);
                        PopupUtils.close(popover)
                    }
                }
            }
        }
    }
}
