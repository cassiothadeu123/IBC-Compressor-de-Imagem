import QtQuick
import QtQuick.Controls

Button {
    id: control
    implicitHeight: 52
    implicitWidth: 190
    font.pixelSize: 15
    font.weight: Font.DemiBold

    contentItem: Text {
        text: control.text
        color: control.enabled ? "#071419" : "#66767c"
        font: control.font
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        radius: 14
        gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop { position: 0; color: control.pressed ? "#55cfc5" : "#8ff5df" }
            GradientStop { position: 1; color: control.pressed ? "#58a9d2" : "#76caff" }
        }
        opacity: control.enabled ? 1 : 0.35
        border.color: "#b9fff4"
        border.width: 1
    }
}
