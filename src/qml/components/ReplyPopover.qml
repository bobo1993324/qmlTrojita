import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1

Component {
    ActionSelectionPopover {
        id: popover
        actions: ActionList {
            Action {
                text: "Reply privately"
                onTriggered: {
                    //TODO delete current message
                    messagePage.replyPrivately()
                    PopupUtils.close(popover)
                }
            }
            Action{
                text: "Reply to all"
                onTriggered: {
                    composePage.reset()
                    composePage.to = TROJITA_MESSAGE_DETAILS.from.address
                    //construct cc
                    var s = "";
                    for (var i in TROJITA_MESSAGE_DETAILS.to){
                        //remove me from list of address
                        if (TROJITA_MESSAGE_DETAILS.to[i].address.indexOf(settingsPage.activeAccount) < 0){
                            s += TROJITA_MESSAGE_DETAILS.to[i].address + ", ";
                        }
                    }
                    for (var i in TROJITA_MESSAGE_DETAILS.cc){
                        //remove me from list of address
                        if (TROJITA_MESSAGE_DETAILS.cc[i].address.indexOf(settingsPage.activeAccount) < 0){
                            s += TROJITA_MESSAGE_DETAILS.cc[i].address + ", ";
                        }
                    }
                    //remove ending ,
                    composePage.cc = (s == "" ? "" : s.substring(0, s.length - 2));

                    //construct bcc
                    s = "";
                    for (var i in TROJITA_MESSAGE_DETAILS.bcc){
                        //remove me from list of address
                        if (TROJITA_MESSAGE_DETAILS.bcc[i].address.indexOf(settingsPage.activeAccount) < 0){
                            s += TROJITA_MESSAGE_DETAILS.bcc[i].address + ", ";
                        }
                    }
                    //remove ending ,
                    composePage.bcc = (s == "" ? "" : s.substring(0, s.length - 2));

                    composePage.subject = "Re: "+TROJITA_MESSAGE_DETAILS.subject
                    composePage.content = TROJITA_MESSAGE_DETAILS.generateReplyMessage();
                    TROJITA_SEND_MAIL.prepare();
                    mainView.goToComposePage()
                    composePage.showCcBcc = true;
                    PopupUtils.close(popover)
                }
            }
        }

        delegate: ListItem.Standard {
            text: action.text
        }
    }
}
