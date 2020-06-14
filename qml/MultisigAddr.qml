import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import "./controls"
import SCM 1.0

Item {
    id : root
    anchors.fill: parent

    AddMultisig {
        id : addMultisigDialog
    }

    MultisigTransfer {
        id: multisigTransferDialog
    }


    Component.onCompleted: {
        http.httpReplied.connect(onReplied)
    }

    function onReplied(data, status) {
        var jsonObject = JSON.parse(data)
        if(jsonObject["id"] === 8001) {
            var resultObject = jsonObject["result"]
            var address = resultObject["address"]
            var balance = resultObject["balance"]

            for(var i = 0; i < listModel.count; i++) {
                if(listModel.get(i)["address"] === address) {
                    listModel.setProperty(i,"balance",balance)
                }
            }
        }
    }

    function update() {
        var msMap = $guiData.readTest()
        listModel.clear()
        for(var key in msMap) {
            listModel.append({address: key, required : msMap[key].required, pubkeys : msMap[key].pubKeys, balance : "0"})
            http.queryAssetBalance(key, $guiData.currentAsset)
        }
    }

    HttpManager {
        id : http
    }

    Text {
        id: title
        text: qsTr("多重签名")
        color: "white"
        font.pixelSize: 16
        font.weight: Font.Bold

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 30
    }

    ColumnLayout {
//        width: parent.width
        anchors.fill: parent

        Component.onCompleted: {

        }

        CustomButton {
            id: transferBtn
            Layout.alignment: Qt.AlignTop | Qt.AlignRight
            Layout.topMargin: 60
            Layout.rightMargin: 30
            text: qsTr("添加新的多签地址")
            onClicked: addMultisigDialog.open()
        }

        ListModel {
            id: listModel

        }


        TableView {
            id : tableView

            Layout.fillWidth: true
            Layout.fillHeight: true

            MultisigInfo {
                id : info
            }


            model: listModel
            Component.onCompleted: {
                root.update()
            }

            TableViewColumn {
                role : "address"
                width: 320
                title : "已添加的" + $guiData.currentAsset + "多签地址"

                delegate: ItemDelegate {
                    Label {
                        anchors.centerIn: parent
                        text: styleData.value
                    }
                }

            }

            TableViewColumn {
                role : "balance"
                width: 140
                title : "余额"

                delegate: ItemDelegate {
                    Label {
                        anchors.centerIn: parent
                        text: styleData.value
                    }
                }
            }

            TableViewColumn {
                title : ""
                width: 100

                delegate: ItemDelegate {
                    Button {
                        text: "transfer"
                        anchors.centerIn: parent

                        font.pixelSize: 12
                        flat: true

                        onClicked: {
                            multisigTransferDialog.fromAddress = listModel.get(styleData.row)["address"]
                            multisigTransferDialog.open()
                        }
                    }
                }
            }

            TableViewColumn {
                title : ""
                width: 100

                delegate: ItemDelegate {
                    Button {
                        text: "delete"
                        anchors.centerIn: parent

                        font.pixelSize: 12
                        flat: true

                        onClicked: {
                            commonDialog.contentText = "确定删除这个多签地址吗?"
                            commonDialog.needCancel = true
                            commonDialog.intData = styleData.row
                            commonDialog.open()
                        }
                    }
                }
            }

            rowDelegate: Item {
                height: 40
            }

            headerDelegate: Rectangle {
                id: rect
                height: 35

                color:"transparent"

                Label {
                    anchors.centerIn: parent
                    text: styleData.value
                }

            }
        }
    }

    CommonDialog {
       id : commonDialog
    }

    Connections {
        target : commonDialog
        onClosed : {
            if(commonDialog.yesOrNo) {
                bitcoinUtil.deleteBtcMultisigAddress(listModel.get(commonDialog.intData)["address"])
                root.update()
            }

        }
    }

}
