import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
UbuntuShape {
    id: mailAddressBox
    width: label.width + units.gu(2)
    height: label.height + units.gu(2)
    property string name;
    property string address;
    property bool isMe: settingsPage.activeAccount == address
    Label{
        id: label
        text:  isMe ? "Me" : (name != "" ? name : address)
        anchors.centerIn: parent
    }
    color: style.mailAddressNotSavedColor
    MouseArea{
        anchors.fill: parent
        onClicked: {
            if(!isMe){
                mainView.mAPAddress = address;
                mainView.mAPName = name;
                PopupUtils.open(mailAddressPopover, mailAddressBox)
            }
        }
    }
}
