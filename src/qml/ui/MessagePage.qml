import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Extras.Browser 0.1
import Ubuntu.Components.Popups 0.1
import "../components"
Rectangle{
    id: messagePage
    property bool detailExpanded:false
    color: "#ECEDED"
    property alias toolbar: toolbar
    DeletePopover{
        id: deletePopover
    }
    MarkPopover{
        id: markPopover
    }
    ReplyPopover{
        id: replyPopover
    }
    //dummy mouseAreato disable click when toolbar is shown and not locked
    MouseArea{
        anchors.fill: parent
        onClicked: {}
        enabled: toolbar.opened && !toolbar.locked
    }

    Column{
        id: infoColumn
        width: parent.width
        ListItem.Standard{
            id: subjectLIS
            text: "Subject"
            control: Row{
                height: fromLIS.height
                spacing: units.gu(2)
                Label{
                    text:TROJITA_MESSAGE_DETAILS.subject
                    anchors.verticalCenter: parent.verticalCenter
                }
                Image{
                    source: Qt.resolvedUrl("../img/dropdown-menu.svg")
                    height: fromLIS.height*0.3
                    width: height
                    anchors.verticalCenter: parent.verticalCenter
                    rotation: messagePage.detailExpanded? 180: 0
                    MouseArea{
                        anchors.fill: parent
                        onClicked: messagePage.detailExpanded= !messagePage.detailExpanded
                    }
                }
            }
        }
        ListItem.Standard{
            id: fromLIS
            text: "From"
            visible: messagePage.detailExpanded;
            control:
                Label{
                text:TROJITA_MESSAGE_DETAILS.from
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        ListItem.Standard{
            id: toLIS
            text: "To"
            visible: messagePage.detailExpanded && TROJITA_MESSAGE_DETAILS.toCount !== 0;
            control:Flickable{
                clip: true
                width: toRow.width < toLIS.width * 0.8 ? toRow.width : toLIS.width * 0.8
                height: toLIS.height
                contentWidth: toRow.width
                contentHeight: parent.height
                Row{
                    id: toRow
                    height: toLIS.height
                    spacing: units.gu(1)


                    Repeater{
                        model: TROJITA_MESSAGE_DETAILS.to
                        delegate: Label{
                            text: modelData + (index < TROJITA_MESSAGE_DETAILS.toCount - 1 ? "," : "")
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }
            }
        }
        ListItem.Standard{
            id: ccLIS
            text: "CC"
            visible: messagePage.detailExpanded && TROJITA_MESSAGE_DETAILS.ccCount !== 0;
            control:Flickable{
                clip: true
                width: ccRow.width < ccLIS.width * 0.8 ? ccRow.width : ccLIS.width * 0.8
                height: ccLIS.height
                contentWidth: ccRow.width
                contentHeight: parent.height
                Row{
                    id: ccRow
                    height: ccLIS.height
                    spacing: units.gu(1)


                    Repeater{
                        model: TROJITA_MESSAGE_DETAILS.cc
                        delegate: Label{
                            text: modelData + ","
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }
            }
        }

        ListItem.Standard{
            id: bccLIS
            text: "BCC"
            visible: messagePage.detailExpanded && TROJITA_MESSAGE_DETAILS.bccCount !== 0;
            control:Flickable{
                clip: true
                width: bccRow.width < bccLIS.width * 0.8 ? bccRow.width : bccLIS.width * 0.8
                height: bccLIS.height
                contentWidth: bccRow.width
                contentHeight: parent.height
                Row{
                    id: bccRow
                    height: bccLIS.height
                    spacing: units.gu(1)


                    Repeater{
                        model: TROJITA_MESSAGE_DETAILS.bcc
                        delegate: Label{
                            text: modelData + ","
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }
            }
        }

        ListItem.Standard{
            visible: messagePage.detailExpanded
            text: TROJITA_MESSAGE_DETAILS.date
        }
    }

    Rectangle{
        anchors{
            top: infoColumn.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            bottomMargin: isPhone ? 0 : toolbar.height
        }
        UbuntuWebView{
            width: parent.width
            height: parent.height
            id: messageWebview
            Connections{
                target: TROJITA_MESSAGE_DETAILS
                onContentChanged: {
                    messageWebview.loadHtml(TROJITA_MESSAGE_DETAILS.content)
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
            width: parent.width
            height: parent.height
            color: "white"
            ToolbarItems{
                back: ToolbarButton{
                    visible: isPhone
                    action: Action{
                        text: "Back"
                        iconSource: Qt.resolvedUrl("../img/back.svg")
                        onTriggered: {
                            mainView.goToMailboxPage()
                        }
                    }
                }

                ToolbarButton{
                    id: deleteButton
                    action: Action{
                        text: "Delete"
                        iconSource:Qt.resolvedUrl("../img/delete.svg")
                        onTriggered:{
                            PopupUtils.open(deletePopover, deleteButton)
                        }
                    }
                }
                ToolbarButton{
                    id: markButton
                    action: Action{
                        text: "Mark"
                        iconSource:Qt.resolvedUrl("../img/compose.svg")
                        onTriggered:{
                            PopupUtils.open(markPopover, markButton)
                        }
                    }
                }
                ToolbarButton{
                    //TODO implement reply
                    id: replyButton
                    action: Action{
                        text: "Reply"
                        iconSource: Qt.resolvedUrl("../img/back.svg")
                        onTriggered:{
                            if(TROJITA_MESSAGE_DETAILS.toCount == 1 && TROJITA_MESSAGE_DETAILS.ccCount == 0 && TROJITA_MESSAGE_DETAILS.bccCount == 0){
                                replyPrivately();
                            } else {
                                PopupUtils.open(replyPopover, replyButton);
                            }
                        }
                    }
                }
            }
        }

        ToolbarShadow{
        }
    }

    function replyPrivately(){
        composePage.reset()
        composePage.to = TROJITA_MESSAGE_DETAILS.from
        composePage.subject = "Re: "+TROJITA_MESSAGE_DETAILS.subject
        composePage.content = TROJITA_MESSAGE_DETAILS.generateReplyMessage();
        TROJITA_SEND_MAIL.prepare();
        mainView.goToComposePage()
    }
}

