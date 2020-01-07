#include "QLogStream.h"
#include "QLogStream_p.h"

#if defined(Q_OS_WIN)
#  include <Windows.h>
#endif

#include <QtEndian>
#include <QDateTime>
#include <QDataStream>

namespace QtLogStreamConstants {
char defaultSign[] = "qtlog.ds.0";
char processSign[] = "qtlog.ps.0";
}

using namespace QtLogStreamConstants;

extern "C" {
#include "../3rdparties/aes/aes_kokke.h"
}

// class QLogStream

QLogStream::QLogStream(QLogStreamPrivate &dd, QIODevice *device)
    : d_ptr(&dd)
{
    d_ptr->q_ptr = this;
    d_ptr->device = device;
}

QLogStream::~QLogStream()
{
}

QIODevice *QLogStream::device() const
{
    Q_D(const QLogStream);

    return d->device;
}

QLogStream::ErrorType QLogStream::error() const
{
    Q_D(const QLogStream);

    return d->error;
}

QLogStream::FormatType QLogStream::format() const
{
    Q_D(const QLogStream);

    return d->format;
}

void QLogStream::setKey(const QByteArray &v)
{
    Q_D(QLogStream);

    d->key = v;
}

void QLogStream::setIv(const QByteArray &v)
{
    Q_D(QLogStream);

    d->iv = v;
}

QByteArray QLogStream::key() const
{
    Q_D(const QLogStream);

    return d->key;
}

QByteArray QLogStream::iv() const
{
    Q_D(const QLogStream);

    return d->iv;
}

// class QLogStreamWriter

QLogStreamWriter::QLogStreamWriter()
    : QLogStream(*new QLogStreamWriterPrivate(), nullptr)
{
    Q_D(QLogStreamWriter);

    d->format = Process;
    d->mode = ReadMode;
}

QLogStreamWriter::QLogStreamWriter(QIODevice *device)
    : QLogStream(*new QLogStreamWriterPrivate(), device)
{
    Q_D(QLogStreamWriter);

    d->fileDevice = qobject_cast<QFileDevice *>(device);
    if (device && device->isWritable()) {
        d->device->write(defaultSign, 10);
        d->format = Default;
    }

    d->mode = StreamMode;
}

QLogStreamWriter::QLogStreamWriter(QIODevice *device, WriterMode mode)
    : QLogStream(*new QLogStreamWriterPrivate(), device)
{
    Q_D(QLogStreamWriter);

    d->fileDevice = qobject_cast<QFileDevice *>(device);
    if (device && device->isWritable()) {

        if (StreamMode == mode) {
            d->format = Default;
            d->device->write(defaultSign, 10);
        } else if (WriteMode == mode) {
            d->format = Process;
            d->device->write(processSign, 10);
        }
    }

    d->mode = mode;
}

QLogStreamWriter::~QLogStreamWriter()
{
}

bool QLogStreamWriter::write(QtMsgType type,
                             const QString &message,
                             const QMessageLogContext &context)
{
    Q_D(QLogStreamWriter);

    if (message.isEmpty()
            || (WriteMode == d->mode)
            || (Invalid == d->format)
            || (NoError != d->error)) {
        return false;
    }

    // reset blocks
    d->from = 0;
    d->blocksDevice->seek(0);
    QDataStream &ds = *d->blocksStream;

    qint64 contextId = qHashBits(&context, sizeof(context));
    if (!d->contextIds.contains(contextId)) {
        // ### Write Context Block ###

        qint16 blockSize = 0;
        qint8  tokenType = ContextToken; // 1
        // ### Context Block Format
        // qint64 processId = 0;         // 8 Process only
        // qint64 contextId = 0;         // 8
        qint16 lineNumber = 0;           // 2
        qint16 fileLength = 0;           // 2
        qint16 functionLength = 0;       // 2
        qint16 categoryLength = 0;       // 2

        if (Process == d->format)
            blockSize = 25;
        else
            blockSize = 17;

        // Count block size

        // line
        lineNumber = context.line;
        // file
        if (context.file) {
            fileLength = qint16(strnlen(context.file, 2048));
            blockSize += fileLength;
        }
        // function
        if (context.function) {
            functionLength = qint16(strnlen(context.function, 2048));
            blockSize += functionLength;
        }
        // category
        if (context.category) {
            categoryLength = qint16(strnlen(context.category, 2048));
            blockSize += categoryLength;
        }

        // Build block data

        ds << blockSize;
        ds << tokenType;

        if (Process == d->format)
            ds << d->processId;
        ds << contextId;

        ds << lineNumber;
        ds << fileLength;
        ds << functionLength;
        ds << categoryLength;

        if (context.file) {
            ds.writeRawData(context.file,
                            fileLength);
        }
        if (context.function) {
            ds.writeRawData(context.function,
                            functionLength);
        }
        if (context.category) {
            ds.writeRawData(context.category,
                            categoryLength);
        }

        d->writeEncrypt(blockSize);

        // Status Test

        if (ds.status() != QDataStream::Ok) {
            d->error = WriteError;
            return false;
        }

        d->contextIds.insert(contextId);
    }

    // ### Write Message Block ###

    QByteArray chunkData;
    if (message.size() <= 4096)
        chunkData = message.toUtf8();
    else
        chunkData = message.left(4096).toUtf8();

    qint16 blockSize = 0;
    qint8  tokenType = MessageToken; // 1
    // ### Message Block Format
    // qint64 processId = 0;         // 8 Process only
    // qint64 contextId = 0;         // 8
    qint64 timestamp = 0;            // 8
    qint8  debugType = type;         // 1
    qint16 chunkSize = 0;            // 2

    if (Process == d->format)
        blockSize = 28;
    else
        blockSize = 20;

    timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();

    // Count block size

    chunkSize = chunkData.size();
    blockSize += chunkSize;

    // Write block data

    ds << blockSize;
    ds << tokenType;

    if (Process == d->format)
        ds << d->processId;
    ds << contextId;

    ds << timestamp;
    ds << debugType;
    ds << chunkSize;

    ds.writeRawData(chunkData.constData(),
                    chunkSize);

    d->writeEncrypt(blockSize);

    // Status Test

    if (ds.status() != QDataStream::Ok) {
        d->error = WriteError;
        return false;
    }

    return d->writeBlocks();
}

