import QtQuick

Rectangle {
    radius: 22
    color: "#160b2436"
    border.color: "#365fd8d4"
    border.width: 1

    Rectangle {
        anchors.fill: parent
        anchors.margins: 1
        radius: parent.radius - 1
        color: "transparent"
        border.color: "#18ffffff"
    }
}
