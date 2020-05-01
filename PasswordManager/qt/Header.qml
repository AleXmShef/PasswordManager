import QtQuick 2.14
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.14

Item {
    function playOpenAnim() {
        scaleAnim.running = true
        topMarginAnim.running = true
    }
    function playLockOpenAnim() {
        lockImage.playing = true
    }

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: parent.top
    anchors.topMargin: topMarginAnim._margin
    height: 100 * scaleAnim._scale
    width: 1280
    transform: Scale {origin.x: window.width/2; origin.y: 50; xScale: scaleAnim._scale; yScale:scaleAnim._scale}
    PropertyAnimation {
        property double _scale: 1
        id: scaleAnim
        target: scaleAnim
        property: "_scale"
        easing.type: Easing.InOutQuad
        to: 0.7
        duration: 1250
    }
    PropertyAnimation {
        property int _margin: 150
        id: topMarginAnim
        target: topMarginAnim
        properties: "_margin"
        easing.type: Easing.InOutQuad
        to: 50
        duration: 1250
        onRunningChanged: {
            if(!running)
                console.log(parent.anchors.topMargin)
        }
    }

    Rectangle {
        id: imageContainer
        width: 100
        height: 100
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        radius: 20
        AnimatedImage {
            antialiasing: true
            id: lockImage
            anchors.fill: parent
            source: "../assets/Lock.gif"
            visible: false
            speed: 1.4
            playing: false
            onCurrentFrameChanged: {
                if(currentFrame + 1 === frameCount) {
                    playing = false;
                }
            }
        }
        OpacityMask {
            anchors.fill: parent
            source: lockImage
            maskSource: imageContainer
            visible: true
            y: -5
        }
    }

    Text {
        anchors.right: imageContainer.left
        anchors.verticalCenter: imageContainer.verticalCenter
        anchors.rightMargin: 20
        text: "Password"
        color: "white"
        font.pointSize: 35
    }

    Text {
        anchors.left: imageContainer.right
        anchors.verticalCenter: imageContainer.verticalCenter
        anchors.leftMargin: 20
        text: "Manager"
        color: "white"
        font.pointSize: 35
    }
}