bool QLogStreamWriter::writeGracefulEnding(qint64 exitCode)
{
    Q_D(QLogStreamWriter);

    if ((WriteMode != d->mode)
            || (Invalid == d->format)
            || (NoError != d->error)) {
        return false;
    }

    // reset blocks
    d->from = 0;
    d->blocksDevice->seek(0);
    QDataStream &ds = *d->blocksStream;

    // ### Write Endmark Block ###

    qint16 blockSize = 0;
    qint8  tokenType = EndmarkToken; // 1
    // ### Endmark Block Format
    // qint64 processId = 0;         // 8 Process only
    qint64 timestamp = 0;            // 8
    // qint64 exitCode = 0;          // 8

    if (Process == d->format)
        blockSize = 25;
    else
        blockSize = 17;

    timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();

    // Write block data

    ds << blockSize;
    ds << tokenType;

    if (Process == d->format)
        ds << d->processId;
    ds << timestamp;

    ds << exitCode;

    d->writeEncrypt(blockSize);

    // Status Test

    if (ds.status() != QDataStream::Ok) {
        d->error = WriteError;
        return false;
    }

    return d->writeBlocks();
}

const QByteArray &QLogStreamWriter::blockBuffer(qint16 &bufferSize) const
{
    Q_D(const QLogStreamWriter);

    static QByteArray empty;
    if (ReadMode != d->mode)
        return empty;

    bufferSize = d->blocksDevice->pos();

    return d->blocks;
}

bool QLogStreamWriter::writeBlockBuffer(const char *data, int size)
{
    Q_D(QLogStreamWriter);

    if (WriteMode != d->mode)
        return false;

    return d->write(data, size);
}

bool QLogStreamWriter::writeBlockBuffer(const QByteArray &data)
{
    Q_D(QLogStreamWriter);

    if (WriteMode != d->mode)
        return false;

    return d->write(data.constData(), data.size());
}

// class QLogStreamReader

QLogStreamReader::QLogStreamReader(QIODevice *device)
    : QLogStream(*new QLogStreamReaderPrivate(), device)
{
    Q_D(QLogStreamReader);

    if (d->device && d->device->isReadable()) {
        QByteArray signature(10, 0);
        if (d->device->read(signature.data(), 10) == 10) {
            if (signature.compare(defaultSign) == 0) {
                d->format = Default;
            } else if (signature.compare(processSign) == 0) {
                d->format = Process;
            }
        }
    }
}

QLogStreamReader::~QLogStreamReader()
{
}

