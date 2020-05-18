import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0
import "./controls"
import SCM 1.0

Window {
    id: appWindow
    visible: true
    width: 640
    height: 480
    title: "数银卡桌面程序"

    flags: Qt.Window | Qt.WindowFullscreenButtonHint


    function cellResize() {
        if(appWindow.visibility != ApplicationWindow.Maximized) {
            var minWidth = Math.min(800, Screen.desktopAvailableWidth - 10);
            var minHeight = Math.min(600, Screen.desktopAvailableHeight - 80);
            appWindow.minimumWidth = minWidth;
            appWindow.minimumHeight = minHeight;
            appWindow.width = minWidth;
            appWindow.height = minHeight;
        }
    }
    onVisibilityChanged: cellResize()

    Component.onCompleted: {
        console.log(Style.control_text)
        console.log($guiData.objectName)
        console.log($guiData.btcUrl)
        updatePage(0)
    }

    BitcoinUtil {
        id : bitcoinUtil
    }


    Rectangle {
        id: mainBackground
        anchors.fill: parent
        color: Style.background_main
    }


    property var contentItems: [
        "CardInfo",
        "MultisigAddr"
    ]
    property int selectedItem : 0

    Item {
        id: sideBar
        width: 70
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.top: parent.top

        Rectangle {
            anchors.fill: parent
            color: Style.content_main
        }

        CustomComboBox {
            id: assetComboBox
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.top: parent.top
            anchors.topMargin: 60

            model: ["BTC","LTC","ETH","BCH"]

            onActivated: {
                $guiData.currentAsset = currentText
            }
        }

        Column {
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 130

            Repeater {
                id: controls
                model: contentItems

                Item {
                    id: control
                    width: parent.width
                    height: 66


                    Image {
                        id: icon
                        x: 21
                        y: 16
                        width: 28
                        height: 28
                        source: "qrc:/qml/pic/icon-" + modelData + (selectedItem == index ? "-active" : "") + ".svg"
                    }

                    Item {
                        visible: control.activeFocus
                        Rectangle {
                            id: indicator
                            y: 6
                            width: 4
                            height: 48
                            color: Style.sideBar_active
                        }

                        DropShadow {
                            anchors.fill: indicator
                            radius: 5
                            samples: 9
                            color: Style.sideBar_active
                            source: indicator
                        }
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        onClicked: {
                            control.focus = true
                            if(selectedItem != index)
                                selectedItem = index
                            updatePage(index)
                        }
                    }
                }
            }
        }
    }

    Loader {
        id: pageLoader
        anchors.fill: parent
        anchors.topMargin: 30
        anchors.leftMargin: 90
        anchors.bottomMargin: 30
        anchors.rightMargin: 30
        focus: true
    }

    function updatePage(index) {
        selectedItem = index
        controls.itemAt(index).focus = true
        pageLoader.setSource("qrc:/qml/" + contentItems[index] + ".qml")
    }
}
