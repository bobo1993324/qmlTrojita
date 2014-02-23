import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import "ui"
/*!
    \brief MainView with Tabs element.
           First Tab has a single Label and
           second Tab has a single ToolbarAction.
*/

MainView {
    id: mainView
    width: units.gu(50)
    height: units.gu(75)
    //This empty page disable default toolbar
    Page{
        anchors.fill: parent
        MailBoxPage{
            id: mailBoxPage
            width: parent.width
            height: parent.height
        }
        FoldersPage{
            id: foldersPage
            //hide at start
            width: parent.width
            height: parent.height
            x: -parent.width
            Behavior on x {
                UbuntuNumberAnimation{}
            }
        }
        MessagePage{
            id: messagePage
            width: parent.width
            height: parent.height
            x: parent.width
            Behavior on x {
                UbuntuNumberAnimation{}
            }
        }
        SettingsPage{
            id: settingsPage
            width: parent.width
            height: parent.height
            x: -parent.width
            Behavior on x {
                UbuntuNumberAnimation{}
            }
        }
        ComposePage{
            id: composePage
            width: parent.width
            height: parent.height
            x: parent.width
            Behavior on x {
                UbuntuNumberAnimation{}
            }
        }
    }
    function goToMailboxPage(){
        closeAllToolbars();
        foldersPage.x = -mainView.width
        messagePage.x = mainView.width
        settingsPage.x = -mainView.width
        composePage.x = mainView.width
        mailBoxPage.x = 0
    }

    function goToComposePage(){
        closeAllToolbars();
        composePage.x = 0;
    }

    function goToFoldersPage(){
        closeAllToolbars();
        foldersPage.x = 0;
    }

    function goToSettingsPage(){
        closeAllToolbars();
        settingsPage.x = 0;
    }

    function goToMessagePage(){
        closeAllToolbars();
        messagePage.x = 0;
    }
    //opened toolbars will grab mouse drag from listview
    function closeAllToolbars(){
        mailBoxPage.toolbar.close();
        composePage.toolbar.close();
        foldersPage.toolbar.close();
        settingsPage.toolbar.close();
        messagePage.toolbar.close();
    }

    //        Tabs{
    //            id: mainTabs
    //            selectedTabIndex: 3
    //            Tab{
    //                title: "Login"
    //                page:Page {
    //                    id: loginPage
    //                    objectName: "settingPage"

    //                    signal settingChanged;
    //                    Column {
    //                        anchors.fill: parent

    //                        ListItem.Standard {
    //                            text: "Account"
    //                            control: TextField {
    //                                id: userNameTf
    //                                onTextChanged: {
    //                                    TROJITA_SETTING.userName=userNameTf.text
    //                                }
    //                            }
    //                        }
    //                        ListItem.Standard {
    //                            text: "Password"
    //                            control: TextField {
    //                                id: passwordTf
    //                                onTextChanged: {
    //                                    TROJITA_SETTING.setPassword(passwordTf.text)
    //                                }
    //                            }
    //                        }
    //                        ListItem.Standard {
    //                            control: Button {
    //                                text: "Login"
    //                                onClicked: {
    //                                    print("Clicked")
    //                                    console.log("user name  is "+TROJITA_SETTING.userName);
    //                                    loginPage.settingChanged();
    //                                    //                                    pageStack.pop();
    //                                }
    //                            }
    //                        }
    //                    }
    //                }
    //            }

    //            Tab{
    //                title:"Mail Boxes"
    //                page: Page{
    //                    id: mailBoxPage
    //                    objectName: "mailBoxPage"
    //                    signal mailBoxClicked(string name)
    //                    ListView{
    //                        anchors.fill: parent;
    //                        model: TROJITA_MAILBOX
    //                        delegate: ListItem.Standard{
    //                            text: name + " " + hasChildren
    //                            onClicked: mailBoxPage.mailBoxClicked(name)
    //                        }
    //                    }
    //                }
    //            }
    //            Tab{
    //                title: "Mail Box"
    //                page: Page{
    //                    id: messagesPage
    //                    objectName: "messagesPage"
    //                    signal messageClicked(int UUID)
    //                    ListView{
    //                        anchors.fill: parent;
    //                        model: TROJITA_MESSAGES
    //                        delegate: ListItem.Standard{
    //                            text: title
    //                            onClicked: messagesPage.messageClicked(uid)
    //                        }
    //                    }
    //                }
    //            }
    //            Tab{
    //                title: "Message"
    //                page: Page{
    //                    UbuntuWebView{
    //                        width: parent.width
    //                        height: parent.height
    //                        id: messageWebview
    //                        anchors.centerIn: parent.center
    //                        Connections{
    //                            target: TROJITA_MESSAGE_DETAILS
    //                            onContentChanged: {
    //                                messageWebview.loadHtml(TROJITA_MESSAGE_DETAILS.content)
    //                            }
    //                        }
    //                    }
    //                }
    //            }
    //            Tab{
    //                title: "Attachments"
    //                page: Page{
    //                    id: attachmentPage
    //                    objectName: "attachmentPage"
    //                    signal attachmentClicked(string fileName)
    //                    ListView{
    //                        anchors.fill: parent;
    //                        model: TROJITA_ATTACHMENTS
    //                        delegate: ListItem.Standard{
    //                            text: fileName
    //                            onClicked: {
    //                                console.log(index + " clicked")
    //                                attachmentPage.attachmentClicked(index)
    //                            }
    //                        }
    //                    }
    //                }
    //            }
    //            Tab{
    //                title: "Write"
    //                page:Page {
    //                    id: writePage
    ////                    visible: false
    //                    Column{
    //                        anchors.fill: parent
    //                        Row{
    //                            Label{
    //                                text: "To"
    //                            }
    //                            TextField{
    //                                id: sendToTextField
    //                            }
    //                        }

    //                        TextArea{
    //                            id: contentTextArea
    //                            width: parent.width
    //                        }
    //                    }
    //                    tools: ToolbarItems {
    //                        ToolbarButton {
    //                            action: Action {
    //                                text: "button"
    //                                iconName: "compose"
    //                                onTriggered: TROJITA_SEND_MAIL.sendMail(sendToTextField.text, contentTextArea.text);
    //                            }
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //    }
}
