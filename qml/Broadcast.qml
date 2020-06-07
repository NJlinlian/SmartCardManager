import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import "./controls"

Dialog {
    id : control
    modal: true
    width: 460
    height: 340

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    visible: false

    property string trxStr: ""
    property string contentText: ""

    Component.onCompleted: {
        http.httpReplied.connect(onReplied)
    }

    function onReplied(data, status) {
        var jsonObject = JSON.parse(data)
        print(JSON.stringify(jsonObject))
        if(jsonObject["id"] === 8003) {
            var resultObject = jsonObject["result"]
            if($guiData.currentAsset === "ETH") {
                var dataStr = resultObject["data"]
                if(dataStr.slice(0,2) === "0x") {
                    commonDialog.contentText = "交易已广播：" + dataStr
                    commonDialog.open()
                }
                else {
                    commonDialog.contentText = dataStr
                    commonDialog.open()
                }
            }
            else {
                print(resultObject["error_code"])
                if(resultObject["error_code"] !== undefined) {
                    commonDialog.contentText = "广播失败"
                    commonDialog.open()
                }
                else {
                    commonDialog.contentText = "交易已广播"
                    commonDialog.open()
                }
            }
        }
    }

    CommonDialog {
        id : commonDialog
    }

    contentItem: ColumnLayout {
        anchors.fill: parent
//        anchors.margins: 30

        TextArea {
            height: 40
            width: parent.width - 80
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 20
            text: contentText
        }

        TextArea {
            height: 240
            width: parent.width - 80
            Layout.alignment: Qt.AlignHCenter
            text: trxStr
            wrapMode: Label.Wrap
            selectByMouse: true
        }


        CustomButton {
            id: okBtn
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 20
            text: qsTr("广播交易")
            onClicked: {
                http.broadcastTrx(trxStr, $guiData.currentAsset)
            }
        }

    }
}
