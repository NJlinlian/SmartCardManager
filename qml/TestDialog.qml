import QtQuick 2.0
import QtQuick.Controls 2.4 as Controls24
import QtQuick.Controls 1.4

Controls24.Dialog {
    id: root


    width: 520
    height: 420
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    visible: false

    ListModel{
        id:libraryModel
        ListElement{title: "aaa"; author: "AAA"}
        ListElement{title: "bbb"; author: "BBB"}
        ListElement{title: "ccc"; author: "ccc"}
    }
    TableView{
        anchors.fill: parent




        model:libraryModel

        TableViewColumn{role:"title"; title: "Title"; width: 100}
        TableViewColumn{role:"author"; title: "Author"; width: 200}

        rowDelegate: Rectangle{
            height: 50
            color: styleData.selected?"#f0b0b0af":(styleData.alternate?"#c3c3c0":"#c0c0c3")
        }

        itemDelegate: Rectangle {
            height: 50
            color: "transparent"
            TextField {
                //anchors.centerIn: parent
                anchors.left: parent.left
                anchors.leftMargin: 6
                anchors.verticalCenter: parent.verticalCenter
//                color: styleData.textColor
                text: styleData.value
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 13
            }
        }
    }
}
