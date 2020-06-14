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

    width: 480
    height: 340
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    visible: false

    background: Rectangle {
        radius: 0
        color: Style.background_dialog
        anchors.fill: parent
    }

//    MultisigInfo {
//        id : info
//    }

    CommonDialog {
        id : commonDialog
    }

    Broadcast {
        id : broadcastDialog
    }

    contentItem: ColumnLayout {
        width: parent.width

       RowLayout {
           Layout.topMargin: 30
           Layout.preferredHeight: 220
           Layout.leftMargin: 20
           Layout.rightMargin: 50

           Text {
               id: text1
               text: qsTr("请输入待签名\nraw transaction")
               font.pixelSize: 14
           }

           TextArea {
               id: rawTrxText
               text: ""
               font.pixelSize: 14
               Layout.leftMargin: 30
               wrapMode: Text.Wrap

               onTextChanged: {
                   if(rawTrxText.text !== ""){
                       bitcoinUtil.parseMultisigInfoFromTrx(rawTrxText.text)
                       var info = bitcoinUtil.multisigInfo
                       if(info !== null){
                           var pk = $cardView.getCompressedPubKey().toString()
                           var result = false
                           for(var i = 0; i < info.pubKeys.length; i++) {
                               if(pk === info.pubKeys[i]) {
                                   result = true
                                   break
                               }
                           }

                           if(result) {
                               signBtn.enabled = true
                               return
                           }
                       }
                   }
                   signBtn.enabled = false
               }
           }

       }

       spacing: 30

        CustomButton {
            id: signBtn
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 30
            text: qsTr("签名")
            enabled: false

            onClicked: {
                var signedTrx = bitcoinUtil.signMultisigTrx($guiData.currentAsset, rawTrxText.text)
                if(signedTrx === "") {
                    commonDialog.contentText = "签名失败"
                    commonDialog.open()
                }
                else if(signedTrx.charAt(0) === "?"){
                    // 签名未满足
                    commonDialog.contentText = "签名成功\n" + signedTrx.substring(1)
                    commonDialog.open()
                }
                else {
                    broadcastDialog.trxStr = signedTrx
                    broadcastDialog.contentText = "签名数已满足，是否广播交易？"
                    broadcastDialog.open()
                }
            }
        }

    }


}
