#include "Lzma2Test.h"

#include <QBuffer>

#include "QLzma2Encoder.h"
#include "QLzma2Decoder.h"

Lzma2Test::Lzma2Test()
{
}

Lzma2Test::~Lzma2Test()
{
}

QByteArray Lzma2Test::readFile(const QString &filename)
{
    QFile file(filename);
    if (file.open(QFile::ReadOnly)) {
        return file.readAll();
    }

    return QByteArray();
}

QSharedPointer<QFile> Lzma2Test::openFile(const QString &filename)
{
    QSharedPointer<QFile> file(new QFile(filename));
    if (file->open(QFile::ReadWrite)) {
        return file;
    }

    return nullptr;
}

void Lzma2Test::initTestCase()
{
    license = readFile(":/testfiles/LICENSE");
    QLzma2Encoder::blockingEncode(license, encodedLicense, encodeProp);
    uncompressedSize = license.size();
}

void Lzma2Test::cleanupTestCase()
{
}

void Lzma2Test::encodeWithInvalidParameters()
{
    // static bool decode(QIODevice *in, QIODevice *out, uchar prop);

    do {
        uchar prop = 0;

        QVERIFY(QLzma2Encoder::blockingEncode(nullptr, nullptr, prop) != true);
    } while (false);

    do {
        QByteArray i;
        uchar prop = 0;

        QScopedPointer<QBuffer> ibuf(new QBuffer(&i));

        QVERIFY(QLzma2Encoder::blockingEncode(ibuf.data(), nullptr, prop) != true);
    } while (false);

    do {
        QByteArray o;
        uchar prop = 0;

        QScopedPointer<QBuffer> obuf(new QBuffer(&o));

        QVERIFY(QLzma2Encoder::blockingEncode(nullptr, obuf.data(), prop) != true);
    } while (false);

    do {
        QByteArray i;
        QByteArray o;
        uchar prop = 0;

        QScopedPointer<QBuffer> ibuf(new QBuffer(&i));
        QScopedPointer<QBuffer> obuf(new QBuffer(&o));

        QVERIFY(QLzma2Encoder::blockingEncode(ibuf.data(), obuf.data(), prop) != true);
    } while (false);

    do {
        QByteArray i;
        QByteArray o;
        uchar prop = 0;

        QScopedPointer<QBuffer> ibuf(new QBuffer(&i));
        QScopedPointer<QBuffer> obuf(new QBuffer(&o));

        ibuf->open(QBuffer::ReadWrite);

        QVERIFY(QLzma2Encoder::blockingEncode(ibuf.data(), obuf.data(), prop) != true);
    } while (false);

    do {
        QByteArray i;
        QByteArray o;
        uchar prop = 0;

        QScopedPointer<QBuffer> ibuf(new QBuffer(&i));
        QScopedPointer<QBuffer> obuf(new QBuffer(&o));

        obuf->open(QBuffer::ReadWrite);

        QVERIFY(QLzma2Encoder::blockingEncode(ibuf.data(), obuf.data(), prop) != true);
    } while (false);

    do {
        QByteArray i;
        QByteArray o;
        uchar prop = 0;

        QScopedPointer<QBuffer> ibuf(new QBuffer(&i));
        QScopedPointer<QBuffer> obuf(new QBuffer(&o));

        ibuf->open(QBuffer::ReadWrite);
        obuf->open(QBuffer::ReadWrite);

        QVERIFY(QLzma2Encoder::blockingEncode(ibuf.data(), obuf.data(), prop) != true);
        QCOMPARE(o.size(), 0);
        QVERIFY(i == o);
    } while (false);

    // static bool encode(const QByteArray &in, QByteArray &out, uchar &prop);

    do {
        QByteArray i;
        QByteArray o;
        uchar prop = 0;

        QVERIFY(QLzma2Encoder::blockingEncode(i, o, prop) != true);
    } while (false);

    do {
        QByteArray i;
        QByteArray o;
        uchar prop = 0;

        i = o = license;

        QVERIFY(QLzma2Encoder::blockingEncode(i, o, prop) == true);
        QVERIFY(o.isEmpty() != true);
        QVERIFY(i != o);
    } while (false);

    do {
        QByteArray i;
        QByteArray o;
        uchar prop = 0;

        i = license;

        QVERIFY(QLzma2Encoder::blockingEncode(i, o, prop) == true);
        QVERIFY(o.isEmpty() != true);
    } while (false);

    // static QByteArray encode(const QByteArray &in, uchar &prop);

    do {
        QByteArray i;
        uchar prop = 0;

        QByteArray o = QLzma2Encoder::blockingEncode(i, prop);
        QCOMPARE(o.size(), 0);
        QVERIFY(i == o);
    } while (false);

    do {
        QByteArray i;
        uchar prop = 0;

        i = license;

        QByteArray o = QLzma2Encoder::blockingEncode(i, prop);
        QVERIFY(o.isEmpty() != true);
        QVERIFY(i != o);
    } while (false);

    // static bool encode2(QByteArray &io, uchar &prop);

    do {
        QByteArray i;
        uchar prop = 0;

        QVERIFY(QLzma2Encoder::blockingEncode2(i, prop) != true);
        QVERIFY(i.isEmpty() == true);
    } while (false);

    do {
        QByteArray i;
        QByteArray o;
        uchar prop = 0;

        i = o = license;

        QVERIFY(QLzma2Encoder::blockingEncode2(o, prop) == true);
        QVERIFY(o.isEmpty() != true);
        QVERIFY(i != o);
    } while (false);
}

