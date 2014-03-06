
import QtQuick 2.0
import QtQuick.Window 2.0
import QtContacts 5.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1
import Ubuntu.Contacts 0.1
import QtContacts 5.0

//import QtQuick 2.0
//import Ubuntu.Components.Popups 0.1
//import Ubuntu.Contacts 0.1
//import QtContacts 5.0

Component {
    DefaultSheet {
        // FIXME: workaround to set the contact list
        // background to black
        Rectangle {
            anchors.fill: parent
            anchors.margins: -units.gu(1)
            color: "#221e1c"
        }
        id: sheet
        title: "Select Contact"
        doneButton: false
        modal: true
        contentsHeight: parent.height
        contentsWidth: parent.width
        ContactListView {
            anchors.fill: parent
            onContactClicked: {
                console.log("contact clicked");
             //TODO add email to the contact clicked

                // FIXME: search for favorite number
//                multiRecipient.addRecipient(contact.phoneNumber.number)
//                multiRecipient.forceActiveFocus()
//                PopupUtils.close(sheet)
            }
        }
        onDoneClicked: PopupUtils.close(sheet)
    }
}
