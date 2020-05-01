import QtQuick 2.0

Item {
    id: mainRectangle
    function playOpenAnim(callback) {
        widthAnim.running = true
        LogonForm.playOpenAnim()
    }
    width: widthAnim._width
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.topMargin: 50
    anchors.bottomMargin: 30
    //anchors.top: title.bottom
    //anchors.bottom: copyright.top

    PropertyAnimation {
        property int _width: 620
        id: widthAnim
        target: widthAnim
        properties: "_width"
        to: 800
        duration: 500
        easing.type: Easing.InOutQuad
    }
    Rectangle {
        anchors.fill: parent
        radius: 40
        color: "white"
    }
}