void Lzma2Test::decodeWithInvalidParameters()
{
    // static bool decode(QIODevice *in, QIODevice *out, uchar prop);

    do {
        QVERIFY(QLzma2Decoder::blockingDecode(nullptr, nullptr, encodeProp) != true);
    } while (false);

    do {
        QByteArray i;

        QScopedPointer<QBuffer> ibuf(new QBuffer(&i));

        QVERIFY(QLzma2Decoder::blockingDecode(ibuf.data(), nullptr, encodeProp) != true);
        QCOMPARE(i.size(), 0);
    } while (false);

    do {
        QByteArray o;

        QScopedPointer<QBuffer> obuf(new QBuffer(&o));

        QVERIFY(QLzma2Decoder::blockingDecode(nullptr, obuf.data(), encodeProp) != true);
        QCOMPARE(o.size(), 0);
    } while (false);

    do {
        QByteArray i;
        QByteArray o;

        QScopedPointer<QBuffer> ibuf(new QBuffer(&i));
        QScopedPointer<QBuffer> obuf(new QBuffer(&o));

        QVERIFY(QLzma2Decoder::blockingDecode(ibuf.data(), obuf.data(), encodeProp) != true);
        QCOMPARE(i.size(), 0);
        QCOMPARE(o.size(), 0);
    } while (false);

    do {
        QByteArray i;
        QByteArray o;

        QScopedPointer<QBuffer> ibuf(new QBuffer(&i));
        QScopedPointer<QBuffer> obuf(new QBuffer(&o));

        ibuf->open(QBuffer::ReadWrite);

        QVERIFY(QLzma2Decoder::blockingDecode(ibuf.data(), obuf.data(), encodeProp) != true);
        QCOMPARE(i.size(), 0);
        QCOMPARE(o.size(), 0);
    } while (false);

    do {
        QByteArray i;
        QByteArray o;

        QScopedPointer<QBuffer> ibuf(new QBuffer(&i));
        QScopedPointer<QBuffer> obuf(new QBuffer(&o));

        obuf->open(QBuffer::ReadWrite);

        QVERIFY(QLzma2Decoder::blockingDecode(ibuf.data(), obuf.data(), encodeProp) != true);
        QCOMPARE(i.size(), 0);
        QCOMPARE(o.size(), 0);
    } while (false);

    do {
        QByteArray i;
        QByteArray o;

        QScopedPointer<QBuffer> ibuf(new QBuffer(&i));
        QScopedPointer<QBuffer> obuf(new QBuffer(&o));

        ibuf->open(QBuffer::ReadWrite);
        obuf->open(QBuffer::ReadWrite);

        QVERIFY(QLzma2Decoder::blockingDecode(ibuf.data(), obuf.data(), encodeProp) != true);
        QCOMPARE(i.size(), 0);
        QCOMPARE(o.size(), 0);
    } while (false);

    // static bool decode(const QByteArray &in, QByteArray &out, uchar prop);

    do {
        QByteArray o;
        QVERIFY(QLzma2Decoder::blockingDecode(QByteArray(), o, encodeProp) != true);
        QCOMPARE(o.size(), 0);
    } while (false);

    do {
        QByteArray o;
        QVERIFY(QLzma2Decoder::blockingDecode(license, o, encodeProp) != true);
        QCOMPARE(o.size(), 0);
    } while (false);

    do {
        QByteArray o = encodedLicense;
        QVERIFY(QLzma2Decoder::blockingDecode(license, o, encodeProp) != true);
        QCOMPARE(o.size(), 0);
    } while (false);

    do {
        QByteArray o;
        QVERIFY(QLzma2Decoder::blockingDecode(encodedLicense, o, encodeProp) == true);
        QVERIFY(o == license);
    } while (false);

    do {
        QByteArray o = encodedLicense;
        QVERIFY(QLzma2Decoder::blockingDecode(encodedLicense, o, encodeProp) == true);
        QVERIFY(o == license);
    } while (false);

    do {
        QByteArray o;

        QVERIFY(QLzma2Decoder::blockingDecode(encodedLicense, o, 64) != true);
        QCOMPARE(o.size(), 0);
    } while (false);

    // static QByteArray decode(const QByteArray &in, uchar prop);

    do {
        QByteArray o = QLzma2Decoder::blockingDecode(QByteArray(), encodeProp);
        QCOMPARE(o.size(), 0);
    } while (false);

    do {
        QByteArray o = QLzma2Decoder::blockingDecode(license, encodeProp);
        QCOMPARE(o.size(), 0);
    } while (false);

    do {
        QByteArray o = QLzma2Decoder::blockingDecode(encodedLicense, encodeProp);
        QVERIFY(o == license);
    } while (false);

    do {
        QByteArray o = QLzma2Decoder::blockingDecode(encodedLicense, 64);
        QCOMPARE(o.size(), 0);
    } while (false);

    // static bool decode2(QByteArray &io, uchar prop);

    do {
        QByteArray i;

        QVERIFY(QLzma2Decoder::blockingDecode2(i, encodeProp) != true);
        QCOMPARE(i.size(), 0);
    } while (false);

    do {
        QByteArray i = license;
        QVERIFY(QLzma2Decoder::blockingDecode2(i, encodeProp) != true);
        QVERIFY(i == license);
    } while (false);

    do {
        QByteArray i = encodedLicense;
        QVERIFY(QLzma2Decoder::blockingDecode2(i, encodeProp) == true);
        QVERIFY(i == license);
    } while (false);

    do {
        QByteArray i = encodedLicense;
        QVERIFY(QLzma2Decoder::blockingDecode2(i, 64) != true);
        QVERIFY(i == encodedLicense);
    } while (false);
}

QTEST_APPLESS_MAIN(Lzma2Test)
