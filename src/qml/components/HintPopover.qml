import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1

Component {
    Popover {
        id: popover
        contentWidth: text.width
        Text{
            id: text
            text: mainView.hintText
        }
    }
}
