import QtQuick 2.0
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1
import "../components"
Rectangle {
    id: composePage
    color: "#ECEDED"
    property alias toolbar: toolbar
    property alias subject: subjectTextField.text
    property alias content: contextTextArea.text
    property alias to: toTextField.text
    property alias cc: ccTextField.text
    property alias bcc: bccTextField.text
    property bool showCcBcc: false

    HintPopover{
        id: hintPopover
    }
    SendStatusDialog{
        id: sendStatusDialog
    }

    Column{
        id: recieversColumn
        width: parent.width
        ListItem.Header{
            text: "Compose"
        }
        ListItem.Base{
            Row{
                width: parent.width
                spacing: units.gu(2)
                anchors.verticalCenter: parent.verticalCenter
                Label{
                    anchors.verticalCenter: parent.verticalCenter
                    text: "To"
                    width: subjectLabel.width
                }
                TextField{
                    id: toTextField
                    width: parent.width - subjectLabel.width - parent.spacing * 2 - toHelpImage.width
                    anchors.verticalCenter: parent.verticalCenter
                }
                Image {
                    id: toHelpImage
                    source: Qt.resolvedUrl("../img/help.svg")
                    height: parent.height * 2 / 3
                    width: height
                    anchors.verticalCenter: parent.verticalCenter
                    MouseArea{
                        anchors.fill: parent
                        onClicked:{
                            mainView.hintText = "To add multiple accounts, \nseparate with ','"
                            PopupUtils.open(hintPopover, toHelpImage)
                        }
                    }
                }
            }
        }
        ListItem.Base{
            id: cclib
            visible: showCcBcc
            Row{
                width: parent.width
                spacing: units.gu(2)
                anchors.verticalCenter: parent.verticalCenter
                Label{
                    anchors.verticalCenter: parent.verticalCenter
                    text: "CC"
                    width: subjectLabel.width
                }
                TextField{
                    id: ccTextField
                    width: parent.width - subjectLabel.width - parent.spacing
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
        ListItem.Base{
            id: bcclib
            visible: showCcBcc
            Row{
                width: parent.width
                spacing: units.gu(2)
                anchors.verticalCenter: parent.verticalCenter
                Label{
                    anchors.verticalCenter: parent.verticalCenter
                    text: "BCC"
                    width: subjectLabel.width
                }
                TextField{
                    id: bccTextField
                    width: parent.width - subjectLabel.width - parent.spacing
                    anchors.verticalCenter: parent.verticalCenter
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
                            //check  the validilty of emailAddresses
                            if (toTextField.text == ""){
                                mainView.hintText = "To is blank";
                                PopupUtils.open(hintPopover);
                            } else if (TROJITA_SEND_MAIL.isAddrCorrect(toTextField.text)
                                    && (ccTextField.text === "" || TROJITA_SEND_MAIL.isAddrCorrect(ccTextField.text))
                                    && (bccTextField.text === "" || TROJITA_SEND_MAIL.isAddrCorrect(bccTextField.text))){
                                PopupUtils.open(sendStatusDialog)
                                TROJITA_SEND_MAIL.sendMail(toTextField.text, subjectTextField.text, contextTextArea.text, ccTextField.text, bccTextField.text);
                                mainView.goToMailboxPage();
                            } else {
                                mainView.hintText = TROJITA_SEND_MAIL.errorMsg;
                                PopupUtils.open(hintPopover);
                            }
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
                    action: Action{
                        text: "Add CC/BCC"
                        visible: !showCcBcc
                        iconSource: Qt.resolvedUrl("../img/add.svg")
                        onTriggered:{
                            showCcBcc = true;
                        }
                    }
                }
            }
        }
        ToolbarShadow{
        }
    }
}

