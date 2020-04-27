import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

Dialog {
    id : control
    modal: true
    width: 460
    height: 280

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    visible: false

    property string contentText: ""
    property bool yesOrNo: false
    property bool needCancel: false
    property int intData: 0
    property string stringData: ""


    contentItem: ColumnLayout {
        anchors.fill: parent
        anchors.margins: 30

        Item {
            TextArea {
                height: 180
                width: 380
                Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                text: contentText
                wrapMode: Text.Wrap
                selectByMouse: true
                readOnly: true

            }
        }

        spacing: 30

        Item {
            width: parent.width
            Layout.alignment: Qt.AlignBottom
            Layout.bottomMargin: 30

            RowLayout {
                width: parent.width
                CustomButton {
                    id: okBtn
                    //                Layout.alignment: needCancel ? Qt.AlignLeft : Qt.AlignHCenter
                    Layout.alignment: needCancel ? Qt.AlignRight : Qt.AlignHCenter
                    text: qsTr("确定")
                    onClicked: {
                        yesOrNo = true
                        control.close()
                    }
                }

                spacing: 60

                CustomButton {
                    id: cancelBtn
                    Layout.alignment: Qt.AlignLeft
                    visible: needCancel
                    text: qsTr("取消")
                    onClicked: {
                        yesOrNo = false
                        control.close()
                    }
                }
            }
        }


    }
}
