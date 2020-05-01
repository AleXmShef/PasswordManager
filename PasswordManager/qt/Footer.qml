import QtQuick 2.0

Item {
    anchors.bottom: parent.bottom
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottomMargin: 15
    height: copyrightText.height
    Text {
        id: copyrightText
        anchors.centerIn: parent
        font.pointSize: 12
        color: "white"
        text: "Aleksandr Shafir, 2020"
    }
}
