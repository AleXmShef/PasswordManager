import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.14
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12
import QtQuick.Shapes 1.12
import app 1.0

Window {
    id: window
    visible: true
    width: 1280
    height: 720
    title: qsTr("Password Manager")
    color: "dodgerblue"

    function open() {
        title.playLockOpenAnim()
        function callback() {title.playOpenAnim()}
        MainField.playOpenAnim(callback)
    }

    function openFailed() {

    }

    function tryLogon(password) {
        var result = UIConnector.decryptFile(password)
        if(result) {
            open()
        }
        else
            openFailed()
    }

    Header {
        id: title
        antialiasing: true
    }

    Rectangle {
        id: mainRectangle
        function playOpenAnim() {
            _wtf.running = true
        }

        radius: 40
        width: 620
        //height: 800
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 50
        anchors.bottomMargin: 30
        anchors.top: title.bottom
        anchors.bottom: copyright.top
        color: "white"
        PropertyAnimation {
            id: _wtf
            target: mainRectangle
            properties: "width"
            to: 800
            duration: 500
            easing.type: Easing.InOutQuad
        }

        ColumnLayout {
            property double _yScale: 1
            function playOpenAnim() {
                openAnim2.running = true
            }

            anchors.topMargin: 20
            anchors.top: parent.top
            width: parent.width
            height: parent.height * _yScale
            id: layout
            transform: Scale {yScale: layout._yScale}
            PropertyAnimation {
                id: openAnim2
                target: layout
                property: "_yScale"
                to: 0
                duration: 500
                easing.type: Easing.InOutQuad
            }

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
                            function callback () {layout.playOpenAnim();title.playOpenAnim();mainRectangle.playOpenAnim()}
                            keyImage.playOpenAnim(callback)
                            listView.populateList()
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
                    SequentialAnimation {
                        id: openAnim
                        property var callback
                        PropertyAnimation {
                            target: keyImage
                            properties: "rotDeg"
                            easing.type: Easing.InOutQuad
                            duration: 400
                            to: 90
                        }
                        PropertyAnimation {
                            target: keyImage
                            properties: "rotDeg"
                            easing.type: Easing.InOutQuad
                            duration: 100
                            to: 90
                        }
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
                            duration: 250
                            to: 90
                        }
                        PropertyAnimation {
                            target: keyImage
                            properties: "rotDeg"
                            easing.type: Easing.InOutQuad
                            duration: 250
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
        Rectangle {
            property double _yScale: 2
            id: listViewContainer
            border.width: 2
            border.color: "lightblue"
            anchors.top: layout.bottom
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 40
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            anchors.right: parent.right
            anchors.left: parent.left
            Rectangle {
                id: searchRect
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: Math.min(parent.height, searchField.height)
                //border.color: "black"
                TextField {
                    width: 200
                    id: searchField
                    //anchors.fill: parent
                    placeholderText: "search"
                    color: "gray"
                    font.pointSize: 15
                    //verticalAlignment: TextInput.AlignVCenter
                    leftPadding: height
                    selectedTextColor: "lightblue"
                    selectionColor: "dodgerblue"
                    clip: true
                    Image {
                        fillMode: Image.PreserveAspectFit
                        source: "../assets/search_icon.svg"
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.margins: 10
                        Layout.alignment: Qt.AlignVCenter
                    }
                    background: Rectangle {
                        implicitWidth: 200
                        id: something
                        radius: 8
                        border.color: "dodgerblue"
                        border.width: 2

                    }
                    placeholderTextColor: "lightgray"
                }
                Rectangle {
                    width: 240
                }
                Image {
                    id: addImg
                    height: parent.height
                    width: height
                    anchors.right: editImg.left
                    anchors.rightMargin: 10
                    fillMode: Image.PreserveAspectFit
                    source: "../assets/Add.svg"
                }
                Image {
                    id: editImg
                    height: parent.height
                    width: height
                    anchors.right: deleteImg.left
                    anchors.rightMargin: 10
                    fillMode: Image.PreserveAspectFit
                    source: "../assets/Edit.svg"
                }
                Image {
                    id: deleteImg
                    height: parent.height
                    width: height
                    anchors.right: parent.right
                    fillMode: Image.PreserveAspectFit
                    source: "../assets/Delete.svg"
                }
            }
            Rectangle {
                anchors.top: searchRect.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                ListView {
                    id: listView
                    function populateList() {
                        var data = UIConnector.getData()
                        var passwords = data.passwords
                        passwords.forEach(password => {
                                listModel.append({"domain": password.domain, "login": password.login, "password": password.password})
                            }
                        )
                    }
                    Shape {
                        anchors.fill: parent
                        ShapePath {
                            strokeWidth: 1
                            strokeColor: "darkgray"
                            startX: 0
                            startY: 0
                            PathLine {x: listView.width; y: 0}
                        }
                    }
                    Shape {
                        anchors.fill: parent
                        ShapePath {
                            strokeWidth: 2
                            strokeColor: "darkgray"
                            startX: 0
                            startY: listView.height
                            PathLine {x: listView.width; y: listView.height}
                        }
                    }

                    anchors.fill: parent
                    anchors.topMargin: 10
                    spacing: 2
                    Component {
                        id: listDelegate

                        Item {
                            id: delegate
                            height: delegateRow.height * 1.2
                            width: listView.width
                            Shape {
                                antialiasing: true
                                anchors.fill: parent
                                ShapePath {
                                    strokeWidth: 1
                                    strokeColor: "darkgray"
                                    startX: 20
                                    startY: delegate.height + 1
                                    PathLine {x: delegateRow.width + 20; y: delegate.height + 1}
                                }
                            }
                            RowLayout {
                                id: delegateRow
                                spacing: 0
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                anchors.right: parent.right
                                height: domainText.height
                                anchors.leftMargin: 20
                                anchors.rightMargin: 20
                                Rectangle {
                                    Layout.fillWidth: true
                                    height: domainText.height
                                    //border.color: "black"
                                    Text {
                                        width: parent.width
                                        anchors.leftMargin: 4
                                        anchors.left: parent.left
                                        id: domainText
                                        font.pointSize: 15
                                        color: "gray"
                                        text: domain
                                    }
                                }
                                Rectangle {
                                    Layout.fillWidth: true
                                    height: loginText.height
                                    //border.color: "black"
                                    Text {
                                        width: parent.width
                                        anchors.leftMargin: 4
                                        anchors.left: parent.left
                                        horizontalAlignment: Text.AlignHCenter
                                        id: loginText
                                        font.pointSize: 15
                                        color: "gray"
                                        text: login
                                    }
                                }
                                Rectangle {
                                    Layout.fillWidth: true
                                    height: passwordText.height
                                    //border.color: "black"
                                    Text {
                                        anchors.leftMargin: 4
                                        anchors.rightMargin: 4
                                        anchors.right: parent.right
                                        anchors.left: parent.left
                                        //width: parent.width
                                        horizontalAlignment: Text.AlignRight
                                        id: passwordText
                                        font.pointSize: 15
                                        color: "gray"
                                        text: password
                                    }
                                }
                            }
                        }
                    }
                    ListModel {
                        id: listModel
                        ListElement {domain: "vk123123123123123123.com"; login: "test"; password: "hello"}
                        ListElement {domain: "123.com"; login: "test"; password: "hello"}
                        ListElement {domain: "hi.com"; login: "test"; password: "hello"}
                        ListElement {domain: "vk.com"; login: "test"; password: "hello"}
                        ListElement {domain: "123.com"; login: "test"; password: "hello"}
                        ListElement {domain: "hi.com"; login: "test"; password: "hello"}
                        ListElement {domain: "vk.com"; login: "test"; password: "hello"}
                        ListElement {domain: "123.com"; login: "test"; password: "hello"}
                        ListElement {domain: "hi.com"; login: "test"; password: "hello"}
                        ListElement {domain: "vk.com"; login: "test"; password: "hello"}
                        ListElement {domain: "123.com"; login: "test"; password: "hello"}
                        ListElement {domain: "hi.com"; login: "test"; password: "hello"}
                        ListElement {domain: "vk.com"; login: "test"; password: "hello"}
                        ListElement {domain: "123.com"; login: "test"; password: "hello"}
                        ListElement {domain: "hi.com"; login: "test"; password: "hello"}
                        ListElement {domain: "vk.com"; login: "test"; password: "hello"}
                        ListElement {domain: "123.com"; login: "test"; password: "hello"}
                        ListElement {domain: "hi.com"; login: "test"; password: "hello"}
                        ListElement {domain: "vk.com"; login: "test"; password: "hello"}
                        ListElement {domain: "123.com"; login: "test"; password: "hello"}
                        ListElement {domain: "hi.com"; login: "test"; password: "hello"}
                        ListElement {domain: "hi.com"; login: "test"; password: "hello"}
                        ListElement {domain: "vk.com"; login: "test"; password: "hello"}
                        ListElement {domain: "123.com"; login: "test"; password: "hello"}
                        ListElement {domain: "hi.com"; login: "test"; password: "hello"}
                        ListElement {domain: "vk.com"; login: "test"; password: "hello"}
                        ListElement {domain: "123.com"; login: "test"; password: "hello"}
                        ListElement {domain: "hi.com"; login: "test"; password: "hello"}
                        ListElement {domain: "hi.com"; login: "test"; password: "hello"}
                        ListElement {domain: "vk.com"; login: "test"; password: "hello"}
                        ListElement {domain: "123.com"; login: "test"; password: "hello"}
                        ListElement {domain: "hi.com"; login: "test"; password: "hello"}
                        ListElement {domain: "vk.com"; login: "test"; password: "hello"}
                        ListElement {domain: "123.com"; login: "test"; password: "hello"}
                        ListElement {domain: "hi.com"; login: "test"; password: "hello"}
                        ListElement {domain: "hi.com"; login: "test"; password: "hello"}
                        ListElement {domain: "vk.com"; login: "test"; password: "hello"}
                        ListElement {domain: "123.com"; login: "test"; password: "hello"}
                        ListElement {domain: "hi.com"; login: "test"; password: "hello"}
                        ListElement {domain: "vk.com"; login: "test"; password: "hello"}
                        ListElement {domain: "123.com"; login: "test"; password: "hello"}
                        ListElement {domain: "hi.com"; login: "test"; password: "hello"}
                    }
                    model: listModel
                    delegate: listDelegate
                    clip: true
                    boundsMovement: Flickable.StopAtBounds
                    ScrollBar.vertical: ScrollBar {minimumSize: 0.3; width: 11 }
                }
            }
        }
    }
    Footer {
        id: copyright
    }
}
