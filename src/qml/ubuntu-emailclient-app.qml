import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Extras.Browser 0.1

import "components"
/*!
    \brief MainView with Tabs element.
           First Tab has a single Label and
           second Tab has a single ToolbarAction.
*/

MainView {

    width: units.gu(50)
    height: units.gu(75)

    PageStack{
        id: pageStack
        Component.onCompleted: {
            push(mainWindow)
//            push(loginPage)
            push(writePage)
        }
        Page {
            id: loginPage
            objectName: "settingPage"
            title: "Login"
            visible: false

            signal settingChanged;
            Column {
                anchors.fill: parent

                ListItem.Standard {
                    text: "Account"
                    control: TextField {
                        id: userNameTf
                        onTextChanged: {
                            TROJITA_SETTING.userName=userNameTf.text
                        }
                    }
                }
                ListItem.Standard {
                    text: "Password"
                    control: TextField {
                        id: passwordTf
                        onTextChanged: {
                            TROJITA_SETTING.setPassword(passwordTf.text)
                        }
                    }
                }
                ListItem.Standard {
                    control: Button {
                        text: "Login"
                        onClicked: {
                            print("Clicked")
                            console.log("user name  is "+TROJITA_SETTING.userName);
                            loginPage.settingChanged();
                            pageStack.pop();
                        }
                    }
                }
            }
        }
        Tabs{
            id: mainWindow
            selectedTabIndex: 3
            Tab{
                title:"Mail Boxes"
                page: Page{
                    id: mailBoxPage
                    objectName: "mailBoxPage"
                    signal mailBoxClicked(string name)
                    ListView{
                        anchors.fill: parent;
                        model: TROJITA_MAILBOX
                        delegate: ListItem.Standard{
                            text: name + " " + hasChildren
                            onClicked: mailBoxPage.mailBoxClicked(name)
                        }
                    }
                }
            }
            Tab{
                title: "Mail Box"
                page: Page{
                    id: messagesPage
                    objectName: "messagesPage"
                    signal messageClicked(int UUID)
                    ListView{
                        anchors.fill: parent;
                        model: TROJITA_MESSAGES
                        delegate: ListItem.Standard{
                            text: title
                            onClicked: messagesPage.messageClicked(uid)
                        }
                    }
                }
            }
            Tab{
                title: "Message"
                page: Page{
                    UbuntuWebView{
                        width: parent.width
                        height: parent.height
                        id: messageWebview
                        anchors.centerIn: parent.center
                        Connections{
                            target: TROJITA_MESSAGE_DETAILS
                            onContentChanged: {
                                messageWebview.loadHtml(TROJITA_MESSAGE_DETAILS.content)
                            }
                        }
                    }
                }
            }
            Tab{
                title: "Attachments"
                page: Page{
                    id: attachmentPage
                    objectName: "attachmentPage"
                    signal attachmentClicked(string fileName)
                    ListView{
                        anchors.fill: parent;
                        model: TROJITA_ATTACHMENTS
                        delegate: ListItem.Standard{
                            text: fileName
                            onClicked: {
                                console.log(index + " clicked")
                                attachmentPage.attachmentClicked(index)
                            }
                        }
                    }
                }
            }
        }
        Page {
            id: writePage
            title: "Write"
            visible: false
            Column{
                anchors.fill: parent
                Row{
                    Label{
                        text: "To"
                    }
                    TextField{
                        id: sendToTextField
                    }
                }

                TextArea{
                    id: contentTextArea
                    width: parent.width
                }
            }
            tools: ToolbarItems {
                ToolbarButton {
                    action: Action {
                        text: "button"
                        iconName: "compose"
                        onTriggered: TROJITA_SEND_MAIL.sendMail(sendToTextField.text, contentTextArea.text);
                    }
                }
            }
        }
    }
}
