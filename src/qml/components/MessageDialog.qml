import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

Component {
    id: dialog
    Dialog {
        id: dialogue
        title: TROJITA_ALERT.errorAccount
        text:  TROJITA_ALERT.errorMsg


        Button{
            text: "Close"
            onClicked: {
                PopupUtils.close(dialogue)
                mainView.messageDialogOpened = false;
            }
        }
    }
}
