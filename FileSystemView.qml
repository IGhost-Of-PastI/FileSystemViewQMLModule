import QtQuick
import QtQuick.Controls

Item {
    SplitView
    {
        anchors.fill: parent
        orientation: Qt.Horizontal
        Page
        {
            //width: 0
            contentWidth: 200
            SplitView.preferredWidth:250
            //width:100
            header:ToolBar
            {

            }
            TreeView
            {
                anchors.fill: parent
            }
        }
        Page
        {
            header:ToolBar
            {

            }
            ListView
            {
                anchors.fill: parent

            }
        }
    }
}


