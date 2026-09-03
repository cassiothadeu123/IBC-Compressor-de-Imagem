#pragma once

#include <QObject>
#include <QVariantList>
#include <atomic>

class ImageCompressor final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QVariantList results READ results NOTIFY resultsChanged)
    Q_PROPERTY(QStringList supportedFormats READ supportedFormats CONSTANT)

public:
    explicit ImageCompressor(QObject *parent = nullptr);

    bool busy() const { return m_busy; }
    double progress() const { return m_progress; }
    QString status() const { return m_status; }
    QVariantList results() const { return m_results; }
    QStringList supportedFormats() const;

    Q_INVOKABLE void compress(const QStringList &urls, const QString &outputDirectory,
                              int targetKb, const QString &format,
                              bool visuallyLossless, bool allowResize,
                              bool stripMetadata);
    Q_INVOKABLE void cancel();
    Q_INVOKABLE QString defaultOutputDirectory() const;
    Q_INVOKABLE QString localPath(const QString &url) const;

signals:
    void busyChanged();
    void progressChanged();
    void statusChanged();
    void resultsChanged();
    void compressionFinished(int succeeded, int failed);

private:
    struct Options {
        qint64 targetBytes = 900 * 1024;
        QByteArray format = "webp";
        bool visuallyLossless = true;
        bool allowResize = true;
        bool stripMetadata = true;
    };

    static QVariantMap compressOne(const QString &sourcePath, const QString &outputDir,
                                   const Options &options, std::atomic_bool *cancelled);
    void setBusy(bool value);
    void setProgress(double value);
    void setStatus(const QString &value);

    bool m_busy = false;
    double m_progress = 0.0;
    QString m_status = QStringLiteral("Pronto para comprimir");
    QVariantList m_results;
    std::atomic_bool m_cancelled = false;
};
