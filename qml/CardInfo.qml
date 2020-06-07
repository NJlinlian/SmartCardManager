import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import "./controls"
import SCM 1.0

Item {
    id: root
    anchors.fill: parent

    TransferBtc {
        id: transferBtcDialog
    }

    TransferEth {
        id: transferEthDialog
    }

    SignMultisigTrx {
        id: signMultisigTrxDialog
    }


    Component.onCompleted: {
        $guiData.currentAssetChanged.connect(updateCardInfo)
        http.httpReplied.connect(onReplied)
        updateCardInfo()
    }

    function updateCardInfo() {
        $cardView.updatePubKey()
        pubkeyText.text = $cardView.getCompressedPubKey()
        addressText.text = $guiData.getCurrentAddress()

        if(addressText.text != "") {
            http.queryAssetBalance(addressText.text, $guiData.currentAsset)
        }
    }

    function onReplied(data, status) {
//        print(data,status)
        var jsonObject = JSON.parse(data)
        if(jsonObject["id"] === 8001) {
            var resultObject = jsonObject["result"]
            var address = resultObject["address"]
            var balance = resultObject["balance"]

            if(addressText.text === address) {
                balanceText.text = balance
            }
        }
    }

    HttpManager {
        id : http
    }



    Text {
        id: title
        text: qsTr("卡片信息")
        color: "white"
        font.pixelSize: 16
        font.weight: Font.Bold

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 30
    }

    Rectangle {
        id: container
        color: "white"
        radius: 5

        anchors.fill: parent
        anchors.topMargin: 90
        anchors.bottomMargin: 20
        anchors.leftMargin: 20
        anchors.rightMargin: 20

        ColumnLayout {
            spacing: 20
            width: parent.width

            RowLayout {
                Layout.topMargin: 50
                Layout.preferredHeight: 30
                Layout.leftMargin: 50
                Layout.rightMargin: 20

                Text {
                    id: text1
                    text: qsTr("公钥")
                    font.pixelSize: 14
                }


                TextArea {
                    id: pubkeyText
                    text: ""
                    font.pixelSize: 14
                    Layout.leftMargin: 30
                }
            }

            RowLayout {
                Layout.topMargin: 20
                Layout.preferredHeight: 30
                Layout.leftMargin: 50
                Layout.rightMargin: 20

                Text {
                    id: text2
                    text: qsTr("地址")
                    font.pixelSize: 14
                }

                TextArea {
                    id: addressText
                    text: ""
                    font.pixelSize: 14
                    Layout.leftMargin: 30
                }
            }

            RowLayout {
                Layout.topMargin: 20
                Layout.preferredHeight: 30
                Layout.leftMargin: 50
                Layout.rightMargin: 20

                Text {
                    id: text3
                    text: qsTr("余额")
                    font.pixelSize: 14
                }

                TextArea {
                    id: balanceText
                    text: "0.00"
                    font.pixelSize: 14
                    Layout.leftMargin: 30
                }
            }


            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 60

                CustomButton {
                    id: transferBtn
                    text: qsTr("转账")
                    onClicked: {
                        if($guiData.currentAsset === "ETH" || $guiData.currentAsset === "USDT"){
                            transferEthDialog.open()
                            transferEthDialog.resetInput()
                        }else if($guiData.currentAsset === "BTC" || $guiData.currentAsset === "LTC" || $guiData.currentAsset === "BCH") {
                            transferBtcDialog.open()
                        }
                    }
                }

                CustomButton {
                    id: multisigSignBtn
                    text: qsTr("多签交易签名")
                    Layout.leftMargin: 30
                    onClicked: {
                        signMultisigTrxDialog.open()
                    }
                    visible: $guiData.isMultisigEnable($guiData.currentAsset)
                }
            }
        }

    }
}
