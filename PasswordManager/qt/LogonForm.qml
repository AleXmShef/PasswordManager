import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.14

Item {
    function playOpenAnim() {
        function callback () {layout.playOpenAnim()}
        keyImage.playOpenAnim(callback)
        listView.populateList()
        scaleAnim.running = true
    }

    function playFailedAnim() {
        keyImage.playErrorAnim()
    }

    anchors.topMargin: 20
    anchors.top: parent.top
    width: parent.width
    height: parent.height * scaleAnim._scale
    transform: Scale {yScale: scaleAnim._scale}

    PropertyAnimation {
        property double _scale: 1
        id: scaleAnim
        target: scaleAnim
        property: "_scale"
        to: 0
        duration: 500
        easing.type: Easing.InOutQuad
    }

    ColumnLayout {
        Text {
            font.pointSize: 20
            text: "Enter your password:"
            color: "gray"
            Layout.alignment: Qt.AlignCenter
        }
        Rectangle {
            width: 500
            height: 50
            TextField {
                id: password
                anchors.fill: parent
                anchors.leftMargin: 50
                placeholderText: "password"
                echoMode: TextInput.Password
                font.pointSize: 20
                verticalAlignment: TextInput.AlignVCenter
                background: Rectangle {
                    radius: 8
                    border.color: "lightblue"
                    border.width: 2
                }

                Keys.onReturnPressed: {
                    var result = UIConnector.decryptFile(password.text)
                    if(result) {
                        title.playLockOpenAnim()

                    }
                    else {
                        console.log("Wrong password")
                        keyImage.playErrorAnim()
                    }
                }
            }
            Image {
                id: keyImage
                property int rotDeg: 0
                function playOpenAnim(callback) {
                    openAnim.callback = callback
                    openAnim.running = true
                }
                function playErrorAnim() {
                    errorAnim.running = true
                }

                anchors.left: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 10
                source: "../assets/key.svg"
                fillMode: Image.PreserveAspectFit
                height: 30
                transform: Rotation {origin.y: (keyImage.height/2); axis {x:1;y:0;z:0} angle: keyImage.rotDeg}
                PropertyAnimation {
                    property var callback
                    id: openAnim
                    target: keyImage
                    properties: "rotDeg"
                    easing.type: Easing.InOutQuad
                    duration: 800
                    to: 90
                    onRunningChanged: {
                        if(!running) {
                            callback()
                        }
                    }
                }
                SequentialAnimation {
                    id: errorAnim
                    PropertyAnimation {
                        target: keyImage
                        properties: "rotDeg"
                        easing.type: Easing.InOutQuad
                        duration: 500
                        to: 90
                    }
                    PropertyAnimation {
                        target: keyImage
                        properties: "rotDeg"
                        easing.type: Easing.InOutQuad
                        duration: 500
                        to: 0
                    }
                }
            }
        }
        Text {
            font.pointSize: 20
            text: "or:"
            color: "gray"
            Layout.alignment: Qt.AlignCenter
        }
        Rectangle {
            //visible: false
            width: 5
            height: 50
            Layout.alignment: Qt.AlignCenter
        }
    }
}
