#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QtTest>

#include "ImageCompressor.h"

class ImageCompressorTest final : public QObject
{
    Q_OBJECT

private slots:
    void reachesTargetWithoutOverwritingOriginal()
    {
        QTemporaryDir temporary;
        QVERIFY(temporary.isValid());

        QImage image(3200, 2200, QImage::Format_RGB32);
        for (int y = 0; y < image.height(); ++y) {
            auto *line = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < image.width(); ++x)
                line[x] = qRgb((x * 17 + y * 3) % 256,
                               (x * 5 + y * 11) % 256,
                               (x * 13 + y * 7) % 256);
        }

        const QString source = temporary.filePath(QStringLiteral("entrada.png"));
        QVERIFY(image.save(source, "PNG"));
        const qint64 sourceSize = QFileInfo(source).size();

        ImageCompressor compressor;
        QSignalSpy finished(&compressor, &ImageCompressor::compressionFinished);
        compressor.compress({source}, temporary.path(), 900,
                            QStringLiteral("JPEG"), true, true, true);
        QVERIFY2(finished.wait(60000), "A compressão não terminou dentro do limite");
        QCOMPARE(finished.first().at(0).toInt(), 1);
        QCOMPARE(finished.first().at(1).toInt(), 0);

        const QVariantMap result = compressor.results().first().toMap();
        QVERIFY(result.value(QStringLiteral("success")).toBool());
        const QString output = result.value(QStringLiteral("output")).toString();
        QVERIFY(QFileInfo::exists(output));
        QVERIFY(QFileInfo(output).size() <= 900 * 1024);
        QCOMPARE(QFileInfo(source).size(), sourceSize);
        QVERIFY(output != source);

        QImageReader verification(output);
        QVERIFY2(verification.canRead(), qPrintable(verification.errorString()));
        QVERIFY(!verification.read().isNull());
    }

    void reportsImpossibleLimitWithoutResize()
    {
        QTemporaryDir temporary;
        QVERIFY(temporary.isValid());
        QImage image(2500, 1800, QImage::Format_RGB32);
        for (int y = 0; y < image.height(); ++y) {
            auto *line = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < image.width(); ++x)
                line[x] = qRgb((x * 31 + y * 19) % 256,
                               (x * 23 + y * 29) % 256,
                               (x * 37 + y * 41) % 256);
        }
        const QString source = temporary.filePath(QStringLiteral("grande.png"));
        QVERIFY(image.save(source, "PNG"));

        ImageCompressor compressor;
        QSignalSpy finished(&compressor, &ImageCompressor::compressionFinished);
        compressor.compress({source}, temporary.path(), 10,
                            QStringLiteral("JPEG"), true, false, true);
        QVERIFY(finished.wait(60000));
        QCOMPARE(finished.first().at(0).toInt(), 0);
        QCOMPARE(finished.first().at(1).toInt(), 1);
    }
};

QTEST_GUILESS_MAIN(ImageCompressorTest)
#include "ImageCompressorTest.moc"
