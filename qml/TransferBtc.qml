import QtQuick 2.0
import QtQuick 2.11
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4 as Controls24
import QtQuick.Controls 1.4 as Controls14
import QtQuick.Controls.Styles 1.4 as Styles14
import "./controls"
import SCM 1.0

Controls24.Dialog {
    id: root
    modal: true

    width: 520
    height: 420
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    visible: false

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
            print("transferbtc.qml",JSON.stringify(jsonObject))
            var resultObject = jsonObject["result"]
            var dataObject = resultObject["data"]
            if(dataObject === "") {
                commonDialog.contentText = "创建交易失败"
                commonDialog.open()
            }
            else {
                var rawTrx = dataObject["hex"];
                if($guiData.getCurrentAddress().slice(0,1) === "1" || $guiData.getCurrentAddress().slice(0,1) === "L") {
                    var signedTrx = bitcoinUtil.signRawTrx($guiData.getCurrentAddress(), rawTrx)

                    broadcastDialog.trxStr = signedTrx
                    broadcastDialog.open()

                }
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

        ListModel {
            id: listModel

            ListElement {
                address: ""
                amount: ""
            }
        }

        Controls14.TableView {
            id: tableView
            width: parent.width
            height : 240

            rowDelegate: Item {
                height: 40
                Rectangle {
                    anchors.fill: parent
                    color: (styleData.alternate && styleData.row >= listModel.count)? "#eeeeee" : "white"
                }
            }

            headerDelegate: Rectangle {
                id: rect
                height: 35

                border.color: "grey"
                border.width: 1
//                color:"transparent"
                Text {
                    anchors.centerIn: parent
                    text: styleData.value
                }
            }



            model : listModel


            Controls14.TableViewColumn {
                role: "address"
                title: qsTr("目标地址")
                width: 300


                delegate: Rectangle {
                    anchors.fill: parent
                    Controls14.TextField {
                        id : textField
                        style: Styles14.TextFieldStyle {
                            textColor: "black"
                            background: Rectangle {
                                radius: 0
                                implicitWidth: 100
                                implicitHeight: 24
                                border.color: "#0088ee"
                                border.width: textField.activeFocus ? 2 : 0
                                color: (textField._row % 2) ? "#eeeeee" : "white"
                            }
                        }
                        font.pixelSize: 14

                        property int _row: styleData.row   // TextFieldStyle中不知如何获取styleData 暂时只能定义一个变量转一下

                        placeholderText : textField._row
                        anchors.fill: parent
                        onEditingFinished : {
                            listModel.set(styleData.row, { address : text, amount : listModel.get(styleData.row)["amount"]})
                        }
                    }
                }
            }

           Controls14.TableViewColumn {
                role: "amount"
                title: qsTr("转出金额")
                width: (tableView.viewport.width - 300) / 2
                delegate: Rectangle {
                    Controls14.TextField {
                        id : textField2
                        style: Styles14.TextFieldStyle {
                            textColor: "black"
                            background: Rectangle {
                                radius: 0
                                implicitWidth: 100
                                implicitHeight: 24
                                border.color: "#0088ee"
                                border.width: textField2.activeFocus ? 2 : 0
                                color: (textField2._row % 2) ? "#eeeeee" : "white"
                            }
                        }
                        font.pixelSize: 14
                        property int _row: styleData.row
                        anchors.fill: parent
                        onEditingFinished : {
                            listModel.set(styleData.row, { address : listModel.get(styleData.row)["address"], amount : text})
                        }
                    }
                }
            }


            Controls14.TableViewColumn {
                title : ""
                width: (tableView.viewport.width - 300) / 2

                delegate: Rectangle {
                    Controls24.Button {
                        id: button
                        text: "delete"
                        anchors.fill: parent
                        font.pixelSize: 12
                        background: Rectangle {
                            border.width: 0
                            color: (button._row % 2) ? "#eeeeee" : "white"
                        }
                        property int _row: styleData.row

                        onClicked: {
                            listModel.remove(styleData.row)
                        }
                    }
                }
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
                http.createTrx($guiData.getCurrentAddress(), JSON.stringify(jsonobj), $guiData.currentAsset)
            }
        }
    }


    onOpened: {

    }
}
