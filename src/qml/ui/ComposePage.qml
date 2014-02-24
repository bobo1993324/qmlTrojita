import QtQuick 2.0
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import "../components"
Rectangle {
    id: composePage
    color: "#ECEDED"
    property alias toolbar: toolbar
    ListModel{
        id: receiverModel
        ListElement{
            recieverType: "TO"
            value: ""
        }
    }

    Column{
        id: recieversColumn
        width: parent.width
        ListItem.Header{
            text: "Compose"
        }

        Repeater{
            model: receiverModel
            delegate: ListItem.Base{
                Row{
                    width: parent.width
                    spacing: units.gu(2)
                    anchors.verticalCenter: parent.verticalCenter
                    Label{
                        id: typeLabel
                        width: subjectLabel.width
                        anchors.verticalCenter: parent.verticalCenter
                        text: recieverType
                    }
                    TextField{
                        width: parent.width - typeLabel.width - removeRecieverButton.width
                               - ((index == 0) ? parent.spacing : parent.spacing * 2)
                        anchors.verticalCenter: parent.verticalCenter
                        text: value
                        onTextChanged: {
                            receiverModel.get(index).value = text;
                        }
                    }
                    Image{
                        id: removeRecieverButton
                        height: index != 0 ? parent.height/2 : 0
                        width: height
                        anchors.verticalCenter: parent.verticalCenter
                        source: Qt.resolvedUrl("../img/close.svg")
                        MouseArea{
                            anchors.fill: parent
                        }
                    }
                }
            }
        }
        ListItem.Base{
            Row{
                width: parent.width
                spacing: units.gu(2)
                anchors.verticalCenter: parent.verticalCenter
                Label{
                    id: subjectLabel
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Subject"
                }
                TextField{
                    id: subjectTextField
                    width: parent.width - subjectLabel.width - parent.spacing
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }
    TextArea{
        //TODO live richtext editing
        id: contextTextArea
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: recieversColumn.bottom
        anchors.margins: units.gu(1)
        anchors.bottomMargin: isPhone ? units.gu(1) : units.gu(1) + toolbar.height
    }
    Panel{
        id: toolbar
        anchors.bottom: parent.bottom
        width: parent.width
        height: units.gu(8)
        Rectangle{
            id: toolbarRec
            width: parent.width
            height: parent.height
            color: "white"
            ToolbarItems{
                back:ToolbarButton{
                    action: Action{
                        text: "Back"
                        iconSource: Qt.resolvedUrl("../img/back.svg")
                        onTriggered: mainView.goToMailboxPage();
                    }
                }
                ToolbarButton{
                    action: Action{
                        id: sendButton
                        text: "Send"
                        iconSource: Qt.resolvedUrl("../img/email.svg")
                        onTriggered: {
                            //TODO show send mail progress
                            //TODO transfer receiverModel to backend one reciever at a time before send
                            TROJITA_SEND_MAIL.sendMail(receiverModel.get(0).value, subjectTextField.text, contextTextArea.text);
                             mainView.goToMailboxPage();
                        }
                    }
                }

                ToolbarButton{
                    //TODO attachFiles
                    visible: false;
                    action: Action{
                        text: "Attach"
                        iconSource: Qt.resolvedUrl("../img/add.svg")
                    }
                }
                ToolbarButton{
                    //TODO sendmail to multiple recievers
                    visible: false;
                    action: Action{
                        text: "Add reciever"
                        iconSource: Qt.resolvedUrl("../img/add.svg")
                    }
                }
            }
        }
        ToolbarShadow{
        }
    }
}

