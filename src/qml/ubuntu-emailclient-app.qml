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
    width: units.gu(150)
    height: units.gu(75)
    //This empty page disable default toolbar
    property bool isPhone: width < units.gu(100)
    property int leftPanelWidth:units.gu(50)
    property int seperatorWidth: units.gu(0.5)
    Component.onCompleted: {
        if(isPhone){
            closeAndUnlockToolbars();
        }else{
            openAndLockToolbars();
        }
    }

    onIsPhoneChanged: {
        if(isPhone){
            closeAndUnlockToolbars();
        }else{
            openAndLockToolbars();
        }
    }

    property string currentPage : "mailbox"
    Page{
        anchors.fill: parent
        MailBoxPage{
            id: mailBoxPage
            width: isPhone ? parent.width :leftPanelWidth
            height: parent.height
            x: (currentPage == "mailbox" || (currentPage == "message" && !isPhone)) ? 0 : parent.width
        }
        FoldersPage{
            id: foldersPage
            //hide at start
            width: isPhone ? parent.width :leftPanelWidth
            height: parent.height
            x: currentPage == "folders" ? 0 : -parent.width
            Behavior on x {
                UbuntuNumberAnimation{}
            }
        }
        MessagePage{
            id: messagePage
            width: isPhone ? parent.width : parent.width -leftPanelWidth -seperatorWidth
            height: parent.height
            x: (currentPage == "message" && isPhone )?
                   0 : (!isPhone && (currentPage == "message" || currentPage == "mailbox" || currentPage == "folders")) ?
                      leftPanelWidth +seperatorWidth : parent.width
            Behavior on x {
                UbuntuNumberAnimation{}
            }
        }
        SettingsPage{
            id: settingsPage
            width: parent.width
            height: parent.height
            x:  currentPage == "settings" ? 0 : -parent.width
            Behavior on x {
                UbuntuNumberAnimation{}
            }
        }
        //implement as a popover if
        ComposePage{
            id: composePage
            width: parent.width
            height: parent.height
            x: currentPage == "compose" ? 0 : parent.width
            Behavior on x {
                UbuntuNumberAnimation{}
            }
        }
        Rectangle{
            id: separator
            color: UbuntuColors.coolGrey
            width:seperatorWidth
            height: parent.height
            visible: !isPhone && (currentPage == "folders" || currentPage == "mailbox" || currentPage == "message")
            x:leftPanelWidth
        }
    }
    function goToMailboxPage(){
        currentPage = "mailbox"
        if(isPhone) closeAllToolbars();
    }

    function goToComposePage(){
        currentPage = "compose"
        if(isPhone) closeAllToolbars();
    }

    function goToFoldersPage(){
        currentPage = "folders"
        if(isPhone) closeAllToolbars();
    }

    function goToSettingsPage(){
        currentPage = "settings"
        if(isPhone) closeAllToolbars();
    }

    function goToMessagePage(){
        currentPage = "message"
        if(isPhone) closeAllToolbars();
    }
    //opened toolbars will grab mouse drag from listview
    function closeAllToolbars(){
        mailBoxPage.toolbar.close();
        composePage.toolbar.close();
        foldersPage.toolbar.close();
        settingsPage.toolbar.close();
        messagePage.toolbar.close();
    }

    function openAndLockToolbars(){
        mailBoxPage.toolbar.open();
        foldersPage.toolbar.open();
        messagePage.toolbar.open();
        settingsPage.toolbar.open();
        composePage.toolbar.open();

        mailBoxPage.toolbar.locked = true;
        foldersPage.toolbar.locked = true;
        messagePage.toolbar.locked = true;
        settingsPage.toolbar.locked = true;
        composePage.toolbar.locked = true;
    }

    function closeAndUnlockToolbars(){

        mailBoxPage.toolbar.close();
        foldersPage.toolbar.close();
        messagePage.toolbar.close();
        settingsPage.toolbar.close();
        composePage.toolbar.close();

        mailBoxPage.toolbar.locked = false;
        foldersPage.toolbar.locked = false;
        messagePage.toolbar.locked = false;
        settingsPage.toolbar.locked = false;
        composePage.toolbar.locked = false;
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
