import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1

Component {
    ActionSelectionPopover {
        id: popover
        actions: ActionList {
            Action {
                text: "Mark as Delete"
                onTriggered: {
                    //TODO delete current message
                    TROJITA_MESSAGE_DETAILS.deleteMessage()
                    PopupUtils.close(popover)
                    mainView.goToMailboxPage()
                }
            }
        }

        delegate: ListItem.Standard {
            text: action.text
        }
    }
}
