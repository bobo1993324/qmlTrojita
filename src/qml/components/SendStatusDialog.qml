import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

Component {
    id: dialog
    Dialog {
        id: dialogue
        title: TROJITA_SEND_MAIL.status
        ActivityIndicator{
            running: TROJITA_SEND_MAIL.status === "sending"
            visible : running
        }
        Text{
            text: TROJITA_SEND_MAIL.errorMsg
            visible: TROJITA_SEND_MAIL.status === "error"
        }
        Button{
            text: "Close"
            onClicked: PopupUtils.close(dialogue)
            visible: TROJITA_SEND_MAIL.status !== "sending"
        }
    }
}
