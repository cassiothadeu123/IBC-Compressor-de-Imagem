#include "ImageCompressor.h"

#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>
#include <QStandardPaths>
#include <QUrl>
#include <QtConcurrent>
#include <algorithm>
#include <cmath>
#include <utility>

namespace {
QByteArray normalizedFormat(const QString &format)
{
    const QByteArray f = format.trimmed().toLower().toLatin1();
    if (f == "jpg" || f == "jpeg") return "jpeg";
    if (f == "avif") return "avif";
    if (f == "png") return "png";
    return "webp";
}

QString suffixFor(const QByteArray &format)
{
    return format == "jpeg" ? QStringLiteral("jpg") : QString::fromLatin1(format);
}

QByteArray encode(const QImage &image, const QByteArray &format, int quality, bool stripMetadata)
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    QImageWriter writer(&buffer, format);
    writer.setQuality(quality);
    writer.setOptimizedWrite(true);
    writer.setProgressiveScanWrite(format == "jpeg");
    if (!stripMetadata)
        writer.setText(QStringLiteral("Software"), QStringLiteral("IBC Compressor de Imagem"));
    if (!writer.write(image))
        return {};
    return bytes;
}
}

ImageCompressor::ImageCompressor(QObject *parent) : QObject(parent) {}

QStringList ImageCompressor::supportedFormats() const
{
    QStringList result;
    const auto formats = QImageWriter::supportedImageFormats();
    for (const auto &wanted : {QByteArray("avif"), QByteArray("webp"), QByteArray("jpeg"), QByteArray("png")}) {
        if (formats.contains(wanted))
            result << (wanted == "jpeg" ? QStringLiteral("JPEG") : QString::fromLatin1(wanted).toUpper());
    }
    return result.isEmpty() ? QStringList{QStringLiteral("JPEG")} : result;
}

QString ImageCompressor::defaultOutputDirectory() const
{
    return QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
           + QStringLiteral("/IBC Comprimidas");
}

QString ImageCompressor::localPath(const QString &url) const
{
    const QUrl parsed(url);
    return parsed.isLocalFile() ? parsed.toLocalFile() : url;
}

void ImageCompressor::setBusy(bool value)
{
    if (m_busy == value) return;
    m_busy = value;
    emit busyChanged();
}

void ImageCompressor::setProgress(double value)
{
    if (qFuzzyCompare(m_progress, value)) return;
    m_progress = value;
    emit progressChanged();
}

void ImageCompressor::setStatus(const QString &value)
{
    if (m_status == value) return;
    m_status = value;
    emit statusChanged();
}

void ImageCompressor::cancel()
{
    m_cancelled = true;
    setStatus(QStringLiteral("Cancelando…"));
}

void ImageCompressor::compress(const QStringList &urls, const QString &outputDirectory,
                               int targetKb, const QString &format,
                               bool visuallyLossless, bool allowResize,
                               bool stripMetadata)
{
    if (m_busy || urls.isEmpty()) return;

    QStringList paths;
    paths.reserve(urls.size());
    for (const auto &url : urls) paths << localPath(url);

    Options options;
    options.targetBytes = std::max(10, targetKb) * 1024LL;
    options.format = normalizedFormat(format);
    options.visuallyLossless = visuallyLossless;
    options.allowResize = allowResize;
    options.stripMetadata = stripMetadata;

    const QString out = outputDirectory.isEmpty() ? defaultOutputDirectory() : localPath(outputDirectory);
    QDir().mkpath(out);
    m_results.clear();
    emit resultsChanged();
    m_cancelled = false;
    setProgress(0.0);
    setBusy(true);
    setStatus(QStringLiteral("Analisando imagens…"));

    auto *watcher = new QFutureWatcher<QVariantMap>(this);
    connect(watcher, &QFutureWatcher<QVariantMap>::resultReadyAt, this,
            [this, watcher, count = paths.size()](int index) {
        m_results.append(watcher->resultAt(index));
        emit resultsChanged();
        setProgress(double(m_results.size()) / double(count));
        setStatus(QStringLiteral("Processando %1 de %2").arg(m_results.size()).arg(count));
    });
    connect(watcher, &QFutureWatcher<QVariantMap>::finished, this, [this, watcher] {
        int succeeded = 0;
        for (const auto &item : std::as_const(m_results))
            if (item.toMap().value(QStringLiteral("success")).toBool()) ++succeeded;
        const int failed = m_results.size() - succeeded;
        setBusy(false);
        setProgress(1.0);
        setStatus(m_cancelled ? QStringLiteral("Operação cancelada")
                              : QStringLiteral("Concluído: %1 arquivo(s)").arg(succeeded));
        emit compressionFinished(succeeded, failed);
        watcher->deleteLater();
    });

    watcher->setFuture(QtConcurrent::mapped(paths, [out, options, this](const QString &path) {
        return compressOne(path, out, options, &m_cancelled);
    }));
}

