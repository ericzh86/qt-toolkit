#ifndef QTLZMA2TEST_H
#define QTLZMA2TEST_H

#include <QtTest>

class QtLzma2 : public QObject
{
    Q_OBJECT

public:
    QtLzma2();
    virtual ~QtLzma2();

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

#endif // QTLZMA2TEST_H
