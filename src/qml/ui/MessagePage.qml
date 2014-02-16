import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Extras.Browser 0.1
Page{
    id: messagePage
    visible: false
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
                }
            }
        }
        ListItem.Standard{
            id: fromLIS
            text: "From"
            control:
                Label{
                text:TROJITA_MESSAGE_DETAILS.from
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        ListItem.Standard{
            text: TROJITA_MESSAGE_DETAILS.date
        }

        Rectangle{
            width: parent.width
            height: parent.height-fromLIS.height
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
}