QVariantMap ImageCompressor::compressOne(const QString &sourcePath, const QString &outputDir,
                                         const Options &options, std::atomic_bool *cancelled)
{
    QVariantMap result{{QStringLiteral("source"), sourcePath},
                       {QStringLiteral("success"), false}};
    if (cancelled->load()) {
        result[QStringLiteral("message")] = QStringLiteral("Cancelado");
        return result;
    }

    QImageReader reader(sourcePath);
    reader.setAutoTransform(true);
    QImage image = reader.read();
    if (image.isNull()) {
        result[QStringLiteral("message")] = reader.errorString();
        return result;
    }

    const qint64 originalBytes = QFileInfo(sourcePath).size();
    const QSize originalSize = image.size();
    QByteArray best;
    int bestQuality = 0;
    int resizePasses = 0;
    const int qualityFloor = options.visuallyLossless ? 55 : 1;

    for (; resizePasses < 14 && !cancelled->load(); ++resizePasses) {
        int low = qualityFloor;
        int high = 100;
        QByteArray candidate;
        int candidateQuality = 0;

        while (low <= high) {
            const int quality = (low + high) / 2;
            QByteArray encoded = encode(image, options.format, quality, options.stripMetadata);
            if (encoded.isEmpty()) {
                result[QStringLiteral("message")] = QStringLiteral("Codificador %1 indisponível")
                                                    .arg(QString::fromLatin1(options.format).toUpper());
                return result;
            }
            if (encoded.size() <= options.targetBytes) {
                candidate = std::move(encoded);
                candidateQuality = quality;
                low = quality + 1;
            } else {
                high = quality - 1;
            }
        }

        if (!candidate.isEmpty()) {
            best = std::move(candidate);
            bestQuality = candidateQuality;
            break;
        }
        if (!options.allowResize || image.width() < 640 || image.height() < 640)
            break;

        const QByteArray floorBytes = encode(image, options.format, qualityFloor, options.stripMetadata);
        const double ratio = floorBytes.isEmpty() ? 0.85
            : std::clamp(std::sqrt(double(options.targetBytes) / double(floorBytes.size())) * 0.97, 0.55, 0.90);
        const QSize nextSize(std::max(320, int(image.width() * ratio)),
                             std::max(320, int(image.height() * ratio)));
        image = image.scaled(nextSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    if (best.isEmpty()) {
        result[QStringLiteral("message")] = QStringLiteral("O limite não pode ser atingido com as regras escolhidas");
        result[QStringLiteral("originalBytes")] = originalBytes;
        return result;
    }

    const QFileInfo sourceInfo(sourcePath);
    const QString outputPath = QDir(outputDir).filePath(
        sourceInfo.completeBaseName() + QStringLiteral("_IBC.") + suffixFor(options.format));
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly) || file.write(best) != best.size()) {
        result[QStringLiteral("message")] = QStringLiteral("Não foi possível gravar o arquivo");
        return result;
    }

    result[QStringLiteral("success")] = true;
    result[QStringLiteral("output")] = outputPath;
    result[QStringLiteral("originalBytes")] = originalBytes;
    result[QStringLiteral("compressedBytes")] = best.size();
    result[QStringLiteral("quality")] = bestQuality;
    result[QStringLiteral("originalWidth")] = originalSize.width();
    result[QStringLiteral("originalHeight")] = originalSize.height();
    result[QStringLiteral("width")] = image.width();
    result[QStringLiteral("height")] = image.height();
    result[QStringLiteral("reduction")] = originalBytes > 0
        ? 100.0 * (1.0 - double(best.size()) / double(originalBytes)) : 0.0;
    result[QStringLiteral("message")] = image.size() == originalSize
        ? QStringLiteral("Dimensões preservadas")
        : QStringLiteral("Redimensionada de forma adaptativa");
    return result;
}
