import QtQuick 2.0
import QtQuick 2.11
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4 as Controls14
import QtQuick.Controls.Styles 1.4 as Styles14
import QtQuick.Controls 2.4
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

    background: Rectangle {
        radius: 0
        color: Style.background_dialog
        anchors.fill: parent
    }


    CommonDialog {
        id : commonDialog
    }


    Dialog {
        id : pubkeyDialog
        modal: true

        width: 420
        height: 300
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        visible: false

        background: Rectangle {
            radius: 0
            color: Style.background_dialog
            anchors.fill: parent
        }


        ColumnLayout {

            width: parent.width
            spacing: 30

            Label {
                text : "输入公钥"
                Layout.topMargin: 40
                //                Layout.alignment: Qt.AlignLeft
            }

            Controls14.TextField {
                id : pubkeyTextEdit
                width: parent.width

                Layout.topMargin: 15
                Layout.alignment: Qt.AlignHCenter
                //: settings tab, node section, peers label
                //% "Peers"
                text: ("123")
                font.pixelSize: 18
                font.styleName: "Bold";
                font.weight: Font.Bold

                style : Styles14.TextFieldStyle {
                    textColor: Style.content_main
                    background : Rectangle {
                        implicitWidth: 360
                        implicitHeight: 24
                        border.width: 1
                        border.color: "#333"
                    }
                }



            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                CustomButton {
                    text : "确定"
                    onClicked: {
                        if(pubkeyTextEdit.text.length > 0) {
                            listModel.append({pubkey : pubkeyTextEdit.text})
                            pubkeyDialog.close()
                        }

                    }
                }

                spacing: 100

                CustomButton {
                    text : "取消"
                    onClicked: {
                        pubkeyDialog.close()
                    }
                }
            }
        }
    }


    contentItem: Column {
        y : 30

        RowLayout {
            width: parent.width
            Layout.topMargin: 30

            Label {
                text : "需要签名数量"
            }

            Controls14.TextField {
                id : requiredNum
                text : ""

                style : Styles14.TextFieldStyle {
                    textColor: Style.content_main
                    background : Rectangle {
                        border.width: 1
                        border.color: "#333"
                    }
                }

            }

            Label {
                id : totalLabel
                text: "/" + listModel.count
            }

            CustomButton {
                id: addAddressBtn
                Layout.alignment: Qt.AlignRight
                text: qsTr("添加公钥")
                onClicked: {
                    pubkeyDialog.open()
                }
            }
        }



        spacing: 30

        Controls14.TableView {
            id: tableView
            width: parent.width
            height: 270


            ListModel {
                id: listModel

            }

            model : listModel

            Controls14.TableViewColumn {
                role: "address"
                title: qsTr("已添加公钥")
                width: 320

                delegate: ItemDelegate {
                    Label {
                        anchors.centerIn: parent
                        text: styleData.value
                    }
                }
            }

            Controls14.TableViewColumn {
                role: "amount"
                title: qsTr("")
                width: tableView.viewport.width - 320
                delegate: ItemDelegate {
                    Button {
                        text: "delete"
                        anchors.centerIn: parent

                        font.pixelSize: 12
                        flat: true

                        onClicked: {
                            listModel.remove(styleData.row)
                        }
                    }
                }
            }

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
                Label {
                    anchors.centerIn: parent
                    text: styleData.value
                }

            }

        }



        CustomButton {
            id: createTrx
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 30
            text: qsTr("创建")
            onClicked: {
                var array = []
                for(var i = 0; i < listModel.count; i++) {
                    array.push(listModel.get(i)["pubkey"])
                }

                var ms = bitcoinUtil.addBtcMultisigAddress(array , parseInt(requiredNum.text))
                print("new multisig address:", ms)

                commonDialog.contentText = "已添加多签地址 " + ms
                commonDialog.open()
            }
        }
    }




}
