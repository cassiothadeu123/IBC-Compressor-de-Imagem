import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import IBC.Compressor

ApplicationWindow {
    id: window
    width: 1280
    height: 820
    minimumWidth: 980
    minimumHeight: 680
    visible: true
    title: "IBC Compressor de Imagem"
    color: "#050b13"

    property var selectedFiles: []
    property string outputPath: imageCompressor.defaultOutputDirectory()
    property real animationTime: 0

    NumberAnimation on animationTime {
        from: 0; to: 1000; duration: 600000
        loops: Animation.Infinite
        running: true
    }

    ShaderEffect {
        anchors.fill: parent
        property real time: window.animationTime
        fragmentShader: "qrc:/shaders/shaders/holographic.frag.qsb"
        opacity: 0.9
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0; color: "#d9050b13" }
            GradientStop { position: 0.52; color: "#b5071422" }
            GradientStop { position: 1; color: "#e5050b13" }
        }
    }

    FileDialog {
        id: imageDialog
        title: "Escolha uma ou várias imagens"
        fileMode: FileDialog.OpenFiles
        nameFilters: ["Imagens (*.jpg *.jpeg *.png *.webp *.avif *.bmp *.tif *.tiff)", "Todos os arquivos (*)"]
        onAccepted: {
            const incoming = Array.from(window.selectedFiles)
            for (let i = 0; i < imageDialog.selectedFiles.length; ++i) {
                if (incoming.indexOf(imageDialog.selectedFiles[i]) < 0) incoming.push(imageDialog.selectedFiles[i])
            }
            window.selectedFiles = incoming
        }
    }

    FolderDialog {
        id: outputDialog
        title: "Escolha a pasta de destino"
        onAccepted: window.outputPath = imageCompressor.localPath(selectedFolder)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 28
        spacing: 20

        RowLayout {
            Layout.fillWidth: true
            spacing: 15

            Image {
                source: "qrc:/qt/qml/IBC/Compressor/assets/ibc-compressor.svg"
                sourceSize: Qt.size(62, 62)
                Layout.preferredWidth: 62
                Layout.preferredHeight: 62
            }

            ColumnLayout {
                spacing: 1
                Text { text: "IBC COMPRESSOR"; color: "#d9fff9"; font.pixelSize: 25; font.weight: Font.Bold; font.letterSpacing: 2 }
                Text { text: "DE IMAGEM"; color: "#68daca"; font.pixelSize: 12; font.weight: Font.DemiBold; font.letterSpacing: 5 }
            }

            Item { Layout.fillWidth: true }
            Rectangle {
                radius: 14
                color: "#122537"
                border.color: "#3b887f"
                implicitWidth: 230
                implicitHeight: 42
                Row {
                    anchors.centerIn: parent
                    spacing: 9
                    Rectangle { width: 8; height: 8; radius: 4; color: imageCompressor.busy ? "#f3c66a" : "#79f3cb" }
                    Text { text: "100% OFFLINE • PRIVADO"; color: "#b7dcd8"; font.pixelSize: 12; font.weight: Font.DemiBold }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 20

            GlassCard {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 760

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 26
                    spacing: 18

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        radius: 18
                        color: drop.hovered ? "#162b3e" : "#0d1c2a"
                        border.width: 2
                        border.color: drop.hovered ? "#87f6e4" : "#377a86"

                        DropArea {
                            id: drop
                            anchors.fill: parent
                            keys: ["text/uri-list"]
                            onDropped: function(drop) {
                                if (drop.hasUrls) {
                                    let incoming = Array.from(window.selectedFiles)
                                    for (let i = 0; i < drop.urls.length; ++i)
                                        if (incoming.indexOf(drop.urls[i]) < 0) incoming.push(drop.urls[i])
                                    window.selectedFiles = incoming
                                }
                            }
                        }

                        Column {
                            anchors.centerIn: parent
                            width: parent.width * 0.78
                            spacing: 14
                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: selectedFiles.length === 0 ? "✦" : selectedFiles.length
                                color: "#79edda"; font.pixelSize: selectedFiles.length === 0 ? 58 : 42; font.weight: Font.Light
                            }
                            Text {
                                width: parent.width
                                horizontalAlignment: Text.AlignHCenter
                                text: selectedFiles.length === 0 ? "Arraste suas imagens para cá" :
                                      selectedFiles.length + (selectedFiles.length === 1 ? " imagem selecionada" : " imagens selecionadas")
                                color: "#ecfffc"; font.pixelSize: 21; font.weight: Font.DemiBold
                            }
                            Text {
                                width: parent.width
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                text: selectedFiles.length === 0 ? "JPEG, PNG, WebP, AVIF e TIFF • processamento em lote" :
                                      "O processamento será feito somente neste computador"
                                color: "#8eabb4"; font.pixelSize: 14
                            }
                            GlowButton {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: selectedFiles.length === 0 ? "ESCOLHER IMAGENS" : "ADICIONAR MAIS"
                                enabled: !imageCompressor.busy
                                onClicked: imageDialog.open()
                            }
                            Button {
                                anchors.horizontalCenter: parent.horizontalCenter
                                visible: selectedFiles.length > 0 && !imageCompressor.busy
                                text: "Limpar seleção"
                                flat: true
                                contentItem: Text { text: parent.text; color: "#7ca6ad"; horizontalAlignment: Text.AlignHCenter }
                                onClicked: window.selectedFiles = []
                            }
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        visible: imageCompressor.busy || imageCompressor.progress > 0
                        spacing: 7
                        RowLayout {
                            Layout.fillWidth: true
                            Text { text: imageCompressor.status; color: "#c7e4e2"; font.pixelSize: 13 }
                            Item { Layout.fillWidth: true }
                            Text { text: Math.round(imageCompressor.progress * 100) + "%"; color: "#80eddb"; font.bold: true }
                        }
                        ProgressBar {
                            Layout.fillWidth: true
                            value: imageCompressor.progress
                            background: Rectangle { implicitHeight: 7; radius: 4; color: "#193040" }
                            contentItem: Item {
                                implicitHeight: 7
                                Rectangle { width: parent.width * imageCompressor.progress; height: parent.height; radius: 4; color: "#79edda" }
                            }
                        }
                    }
                }
            }

            GlassCard {
                Layout.preferredWidth: 390
                Layout.fillHeight: true

                ScrollView {
                    anchors.fill: parent
                    anchors.margins: 24
                    clip: true
                    contentWidth: availableWidth

                    ColumnLayout {
                        width: parent.width
                        spacing: 18

                        Text { text: "CONFIGURAÇÃO INTELIGENTE"; color: "#d8f9f5"; font.pixelSize: 14; font.bold: true; font.letterSpacing: 1.3 }

                        ColumnLayout {
                            Layout.fillWidth: true; spacing: 7
                            Text { text: "Tamanho máximo por imagem"; color: "#a9c8ca"; font.pixelSize: 13 }
                            RowLayout {
                                SpinBox {
                                    id: targetSize
                                    Layout.fillWidth: true
                                    from: 50; to: 102400; value: 900; stepSize: 50; editable: true
                                }
                                Text { text: "KB"; color: "#79edda"; font.bold: true }
                            }
                            Text { text: "Exemplo: 900 KB para portais e envio rápido"; color: "#647f88"; font.pixelSize: 11 }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true; spacing: 7
                            Text { text: "Formato de saída"; color: "#a9c8ca"; font.pixelSize: 13 }
                            ComboBox { id: formatBox; Layout.fillWidth: true; model: imageCompressor.supportedFormats; currentIndex: 0 }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true; spacing: 7
                            Text { text: "Estratégia"; color: "#a9c8ca"; font.pixelSize: 13 }
                            ComboBox {
                                id: strategyBox; Layout.fillWidth: true
                                model: ["Qualidade visual preservada", "Compressão máxima"]
                            }
                        }

                        CheckBox {
                            id: resizeCheck; checked: true
                            text: "Redimensionar se necessário"
                            contentItem: Text { text: parent.text; color: "#b9d3d3"; leftPadding: parent.indicator.width + parent.spacing; verticalAlignment: Text.AlignVCenter }
                        }
                        CheckBox {
                            id: metadataCheck; checked: true
                            text: "Remover metadados privados"
                            contentItem: Text { text: parent.text; color: "#b9d3d3"; leftPadding: parent.indicator.width + parent.spacing; verticalAlignment: Text.AlignVCenter }
                        }

                        Rectangle { Layout.fillWidth: true; height: 1; color: "#284350" }

                        ColumnLayout {
                            Layout.fillWidth: true; spacing: 6
                            Text { text: "Pasta de destino"; color: "#a9c8ca"; font.pixelSize: 13 }
                            Text { Layout.fillWidth: true; text: window.outputPath; color: "#78969e"; elide: Text.ElideMiddle; font.pixelSize: 11 }
                            Button {
                                text: "ALTERAR PASTA"; flat: true
                                contentItem: Text { text: parent.text; color: "#78dcca"; font.bold: true; horizontalAlignment: Text.AlignLeft }
                                onClicked: outputDialog.open()
                            }
                        }

                        Item { Layout.fillHeight: true; Layout.minimumHeight: 8 }

                        GlowButton {
                            Layout.fillWidth: true
                            text: imageCompressor.busy ? "PROCESSANDO…" : "COMPRIMIR AGORA"
                            enabled: selectedFiles.length > 0 && !imageCompressor.busy
                            onClicked: imageCompressor.compress(
                                selectedFiles, outputPath, targetSize.value, formatBox.currentText,
                                strategyBox.currentIndex === 0, resizeCheck.checked, metadataCheck.checked)
                        }
                        Button {
                            Layout.fillWidth: true
                            visible: imageCompressor.busy
                            text: "Cancelar"
                            onClicked: imageCompressor.cancel()
                        }
                        Text {
                            Layout.fillWidth: true
                            text: "O IBC Compressor busca o maior nível de qualidade possível dentro do tamanho escolhido."
                            wrapMode: Text.WordWrap; horizontalAlignment: Text.AlignHCenter
                            color: "#617e87"; font.pixelSize: 11; lineHeight: 1.25
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Text { text: "Tecnologia IBC Hertz • Compressão adaptativa local"; color: "#56737d"; font.pixelSize: 11 }
            Item { Layout.fillWidth: true }
            Text { text: "v0.1.1"; color: "#56737d"; font.pixelSize: 11 }
        }
    }
}
