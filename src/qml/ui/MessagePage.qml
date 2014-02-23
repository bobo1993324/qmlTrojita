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
    Column{
        anchors.fill: parent
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
            visible: messagePage.detailExpanded
            text: TROJITA_MESSAGE_DETAILS.date
        }

        Rectangle{
            width: parent.width
            height: parent.height-fromLIS.height*(messagePage.detailExpanded? 3:1);
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
    }
//    Button{
//        text: "go back"
//        onClicked: mainView.goToMailboxPage()
//    }

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
                    visible: false
                    action: Action{
                        text: "Reply"
                        iconSource: Qt.resolvedUrl("../img/compose.svg")
                    }
                }
            }
        }
    }
}

