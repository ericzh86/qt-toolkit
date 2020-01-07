#include "QLzma2Decoder.h"
#include "QLzma2Decoder_p.h"

#include <QBuffer>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcLzma2Decoder, "QLzma2Decoder")

// class QLzma2Decoder

QLzma2Decoder::QLzma2Decoder(QObject *parent)
    : QObject(parent)
    , d_ptr(new QLzma2DecoderPrivate())
{
    d_ptr->q_ptr = this;
}

QLzma2Decoder::~QLzma2Decoder()
{
}

bool QLzma2Decoder::blockingDecode(QIODevice *in, QIODevice *out, uchar prop)
{
    return QLzma2Decoder::blockingDecode(in, out, prop, -1);
}

bool QLzma2Decoder::blockingDecode(const QByteArray &in, QByteArray &out, uchar prop)
{
    if (in.isEmpty()) {
        qWarning(lcLzma2Decoder, "empty input");
        return false;
    }
    if (!out.isNull()) {
        QByteArray clean = std::move(out);
        Q_UNUSED(clean);
    }

    return QLzma2Decoder::blockingDecode(in, out, prop, -1);
}

QByteArray QLzma2Decoder::blockingDecode(const QByteArray &in, uchar prop)
{
    if (in.isEmpty()) {
        qWarning(lcLzma2Decoder, "empty input");
        return QByteArray();
    }

    QByteArray out;

    return QLzma2Decoder::blockingDecode(in, out, prop, -1) ? out : QByteArray();
}

bool QLzma2Decoder::blockingDecode2(QByteArray &io, uchar prop)
{
    if (io.isEmpty()) {
        qWarning(lcLzma2Decoder, "empty input");
        return false;
    }

    QByteArray in = std::move(io);

    if (!QLzma2Decoder::blockingDecode(in, io, prop, -1)) {
        in.swap(io);
        return false;
    }

    return true;
}

bool QLzma2Decoder::blockingDecode(QIODevice *in, QIODevice *out, uchar prop, qint64 uncompressedSize)
{
    if (!in) {
        qWarning(lcLzma2Decoder, "null input device");
        return false;
    }
    if (!out) {
        qWarning(lcLzma2Decoder, "null output device");
        return false;
    }
    if (!in->isReadable()) {
        qWarning(lcLzma2Decoder, "unreadable input device");
        return false;
    }
    if (!out->isWritable()) {
        qWarning(lcLzma2Decoder, "unwritable output device");
        return false;
    }

    char value = 0;
    if (in->peek(&value, 1) != 1) {
        qWarning(lcLzma2Decoder, "null input device");
        return false;
    }

    QLzma2DecoderHandler handler;
    if (!handler.init(prop)) {
        return false;
    }

    return handler.decode(in, out, uncompressedSize);
}

bool QLzma2Decoder::blockingDecode(const QByteArray &in, QByteArray &out, uchar prop, qint64 uncompressedSize)
{
    QByteArray *in2 = const_cast<QByteArray *>(&in);
    QScopedPointer<QBuffer> ibuf(new QBuffer(in2));
    QScopedPointer<QBuffer> obuf(new QBuffer(&out));

    if (!ibuf->open(QIODevice::ReadOnly)) {
        qWarning(lcLzma2Decoder, "failed to open input buffer");
        return false;
    }
    if (!obuf->open(QIODevice::WriteOnly)) {
        qWarning(lcLzma2Decoder, "failed to open output buffer");
        return false;
    }

    return QLzma2Decoder::blockingDecode(ibuf.data(), obuf.data(), prop, uncompressedSize);
}

QByteArray QLzma2Decoder::blockingDecode(const QByteArray &in, uchar prop, qint64 uncompressedSize)
{
    QByteArray out;

    return QLzma2Decoder::blockingDecode(in, out, prop, uncompressedSize) ? out : QByteArray();
}

bool QLzma2Decoder::blockingDecode2(QByteArray &io, uchar prop, qint64 uncompressedSize)
{
    QByteArray in = std::move(io);

    if (!QLzma2Decoder::blockingDecode(in, io, prop, uncompressedSize)) {
        in.swap(io);
        return false;
    }

    return true;
}

// class QLzma2DecoderPrivate

QLzma2DecoderPrivate::QLzma2DecoderPrivate()
    :q_ptr(nullptr)
{
}

QLzma2DecoderPrivate::~QLzma2DecoderPrivate()
{
}

// class QLzma2DecoderHandler

ISzAlloc QLzma2DecoderHandler::allocator{&QLzma2DecoderHandler::allocFunc, &QLzma2DecoderHandler::freeFunc};

QLzma2DecoderHandler::QLzma2DecoderHandler()
{
    Lzma2Dec_Construct(&decHandle);
}

QLzma2DecoderHandler::~QLzma2DecoderHandler()
{
    Lzma2Dec_Free(&decHandle, &QLzma2DecoderHandler::allocator);
}

bool QLzma2DecoderHandler::init(uchar decProp)
{
    SRes res = Lzma2Dec_Allocate(&decHandle, decProp, &QLzma2DecoderHandler::allocator);
    if (SZ_OK != res) {
        qWarning(lcLzma2Decoder, "failed to initialize decoder: %d", res);
        return false;
    }

    Lzma2Dec_Init(&decHandle);

    return true;
}

bool QLzma2DecoderHandler::decode(QIODevice *i, QIODevice *o, qint64 uncompressedSize)
{
    qint64 bytes = uncompressedSize;
    bool hasBytes = (bytes > 0);
    QByteArray ibuf(64 * 1024, 0);
    QByteArray obuf(64 * 1024, 0);

    qint64 ipos = 0;
    qint64 isize = 0;
    qint64 opos = 0;

    do {
        if (ipos == isize) {
            isize = i->read(ibuf.data(), ibuf.size());
            if (isize < 0) {
                qWarning(lcLzma2Decoder, "failed to read data");
                return false;
            }

            ipos = 0;
        }

        SizeT iproc = isize - ipos;
        SizeT oproc = obuf.size() - opos;
        ELzmaFinishMode mode = LZMA_FINISH_ANY;
        if (hasBytes && (oproc > bytes)) {
            mode = LZMA_FINISH_END;
            oproc = bytes;
        }

        ELzmaStatus status;
        SRes res = Lzma2Dec_DecodeToBuf(&decHandle,
                                        reinterpret_cast<Byte *>(obuf.data() + opos), &oproc,
                                        reinterpret_cast<const Byte *>(ibuf.constData() + ipos), &iproc, mode, &status);
        if (SZ_OK != res) {
            qWarning(lcLzma2Decoder, "failed to decode data: %d", res);
            return false;
        }

        ipos += iproc;
        bytes -= oproc;
        opos += oproc;

        if (o->write(obuf.constData(), opos) != opos) {
            qWarning(lcLzma2Decoder, "failed to write data");
            return false;
        }
        opos = 0;

        if (hasBytes && (bytes == 0)) {
            break;
        }

        if ((iproc == 0) && (oproc == 0)) {
            if (hasBytes || (LZMA_STATUS_FINISHED_WITH_MARK != status)) {
                qWarning(lcLzma2Decoder, "error status");
                return false;
            }
            break;
        }
    } while (true);

    return true;
}

void *QLzma2DecoderHandler::allocFunc(ISzAllocPtr, size_t size)
{
    return size > 0 ? malloc(size) : nullptr;
}

void QLzma2DecoderHandler::freeFunc(ISzAllocPtr, void *address)
{
    free(address);
}
