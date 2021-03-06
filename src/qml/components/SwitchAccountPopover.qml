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
                    text: index === settingsPage.getActiveAccountIndex() ? "<b>" + modelData + "</b>" : modelData
                    onClicked: {
                        //reload inbox
                        settingsPage.setActiveAccountIndex(index);
                        PopupUtils.close(popover)
                    }
                    Rectangle{
                        visible: TROJITA_ALERT.getAlertAccount(modelData)
                        color: "red"
                        anchors{
                            verticalCenter: parent.verticalCenter
                            right: parent.right
                            rightMargin: width
                        }
                        width: units.gu(2)
                        height: width
                        radius: width / 2
                    }
                }
            }
        }
    }
}
