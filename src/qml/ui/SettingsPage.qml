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
    property int editingIndex

    signal addingAccount(var s);
    signal removingAccount(int idx);
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
            TROJITA_MESSAGES.displayCount = 20;
            settingsPage.setCurrentAccount(idx)
        }
        function configAccountIndex(idx){
            editingIndex = idx
            settingsPage.state="CONFIG_MAILBOX"
            if (idx !== -1) {
                usernameTextField.text = accountsDocument.contents.accounts[idx]["imap.auth.user"]
                configPassWordTextField.text = accountsDocument.contents.accounts[idx]["imap.auth.pass"]

                imapServerTextField.text = accountsDocument.contents.accounts[idx]["imap.host"]
                imapPortTextField.text = accountsDocument.contents.accounts[idx]["imap.port"]

                if (accountsDocument.contents.accounts[idx]["imap.method"] === "SSL"){
                    encryptionLI.selectedIndex = 0;
                } else if (accountsDocument.contents.accounts[idx]["imap.starttls"]){
                    encryptionLI.selectedIndex = 1;
                } else {
                    encryptionLI.selectedIndex = 2;
                }

                smtpServerTextField.text = accountsDocument.contents.accounts[idx]["msa.smtp.host"]
                smtpPortTextField.text = accountsDocument.contents.accounts[idx]["msa.smtp.port"]

                smtpMethodLI.selectedIndex = (accountsDocument.contents.accounts[idx]["msa.method"] === "SMTP" ? 0 : 1);
                performSTARTTLSCheckBox.checked = accountsDocument.contents.accounts[idx]["msa.smtp.starttls"];
            }
            else {
                usernameTextField.text = emailTextField.text
                configPassWordTextField.text = passwordTextField.text
            }


        }

        function removeAccount(idx){
            var tmp = contents
            if(contents.currentAccountIndex>=idx){
                tmp.currentAccountIndex--
            }
            tmp.accounts.splice(idx, 1);
            contents=tmp;
            settingsPage.removingAccount(idx);
        }
        function addAccount(account){
            var tmp = contents
            tmp.accounts.push(account);
            if(contents.accounts.length ===0){
                tmp.currentAccountIndex = 0
            }
            contents=tmp;
            settingsPage.addingAccount(account);
            if(contents.accounts.length === 1){
                setCurrentIndex(0);
            }
        }

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
            if(accountsDocument.contents.currentAccountIndex >=0)
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
            PropertyChanges{
                target: configColumn
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
            PropertyChanges{
                target: configColumn
                visible: false;
            }
        },
        State{
            name: "CONFIG_MAILBOX"
            PropertyChanges {
                target: accountColumn
                visible: false
            }
            PropertyChanges{
                target: configColumn
                visible: true;
            }

        }

    ]
    Flickable{
        height:  parent.height - (toolbar.locked? toolbar.height : 0)
        width: parent.width
        contentWidth: parent.width
        contentHeight: accountColumn.visible ? accountColumn.height : configColumn.height
        Column{
            id: accountColumn
            width: parent.width
            ListItem.Header{
                text: "Current account"
            }
            ListItem.Standard{
                id: currentAccount
                visible: accountsDocument.contents.currentAccountIndex != -1
                text: visible ? accountsDocument.contents.accounts[accountsDocument.contents.currentAccountIndex]["imap.auth.user"] : ""
                control:Row{
                    height: currentAccount.height
                    spacing: units.gu(2)
                    Image{
                        height: parent.height/2
                        width: height
                        anchors.verticalCenter: parent.verticalCenter
                        source: Qt.resolvedUrl("../img/settings.svg")
                        MouseArea{
                            anchors.fill: parent
                            onClicked: accountsDocument.configAccountIndex(accountsDocument.contents.currentAccountIndex)
                        }
                    }
                    Image{
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
                            source: Qt.resolvedUrl("../img/settings.svg")
                            MouseArea{
                                anchors.fill: parent
                                onClicked: accountsDocument.configAccountIndex(index)
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
                        id: addAccountButton
                        text: "Add"
                        onClicked:{
                            settingFetchingIndicator.running = true;
                            MailConfig.fetchConfiguration(
                                        emailTextField.text,
                                        passwordTextField.text,
                                        function(account){
                                            settingFetchingIndicator.running = false;
                                            accountsDocument.addAccount(account);
                                            settingsPage.state="NORMAL"
                                        },
                                        function(){
                                            console.log("fetch failed")
                                            settingFetchingIndicator.running = false
                                            addAccountConfig.visible = true
                                            addAccountButton.visible = false

                                        }
                                        );
                        }
                    }
                    Button{
                        text: "Cancel"
                        onClicked: {
                            settingFetchingIndicator.running = false
                            addAccountConfig.visible = false
                            addAccountButton.visible = true
                            emailTextField.text = ""
                            passwordTextField.text = ""

                            settingsPage.state="NORMAL"
                        }
                    }
                    ActivityIndicator{
                        id: settingFetchingIndicator
                        running: false
                        visible: running
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Button{
                        id: addAccountConfig
                        text: "Manual Configuration"
                        onClicked: {

                            accountsDocument.configAccountIndex(-1)
                        }
                        visible: false
                    }
                }
            }

        }
        Column {
            id: configColumn
            width: parent.width
            ListItem.Header{
                text: "Config Account"
            }
            ListItem.Standard{
                //TODO check email address is valid
                id: usernameLI
                text: "Username"
                control: TextField{
                    id: usernameTextField
                }
            }
            ListItem.Standard{
                //TODO check email address is valid
                id: configPasswordLI
                text: "Password"
                control: TextField{
                    id: configPassWordTextField
                    text: passwordTextField.text
                    echoMode: TextInput.Password
                }
            }
            ListItem.Header{
                text: "IMAP"
            }
            ListItem.ValueSelector {
                id: encryptionLI
                text: "Encryption"
                values: ["Force encryption (TLS)", "Use encryption (STARTTLS)", "No encrytion"]
            }
            ListItem.Standard{
                //TODO check email address is valid
                id: imapServerLI
                text: "Server"
                control: TextField{
                    id: imapServerTextField
                }
            }
            ListItem.Standard{
                //TODO check email address is valid
                id: imapPortLI
                text: "Port"
                control: TextField{
                    id: imapPortTextField
                }
            }

            ListItem.Header{
                text: "SMTP"
            }
            ListItem.ValueSelector {
                id: smtpMethodLI
                text: "Encryption"
                values: ["SMTP", "Secure SMTP"]
                onSelectedIndexChanged: {
                    if (selectedIndex == 1) {
                        performSTARTTLSCheckBox.checked = true
                        performSTARTTLSCheckBox.enabled = false
                    }
                    else {
                        performSTARTTLSCheckBox.checked = false
                        performSTARTTLSCheckBox.enabled = true
                    }
                }
            }
            ListItem.Standard{
                //TODO check email address is valid
                id: smtpServerLI
                text: "Server"
                control: TextField{
                    id: smtpServerTextField
                }
            }
            ListItem.Standard{
                //TODO check email address is valid
                id: smtpPortLI
                text: "Port"
                control: TextField{
                    id: smtpPortTextField
                }
            }
            ListItem.Standard{
                //TODO check email address is valid
                id: performSTARTTLSLI
                text: "Perform STARTTLS"
                control: CheckBox{
                    id: performSTARTTLSCheckBox
                }
            }
            ListItem.SingleControl{
                id: configAddButtons
                control:Row{
                    height: currentAccount.height
                    spacing: units.gu(2)
                    Button{
                        text: "Save"
                        onClicked:{
                            if (editingIndex !== -1)
                                accountsDocument.removeAccount(editingIndex)
                            var account = {
                                "imap.auth.user" : usernameTextField.text,
                                "imap.auth.pass" : configPassWordTextField.text,
                                "imap.host": imapServerTextField.text,
                                "imap.port" : imapPortTextField.text,
                                "imap.method": encryptionLI.selectedIndex == 0? "SSL" : "TCP",
                                                                                "imap.starttls": encryptionLI.selectedIndex != 2,
                                                                                "msa.method": smtpMethodLI.selectedIndex == 0 ? "SMTP" : "SSMTP",   // use SMTP by default
                                                                                                                                "msa.smtp.host": smtpServerTextField.text,
                                                                                                                                "msa.smtp.port": smtpPortTextField.text,
                                                                                                                                "msa.smtp.starttls": performSTARTTLSCheckBox.checked,
                                                                                                                                "msa.smtp.auth": true,    //default to be true
                                                                                                                                "msa.smtp.auth.user": usernameTextField.text,
                                                                                                                                "msa.smtp.auth.pass": configPassWordTextField.text
                            }
//                            console.log(JSON.stringify(account))
                            accountsDocument.addAccount(account);
                            settingsPage.state="NORMAL"
                        }
                    }
                    Button{
                        text: "Cancel"
                        onClicked: {

                            settingsPage.state="NORMAL"
                            emailTextField.text = ""
                            passwordTextField.text = ""
                            settingFetchingIndicator.running = false
                            addAccountConfig.visible = false
                            addAccountButton.visible = true
                        }

                    }
                }
            }
        }
    }
    //dummy mouseAreato disable click when toolbar is shown and not locked
    MouseArea{
        anchors.fill: parent
        onClicked: {}
        enabled: toolbar.opened && !toolbar.locked
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