bool QLogStreamReader::next()
{
    Q_D(QLogStreamReader);

    while (d->error == NoError) {
        qint16 blockSize = 0;
        char *temp = reinterpret_cast<char *>(&blockSize);
        if (d->device->read(temp, 2) != 2) {
            d->error = ReadError;
            return false;
        }

#if Q_LITTLE_ENDIAN == Q_BYTE_ORDER
        blockSize = qFromBigEndian(blockSize);
#endif

        if (d->device->read(d->blocks.data(), blockSize) != blockSize) {
            d->error = ReadError;
            return false;
        }

        // reset blocks
        d->from = 0;
        d->blocksDevice->seek(0);
        QDataStream &ds = *d->blocksStream;

        // decode
        d->readDecrypt(blockSize);
        //

        qint8 tokenType = -1;
        ds >> tokenType;
        if (ContextToken == tokenType) {
            // ### Context Block Format
            qint64 processId = 0;            // 8 Process only
            qint64 contextId = 0;            // 8
            qint16 lineNumber = 0;           // 2
            qint16 fileLength = 0;           // 2
            qint16 functionLength = 0;       // 2
            qint16 categoryLength = 0;       // 2

            // Read block data

            if (Process == d->format)
                ds >> processId;
            ds >> contextId;

            ds >> lineNumber;
            ds >> fileLength;
            ds >> functionLength;
            ds >> categoryLength;

            // file
            QByteArray fileData(fileLength, 0);
            if (ds.readRawData(fileData.data(),
                               fileLength) < 0) {
                d->error = ReadError;
                return false;
            }
            d->contextData << fileData;
            // function
            QByteArray functionData(functionLength, 0);
            if (ds.readRawData(functionData.data(),
                               functionLength) < 0) {
                d->error = ReadError;
                return false;
            }
            d->contextData << functionData;
            // category
            QByteArray categoryData(categoryLength, 0);
            if (ds.readRawData(categoryData.data(),
                               categoryLength) < 0) {
                d->error = ReadError;
                return false;
            }
            d->contextData << categoryData;


            QLogStreamReaderPrivate::Context context(new QMessageLogContext(fileData.constData(),
                                                                            lineNumber,
                                                                            functionData.constData(),
                                                                            categoryData.constData()));
            d->contexts.insert(contextId, context);

        } else if (MessageToken == tokenType) {
            // ### Message Block Format
            qint64 processId = 0;            // 8 Process only
            qint64 contextId = 0;            // 8
            qint64 timestamp = 0;            // 8
            qint8  debugType = 0;            // 1
            qint16 chunkSize = 0;            // 2

            if (Process == d->format)
                ds >> processId;
            ds >> contextId;

            ds >> timestamp;
            ds >> debugType;
            ds >> chunkSize;

            switch (debugType) {
            case QtDebugMsg:
                d->type = QtDebugMsg;
                break;
            case QtInfoMsg:
                d->type = QtInfoMsg;
                break;
            case QtWarningMsg:
                d->type = QtWarningMsg;
                break;
            case QtCriticalMsg:
                d->type = QtCriticalMsg;
                break;
            case QtFatalMsg:
                d->type = QtFatalMsg;
                break;
            }

            // chunk
            QByteArray chunkData(chunkSize, 0);
            if (ds.readRawData(chunkData.data(),
                               chunkSize) < 0) {
                d->error = ReadError;
                return false;
            }

            // Status Test

            if (ds.status() != QDataStream::Ok) {
                d->error = ReadError;
                return false;
            }

            d->message = QString::fromUtf8(chunkData);
            d->context = d->contexts.value(contextId);

            d->processId = processId;
            d->contextId = contextId;
            d->timestamp = timestamp;

            return true;
        } else if (EndmarkToken == tokenType) {
            // ### Endmark Block Format
            qint64 processId = 0;            // 8 Process only
            qint64 timestamp = 0;            // 8
            // qint64 exitCode = 0;          // 8

            if (Process == d->format)
                ds >> processId;
            ds >> timestamp;

            ds >> d->exitCode;

            // Status Test

            if (ds.status() != QDataStream::Ok) {
                d->error = WriteError;
                return false;
            }

            d->processId = processId;
            d->timestamp = timestamp;

            d->isGracefulEnding = true;

            return false;
        } else {
            d->error = FormatError;

            return false;
        }
    }

    return false;
}

QtMsgType QLogStreamReader::type() const
{
    Q_D(const QLogStreamReader);

    return d->type;
}

const QString &QLogStreamReader::message() const
{
    Q_D(const QLogStreamReader);

    return d->message;
}

const QMessageLogContext &QLogStreamReader::context() const
{
    Q_D(const QLogStreamReader);

    static QMessageLogContext nullContext;
    return d->context ? *d->context : nullContext;
}

qint64 QLogStreamReader::processId() const
{
    Q_D(const QLogStreamReader);

    return d->processId;
}

qint64 QLogStreamReader::contextId() const
{
    Q_D(const QLogStreamReader);

    return d->contextId;
}

qint64 QLogStreamReader::timestamp() const
{
    Q_D(const QLogStreamReader);

    return d->timestamp;
}

bool QLogStreamReader::isGracefulEnding() const
{
    Q_D(const QLogStreamReader);

    return d->isGracefulEnding;
}

