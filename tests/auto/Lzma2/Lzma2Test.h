#ifndef LZMA2TEST_H
#define LZMA2TEST_H

#include <QtTest>

class Lzma2Test : public QObject
{
    Q_OBJECT

public:
    Lzma2Test();
    virtual ~Lzma2Test();

private:
    static QByteArray readFile(const QString &filename);
    static QSharedPointer<QFile> openFile(const QString &filename);

private slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void encodeWithInvalidParameters();
    void decodeWithInvalidParameters();
private:
    QByteArray license;
    QByteArray encodedLicense;
    uchar      encodeProp;
    qint64     uncompressedSize;
};

#endif // LZMA2TEST_H
