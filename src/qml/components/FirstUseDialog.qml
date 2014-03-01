import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

Component {
    id: dialog
    Dialog {
        id: dialogue
        title: "Welcome to qmlTrojita"
        text: "Please start with setting up email accounts"

        Button{
            text: "Go to settings"
            onClicked: {
                PopupUtils.close(dialogue)
                mainView.goToSettingsPage();
            }
        }
    }
}
