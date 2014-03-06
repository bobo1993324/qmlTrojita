import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1
import "ui"
import "components"
/*!
    \brief MainView with Tabs element.
           First Tab has a single Label and
           second Tab has a single ToolbarAction.
*/

MainView {
    id: mainView
    objectName: "mainView"
    width: units.gu(50)
    height: units.gu(75)
    applicationName: "com.bobo-324.qmltrojita"
    //This empty page disable default toolbar
    property bool isPhone: width < units.gu(100)

    property int leftPanelWidth:units.gu(50)
    property int seperatorWidth: units.gu(0.5)

    property string currentPage : "mailbox"

    property string hintText
    property bool messageDialogOpened: false

    property string mAPAddress // content passed to MailAddressPopover
    property string mAPName

    MessageDialog{
        id: messageDialog
    }
    FirstUseDialog{
        id: firstUseDialog
    }

    SelectContactSheet{
        id: selectContactSheet
    }

    Style{
        id: style
    }

    Connections{
        target: TROJITA_ALERT
        onAuthenticateFailed:{
            //prevent trigger multiple times
            if(!messageDialogOpened){
                messageDialogOpened = true
                PopupUtils.open(messageDialog)
            }
        }
    }

    signal unReadCountUpdated(string account, int newUnreadCount);

    Component.onCompleted: {
        if(isPhone){
            closeAndUnlockToolbars();
        }else{
            openAndLockToolbars();
        }

        //with no account setup, open a Dialog that guides user to settings
        if(!settingsPage.activeAccount || settingsPage.activeAccount == ""){
            PopupUtils.open(firstUseDialog);
        }
    }

    onIsPhoneChanged: {
        if(isPhone){
            closeAndUnlockToolbars();
        }else{
            openAndLockToolbars();
        }
    }

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
}
