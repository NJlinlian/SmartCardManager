import QtQuick 2.0
import QtQuick 2.11
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
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

    property string fromAddress: ""

    background: Rectangle {
        radius: 0
        color: Style.background_dialog
        anchors.fill: parent
    }

    Component.onCompleted: {
        http.httpReplied.connect(onReplied)
    }

    function onReplied(data, status) {
        var jsonObject = JSON.parse(data)
        if(jsonObject["id"] === 8002) {
            print("jjjjjjjjjjj ", JSON.stringify(jsonObject) )
            var resultObject = jsonObject["result"]
            var dataObject = resultObject["data"]
            if(dataObject === "") {
                commonDialog.contentText = "创建交易失败"
                commonDialog.open()
            }
            else {
                var rawTrx = dataObject["hex"];
                commonDialog.contentText = rawTrx
                commonDialog.open()
            }

        }
    }


    contentItem: Column {
        width: parent.width

        CustomButton {
            id: addAddressBtn
            anchors.left: parent.left
            text: qsTr("添加目标地址")
            onClicked: {
                listModel.append({ "address" : "", "amount": ""})
            }
        }

        spacing: 30

        TableView {
            id: tableView
            width: parent.width


            ListModel {
                id: listModel

                ListElement {
                    address: ""
                    amount: ""
                }
            }

            model : listModel

            TableViewColumn {
                role: "address"
                title: qsTr("目标地址")
                width: 280
                delegate: ItemDelegate {
                    TextInput {
                        width: parent.width
//                        text: ""
//                        acceptableInput: true
                        onEditingFinished : {
                            listModel.set(styleData.row, { address : text, amount : listModel.get(styleData.row)["amount"]})
                        }
                    }
                }
            }

            TableViewColumn {
                role: "amount"
                title: qsTr("转出金额")
                width: (tableView.viewport.width - 280) / 2
                delegate: ItemDelegate {
                    TextEdit {
                        width: parent.width
//                        text: ""
                        onEditingFinished : {
                            listModel.set(styleData.row, { address : listModel.get(styleData.row)["address"], amount : text})
                        }
                    }
                }
            }


        }

        CommonDialog {
            id : commonDialog
        }

        CustomButton {
            id: createTrx
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("创建交易")
            onClicked: {
                var jsonobj = Object.create(null)
                for(var i = 0; i < listModel.count; i++) {
//                    console.log( "1111", listModel.get(i)["address"], listModel.get(i)["amount"])
                    if(listModel.get(i)["address"] === "" || listModel.get(i)["amount"] === null) {
                        commonDialog.contentText = "错误的输入"
                        commonDialog.open()
                        return
                    }
                    else {
                        jsonobj[listModel.get(i)["address"]] = parseFloat(listModel.get(i)["amount"])
                    }
                }
                print("cccccccc", fromAddress, JSON.stringify(jsonobj))
                http.createTrx(fromAddress, JSON.stringify(jsonobj), $guiData.currentAsset)
            }
        }
    }


    onOpened: {

    }
}
