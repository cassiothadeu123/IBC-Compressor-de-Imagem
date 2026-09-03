#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "ImageCompressor.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("IBC Compressor de Imagem"));
    QGuiApplication::setOrganizationName(QStringLiteral("Instituto Biofísica da Consciência"));
    QGuiApplication::setOrganizationDomain(QStringLiteral("ibchertz.com.br"));
    QQuickStyle::setStyle(QStringLiteral("Basic"));

    ImageCompressor compressor;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("imageCompressor"), &compressor);
    engine.loadFromModule(QStringLiteral("IBC.Compressor"), QStringLiteral("Main"));

    if (engine.rootObjects().isEmpty())
        return EXIT_FAILURE;
    return app.exec();
}
