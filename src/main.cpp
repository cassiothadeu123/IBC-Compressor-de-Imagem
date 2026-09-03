#include <QGuiApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QStandardPaths>
#include <QTextStream>
#include <cstdlib>

#include "ImageCompressor.h"

namespace {
QString startupLogPath()
{
    const QString directory = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
                              + QStringLiteral("/logs");
    QDir().mkpath(directory);
    return directory + QStringLiteral("/startup.log");
}

void writeStartupLog(const QString &message)
{
    QFile file(startupLogPath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;
    QTextStream stream(&file);
    stream << QDateTime::currentDateTime().toString(Qt::ISODateWithMs)
           << QStringLiteral("  ") << message << Qt::endl;
}

void qtMessageHandler(QtMsgType type, const QMessageLogContext &, const QString &message)
{
    const QString level = type == QtFatalMsg ? QStringLiteral("FATAL")
                         : type == QtCriticalMsg ? QStringLiteral("CRITICAL")
                         : type == QtWarningMsg ? QStringLiteral("WARNING")
                         : type == QtInfoMsg ? QStringLiteral("INFO")
                         : QStringLiteral("DEBUG");
    writeStartupLog(level + QStringLiteral(": ") + message);
    if (type == QtFatalMsg)
        abort();
}
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("IBC Compressor de Imagem"));
    QGuiApplication::setOrganizationName(QStringLiteral("Instituto Biofísica da Consciência"));
    QGuiApplication::setOrganizationDomain(QStringLiteral("ibchertz.com.br"));
    qInstallMessageHandler(qtMessageHandler);
    writeStartupLog(QStringLiteral("Inicialização da versão 0.1.1"));
    writeStartupLog(QStringLiteral("Plataforma: ") + QGuiApplication::platformName());
    QQuickStyle::setStyle(QStringLiteral("Basic"));

    ImageCompressor compressor;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("imageCompressor"), &compressor);
    engine.loadFromModule(QStringLiteral("IBC.Compressor"), QStringLiteral("Main"));

    if (engine.rootObjects().isEmpty()) {
        writeStartupLog(QStringLiteral("ERRO: a interface QML não criou a janela principal"));
        return EXIT_FAILURE;
    }
    writeStartupLog(QStringLiteral("Interface carregada com sucesso"));
    return app.exec();
}