qint64 QLogStreamReader::exitCode() const
{
    Q_D(const QLogStreamReader);

    return d->exitCode;
}

// class QLogStreamPrivate

QLogStreamPrivate::QLogStreamPrivate()
    : q_ptr (nullptr)
    , error(QLogStream::NoError)
    , format(QLogStream::Invalid)
    , device(nullptr)
    , blocksDevice(nullptr)
    , blocksStream(nullptr)
{
    blocks.resize(16 * 1024);
    blocksDevice = new QBuffer(&blocks);
    blocksStream = new QDataStream(blocksDevice);
    if (blocksDevice->open(QIODevice::ReadWrite))
        blocksStream->setDevice(blocksDevice);
}

QLogStreamPrivate::~QLogStreamPrivate()
{
    if (blocksStream) {
        delete blocksStream;
        blocksStream = nullptr;
    }
    if (blocksDevice) {
        delete blocksDevice;
        blocksDevice = nullptr;
    }
}

// class QLogStreamWriterPrivate

QLogStreamWriterPrivate::QLogStreamWriterPrivate()
    : mode(QLogStreamWriter::ReadMode)
    , fileDevice(nullptr)
    #if defined(Q_OS_WIN)
    , processId(GetCurrentProcessId())
    #else
    , processId(0)
    #endif
{
}

QLogStreamWriterPrivate::~QLogStreamWriterPrivate()
{
}

bool QLogStreamWriterPrivate::write(const char *data, qint64 size)
{
    if (QLogStream::NoError != error) {
        return false;
    } else if (nullptr == device) {
        if (QLogStreamWriter::ReadMode == mode) {
            return true;
        }
        return false;
    }

    const char *temp = reinterpret_cast<const char *>(data);
    if (device->write(temp, size) != size) {
        error = QLogStream::WriteError;
        return false;
    }

    if (fileDevice)
        fileDevice->flush();

    return true;
}

void QLogStreamWriterPrivate::writeEncrypt(qint16 size)
{
    if (key.isEmpty())
        return;

    qint16 padding = (AES_BLOCKLEN - (size % AES_BLOCKLEN));
    char *s = blocks.data() + from + size + 2;
    for (qint16 i = 0; i < padding; ++i) {
        *s = padding; ++s;
    }
    qint16 resultSize = size + padding;

    char *d = blocks.data() + from + 2;
    AES_ctx ctx;
    if(iv.isEmpty()) {
        AES_init_ctx(&ctx, reinterpret_cast<const uint8_t *>(key.constData()));
        for(qint16 i = 0; i < resultSize; i += AES_BLOCKLEN) {
            AES_ctx ctx2 = ctx;
            AES_ECB_encrypt(&ctx2, reinterpret_cast<uint8_t *>(d + i));
        }
    } else {
        AES_init_ctx_iv(&ctx, reinterpret_cast<const uint8_t *>(key.constData()),
                        reinterpret_cast<const uint8_t *>(iv.constData()));
        AES_CBC_encrypt_buffer(&ctx, reinterpret_cast<uint8_t *>(d), resultSize);
    }
    memset(&ctx, 0, sizeof(AES_ctx));

    blocksDevice->seek(from);
    *blocksStream << resultSize;
    from += resultSize + 2;
    blocksDevice->seek(from);
}

bool QLogStreamWriterPrivate::writeBlocks()
{
    return write(blocks.constData(), blocksDevice->pos());
}

// class QLogStreamReaderPrivate

QLogStreamReaderPrivate::QLogStreamReaderPrivate()
    : type(QtDebugMsg)
    , context(nullptr)
    //
    , processId(0)
    , contextId(0)
    , timestamp(0)
    , isGracefulEnding(false)
    , exitCode(-1)
{
}

QLogStreamReaderPrivate::~QLogStreamReaderPrivate()
{
}

void QLogStreamReaderPrivate::readDecrypt(qint16 size)
{
    if (key.isEmpty())
        return;

    char *d = blocks.data();
    AES_ctx ctx;
    if(iv.isNull()) {
        AES_init_ctx(&ctx, reinterpret_cast<const uint8_t *>(key.constData()));
        for(qint16 i = 0; i < size; i += AES_BLOCKLEN) {
            AES_ctx ctx2 = ctx;
            AES_ECB_decrypt(&ctx2, reinterpret_cast<uint8_t *>(d + i));
        }
    } else {
        AES_init_ctx_iv(&ctx, reinterpret_cast<const uint8_t *>(key.constData()),
                        reinterpret_cast<const uint8_t *>(iv.constData()));
        AES_CBC_decrypt_buffer(&ctx, reinterpret_cast<uint8_t *>(d), size);
    }
}
