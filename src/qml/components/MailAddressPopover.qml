import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1

Component {
    Popover {
        id: popover

        Column{
            width: parent.width
            ListItem.Header{
                text: mainView.mAPAddress
                visible: mainView.mAPName != "";
            }

            ListItem.Standard {
                //TODO wait for address-book-app interface to catchup
                text: "Add to a new contact"
                visible: false
            }
            ListItem.Standard {
                //TODO wait for address-book-app interface to catchup
                text: "Add to an existing contact"
                visible: false
                onClicked: {
                    PopupUtils.close(popover)
                    PopupUtils.open(selectContactSheet)
                }
            }
            ListItem.ThinDivider{
                visible: false
            }
            ListItem.Standard{
                text: "Write to"
                onClicked: {
                    composePage.reset()
                    composePage.to = mainView.mAPAddress
                    TROJITA_SEND_MAIL.prepare();
                    mainView.goToComposePage()
                    PopupUtils.close(popover)
                }
            }
        }
    }
}
