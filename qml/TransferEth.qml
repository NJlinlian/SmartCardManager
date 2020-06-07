import QtQuick 2.0
import QtQuick 2.11
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtQuick.Controls 1.4 as Controls14
import QtQuick.Controls.Styles 1.4 as Styles14
import "./controls"
import SCM 1.0

Dialog {
    id: root
    modal: true

    width: 520
    height: 420
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    visible: false

    property bool isCallContract: false

    background: Rectangle {
        radius: 0
        color: Style.background_dialog
        anchors.fill: parent
    }

    Component.onCompleted: {
        http.httpReplied.connect(onReplied)
        http.getEthAddressNonce($guiData.getCurrentAddress())
    }

    function onReplied(data, status) {
        var jsonObject = JSON.parse(data)
        if(jsonObject["id"] === 8101) {
            print(JSON.stringify(jsonObject))
            var resultStr = jsonObject["result"]
            nonceLabel.text = parseInt(resultStr)
        }
    }

    function resetInput() {
        http.getEthAddressNonce($guiData.getCurrentAddress())
    }


    contentItem: ColumnLayout {
        width: parent.width
        spacing: 20

        RowLayout {
            width: parent.width
            Layout.preferredHeight: 30
            Layout.leftMargin: 50
            Layout.rightMargin: 50

            Label {
                text: "接收地址"
                Layout.preferredWidth: 80
            }

            TextField {
                id: receiveAddressText
                Layout.preferredWidth: 300
                Layout.preferredHeight: 30

                text: ""
                selectByMouse: true
            }
        }

        RowLayout {
            width: parent.width
            Layout.preferredHeight: 30
            Layout.leftMargin: 50
            Layout.rightMargin: 50

            Label {
                text: "金额"
                Layout.preferredWidth: 80
            }

            TextField {
                id: amountText
                Layout.preferredWidth: 300
                Layout.preferredHeight: 30

                text: ""
                selectByMouse: true
            }
        }

        RowLayout {
            width: parent.width
            Layout.preferredHeight: 30
            Layout.leftMargin: 50
            Layout.rightMargin: 50

            Label {
                Layout.preferredWidth: 80
                text: "gas limit"

            }

            TextField {
                id: gasLimitText
                Layout.preferredWidth: 300
                Layout.preferredHeight: 30

                text: "21000"
                selectByMouse: true
            }
        }

        RowLayout {
            width: parent.width
            Layout.preferredHeight: 30
            Layout.leftMargin: 50
            Layout.rightMargin: 50


            Label {
                Layout.preferredWidth: 80
                text: "gas price"

            }

            SpinBox {
                id: gasPriceSpinBox
                from: 1
                to: 50
                value: 50
                editable: true
            }

            Label {
                Layout.leftMargin: 10
                text: "Gwei"
            }
        }

        RowLayout {
            width: parent.width
            Layout.preferredHeight: 30
            Layout.leftMargin: 50
            Layout.rightMargin: 50


            Label {
                Layout.preferredWidth: 80
                text: "data"
            }


            TextField {
                id: dataText
                Layout.preferredWidth: 300
                Layout.preferredHeight: 30

                text: ""
                selectByMouse: true
            }
        }


        RowLayout {
            width: parent.width
            Layout.preferredHeight: 30
            Layout.leftMargin: 50
            Layout.rightMargin: 50

            Label {
                Layout.preferredWidth: 80
                text: "nonce"
            }


            Label {
                id: nonceLabel
                text: "获取中..."

            }
        }


        CommonDialog {
            id : commonDialog
        }

        Broadcast {
            id : broadcastDialog
        }


        CustomButton {
            id: createTrx
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("创建交易")
            onClicked: {
                if(receiveAddressText.text !== 42 && receiveAddressText.text.substring(0,2) != "0x") {
                    commonDialog.contentText = "错误的接收地址"
                    commonDialog.open()
                    return
                }

                if(Number(amountText.text) <= 0) {
                    commonDialog.contefntText = "错误的金额"
                    commonDialog.open()
                    return
                }

                var realAmount = $guiData.decimalToIntegerStr(amountText.text,18)
                var realGasPrice = $guiData.decimalToIntegerStr(gasPriceSpinBox.value.toString(),9)
                var trxStr = ethereumUtil.transfer(receiveAddressText.text, realAmount, gasLimitText.text, realGasPrice,
                                      dataText.text, nonceLabel.text)
                if(trxStr !== "") {
                    trxStr = "0x" + trxStr
                    broadcastDialog.trxStr = trxStr
                    broadcastDialog.open()
                }
            }
        }
    }

}
