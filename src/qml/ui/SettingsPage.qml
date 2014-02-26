import QtQuick 2.0
import U1db 1.0 as U1db
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import "../js/MailConfig.js" as MailConfig
import "../components"
Rectangle{
    id: settingsPage
    objectName: "settingPage"
    color: "#ECEDED"
    property alias toolbar: toolbar

    signal settingChanged;
    signal addingAccount(var s);
    signal setCurrentAccount(int idx);

    property string activeAccount: accountsDocument.contents.accounts[accountsDocument.contents.currentAccountIndex]["imap.auth.user"]
    function getAccountNames(){
        var returnVal = [];
        for ( var i in accountsDocument.contents.accounts){
            returnVal.push(accountsDocument.contents.accounts[i]["imap.auth.user"])
        }
        return returnVal;
    }

    function getActiveAccountIndex(){
        return accountsDocument.contents.currentAccountIndex;
    }
    function setActiveAccountIndex(index){
        accountsDocument.setCurrentIndex(index);
    }

    U1db.Database{
        id: accountsDatabase
        path: "accounts.u1db"
    }

    U1db.Document{
        id: accountsDocument
        objectName: "accountsDocument"
        database: accountsDatabase
        docId: "accounts"
        create: true
        defaults: {
            "currentAccountIndex": -1,
                    "accounts":[]
        }

        function setCurrentIndex(idx){
            var tmp = contents
            tmp.currentAccountIndex = idx
            contents = tmp
            settingsPage.setCurrentAccount(idx)
        }
        function removeAccount(idx){
            var tmp = contents
            if(contents.currentAccountIndex>=idx){
                tmp.currentAccountIndex--
            }
            tmp.accounts.splice(idx, 1);
            contents=tmp;
        }
        function addAccount(account){
            var tmp = contents
            tmp.accounts.push(account);
            if(contents.accounts.length ===0){
                tmp.currentAccountIndex=0
            }
            contents=tmp;
            settingsPage.addingAccount(account);
            if(contents.accounts.length === 1){
                settingsPage.setCurrentAccount(0);
            }
        }

//        function updateSettings(){
//            if(contents.currentAccountIndex <0)
//                return;
//            for (var i in contents.accounts[contents.currentAccountIndex]){
//                TROJITA_SETTING.updateSetting(i, contents.accounts[contents.currentAccountIndex][i]);
//            }
//            //flag to automatically load first mailbox
//            mailBoxPage.loaded=false;
//            settingsPage.settingChanged();
//        }

        Component.onCompleted: {
            addAccountTimer.start()
        }
    }

    //need to wait for the connect signal to work
    Timer {
        id: addAccountTimer
        interval: 200
        running: false
        repeat: false
        onTriggered: {
            for (var i in accountsDocument.contents.accounts){
                addingAccount(accountsDocument.contents.accounts[i]);
            }
            setCurrentAccount(accountsDocument.contents.currentAccountIndex);
        }
    }

    state: "NORMAL"
    states: [
        State{
            name: "NORMAL"
            PropertyChanges {
                target: addButton
                visible: true
            }
            PropertyChanges {
                target: emailAddressLI
                visible: false
            }
            PropertyChanges{
                target: passwordLI
                visible: false
            }
            PropertyChanges{
                target: addButtons
                visible: false;
            }
        },
        State{
            name: "ADDING_ACCOUNT"
            PropertyChanges {
                target: addButton
                visible: false
            }
            PropertyChanges {
                target: emailAddressLI
                visible: true
            }
            PropertyChanges{
                target: passwordLI
                visible: true
            }
            PropertyChanges{
                target: addButtons
                visible: true;
            }
        }
    ]
    Column{
        width: parent.width
        height: parent.height
        ListItem.Header{
            text: "Current account"
        }
        ListItem.Standard{
            id: currentAccount
            visible: accountsDocument.contents.currentAccountIndex != -1
            text: visible ? accountsDocument.contents.accounts[accountsDocument.contents.currentAccountIndex]["imap.auth.user"] : ""
            control:Image{
                height: currentAccount.height/2
                width: height
                anchors.verticalCenter: parent.verticalCenter
                source: Qt.resolvedUrl("../img/close.svg")
                MouseArea{
                    anchors.fill: parent
                    onClicked: accountsDocument.removeAccount(accountsDocument.contents.currentAccountIndex)
                }
            }
        }
        ListItem.Header{
            text: "Available accounts"
        }
        Repeater{
            model:accountsDocument.contents.accounts
            delegate: ListItem.Standard{
                visible: index !== accountsDocument.contents.currentAccountIndex
                text: modelData["imap.auth.user"]
                control:Row{
                    height: currentAccount.height
                    spacing: units.gu(2)
                    Image{
                        height: parent.height/2
                        width: height
                        anchors.verticalCenter: parent.verticalCenter
                        source: Qt.resolvedUrl("../img/select.svg")
                        MouseArea{
                            anchors.fill: parent
                            onClicked: accountsDocument.setCurrentIndex(index)
                        }
                    }
                    Image{
                        height: parent.height/2
                        width: height
                        anchors.verticalCenter: parent.verticalCenter
                        source: Qt.resolvedUrl("../img/close.svg")
                        MouseArea{
                            anchors.fill: parent
                            onClicked: accountsDocument.removeAccount(index)
                        }
                    }
                }
            }
        }
        ListItem.SingleControl{
            id: addButton
            control: Button{
                text: "Add more account"
                onClicked: {
                    settingsPage.state="ADDING_ACCOUNT"
                }
            }
        }
        ListItem.Standard{
            //TODO check email address is valid
            id: emailAddressLI
            text: "Email address"
            control: TextField{
                id: emailTextField
            }
        }
        ListItem.Standard{
            id: passwordLI
            text: "Password"
            control: TextField{
                id: passwordTextField
                echoMode: TextInput.Password
            }
        }
        ListItem.SingleControl{
            id: addButtons
            control:Row{
                height: currentAccount.height
                spacing: units.gu(2)
                Button{
                    text: "Add"
                    onClicked:{
                        settingFetchingIndicator.running = true;
                        MailConfig.fetchConfiguration(
                                    emailTextField.text,
                                    passwordTextField.text,
                                    function(account){
                                        //                                        console.log(JSON.stringify(account));
                                        //                                        return;
                                        settingFetchingIndicator.running = false;
                                        accountsDocument.addAccount(account);
                                        settingsPage.state="NORMAL"


                                    },
                                    //TODO show error and provide manual configuration options
                                    function(){console.log("fetch failed")}
                                    );
                    }
                }
                Button{
                    text: "Cancel"
                    onClicked: settingsPage.state="NORMAL"
                }
                ActivityIndicator{
                    id: settingFetchingIndicator
                    running: false
                    visible: running
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
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
                id: toolbarItems
                back:ToolbarButton{
                    action: Action{
                        text: "Back"
                        iconSource: Qt.resolvedUrl("../img/back.svg")
                        onTriggered: mainView.goToMailboxPage();
                    }
                }
            }
        }
        ToolbarShadow{
        }
    }
}
