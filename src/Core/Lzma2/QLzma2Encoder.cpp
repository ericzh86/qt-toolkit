#include "QLzma2Encoder.h"
#include "QLzma2Encoder_p.h"

#include <QBuffer>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcLzma2Encoder, "QLzma2Encoder")

// class QLzma2EncoderOptions

QLzma2EncoderOptions::QLzma2EncoderOptions() // QLzma2EncoderOptions
    : d_ptr(new QLzma2EncoderOptionsPrivate())
{
}

QLzma2EncoderOptions::QLzma2EncoderOptions(const QLzma2EncoderOptions &other)
    : d_ptr(other.d_ptr)
{
}

QLzma2EncoderOptions &QLzma2EncoderOptions::operator=(const QLzma2EncoderOptions &rhs)
{
    if (this != &rhs)
        d_ptr.operator=(rhs.d_ptr);
    return *this;
}

QLzma2EncoderOptions::~QLzma2EncoderOptions() // QLzma2EncoderOptions
{
}

void QLzma2EncoderOptions::setCompressionLevel(int level)
{
    if ((level < 0) || (level > 9)) {
        if (level == -1) {
            d_ptr->props.lzmaProps.level = 5;
        }
    } else {
        d_ptr->props.lzmaProps.level = level;
    }
}

int QLzma2EncoderOptions::compressionLevel() const
{
    return d_ptr->props.lzmaProps.level;
}

// class QLzma2EncoderOptionsPrivate

QLzma2EncoderOptionsPrivate::QLzma2EncoderOptionsPrivate()
{
    Lzma2EncProps_Init(&props);
}

QLzma2EncoderOptionsPrivate::~QLzma2EncoderOptionsPrivate()
{
}

// class QLzma2Encoder

QLzma2Encoder::QLzma2Encoder(QObject *parent)
    : QObject(parent)
    , d_ptr(new QLzma2EncoderPrivate())
{
    d_ptr->q_ptr = this;
}

QLzma2Encoder::~QLzma2Encoder()
{
}

bool QLzma2Encoder::blockingEncode(QIODevice *in, QIODevice *out, uchar &prop)
{
    QLzma2EncoderOptions options;

    return QLzma2Encoder::blockingEncode(in, out, prop, options);
}

bool QLzma2Encoder::blockingEncode(const QByteArray &in, QByteArray &out, uchar &prop)
{
    if (in.isEmpty()) {
        qWarning(lcLzma2Encoder, "empty input");
        return false;
    }
    if (!out.isNull()) {
        QByteArray clean = std::move(out);
        Q_UNUSED(clean);
    }

    QByteArray *in2 = const_cast<QByteArray *>(&in);
    QScopedPointer<QBuffer> ibuf(new QBuffer(in2));
    QScopedPointer<QBuffer> obuf(new QBuffer(&out));

    if (!ibuf->open(QIODevice::ReadOnly)) {
        qWarning(lcLzma2Encoder, "failed to open input buffer");
        return false;
    }
    if (!obuf->open(QIODevice::WriteOnly)) {
        qWarning(lcLzma2Encoder, "failed to open output buffer");
        return false;
    }

    QLzma2EncoderOptions options;

    return QLzma2Encoder::blockingEncode(ibuf.data(), obuf.data(), prop, options);
}

QByteArray QLzma2Encoder::blockingEncode(const QByteArray &in, uchar &prop)
{
    if (in.isEmpty()) {
        qWarning(lcLzma2Encoder, "empty input");
        return QByteArray();
    }

    QByteArray out;
    QLzma2EncoderOptions options;

    return QLzma2Encoder::blockingEncode(in, out, prop, options) ? out : QByteArray();
}

bool QLzma2Encoder::blockingEncode2(QByteArray &io, uchar &prop)
{
    if (io.isEmpty()) {
        qWarning(lcLzma2Encoder, "empty input");
        return false;
    }

    QLzma2EncoderOptions options;
    QByteArray in = std::move(io);

    if (!QLzma2Encoder::blockingEncode(in, io, prop, options)) {
        in.swap(io);
        return false;
    }

    return true;
}

bool QLzma2Encoder::blockingEncode(QIODevice *in, QIODevice *out, uchar &prop, const QLzma2EncoderOptions &options)
{
    if (!in) {
        qWarning(lcLzma2Encoder, "null input device");
        return false;
    }
    if (!out) {
        qWarning(lcLzma2Encoder, "null output device");
        return false;
    }
    if (!in->isReadable()) {
        qWarning(lcLzma2Encoder, "unreadable input device");
        return false;
    }
    if (!out->isWritable()) {
        qWarning(lcLzma2Encoder, "unwritable output device");
        return false;
    }

    char value = 0;
    if (in->peek(&value, 1) != 1) {
        qWarning(lcLzma2Encoder, "null input device");
        return false;
    }

    QLzma2EncoderHandler handler;
    if (!handler.init(options.d_ptr->props, prop)) {
        return false;
    }

    return handler.encode(in, out);
}

bool QLzma2Encoder::blockingEncode(const QByteArray &in, QByteArray &out, uchar &prop, const QLzma2EncoderOptions &options)
{
    QByteArray *in2 = const_cast<QByteArray *>(&in);
    QScopedPointer<QBuffer> ibuf(new QBuffer(in2));
    QScopedPointer<QBuffer> obuf(new QBuffer(&out));

    if (!ibuf->open(QIODevice::ReadOnly)) {
        return false;
    }
    if (!obuf->open(QIODevice::WriteOnly)) {
        return false;
    }

    return QLzma2Encoder::blockingEncode(ibuf.data(), obuf.data(), prop, options);
}

QByteArray QLzma2Encoder::blockingEncode(const QByteArray &in, uchar &prop, const QLzma2EncoderOptions &options)
{
    QByteArray out;

    return QLzma2Encoder::blockingEncode(in, out, prop, options) ? out : QByteArray();
}

bool QLzma2Encoder::blockingEncode2(QByteArray &io, uchar &prop, const QLzma2EncoderOptions &options)
{
    QByteArray in = std::move(io);

    if (!QLzma2Encoder::blockingEncode(in, io, prop, options)) {
        in.swap(io);
        return false;
    }

    return true;
}

// class QLzma2EncoderPrivate

QLzma2EncoderPrivate::QLzma2EncoderPrivate()
    : q_ptr(nullptr)
{
}

QLzma2EncoderPrivate::~QLzma2EncoderPrivate()
{
}

// class QLzma2EncoderHandler

ISzAlloc QLzma2EncoderHandler::allocator{&QLzma2EncoderHandler::allocFunc, &QLzma2EncoderHandler::freeFunc};

QLzma2EncoderHandler::QLzma2EncoderHandler()
    : encHandle(nullptr)
{
    encHandle = Lzma2Enc_Create(&QLzma2EncoderHandler::allocator, &QLzma2EncoderHandler::allocator);
}

QLzma2EncoderHandler::~QLzma2EncoderHandler()
{
    Lzma2Enc_Destroy(encHandle);
}

bool QLzma2EncoderHandler::init(const CLzma2EncProps &encProps, uchar &decProp)
{
    SRes res = Lzma2Enc_SetProps(encHandle, &encProps);
    if (SZ_OK != res) {
        qWarning(lcLzma2Encoder, "failed to initialize encoder: %d", res);
        return false;
    }

    decProp = Lzma2Enc_WriteProperties(encHandle);

    return true;
}

bool QLzma2EncoderHandler::encode(QIODevice *i, QIODevice *o)
{
    QLzma2EncoderInStream istr(i);
    QLzma2EncoderOutStream ostr(o);

    SRes res = Lzma2Enc_Encode2(encHandle,
                                &ostr.s, nullptr, 0,
                                &istr.s, nullptr, 0,
                                nullptr);
    if (SZ_OK != res) {
        qWarning(lcLzma2Encoder, "failed to encode data: %d", res);
        return false;
    }

    return true;
}

void *QLzma2EncoderHandler::allocFunc(ISzAllocPtr, size_t size)
{
    return size > 0 ? malloc(size) : nullptr;
}

void QLzma2EncoderHandler::freeFunc(ISzAllocPtr, void *address)
{
    free(address);
}

// class QLzma2EncoderInStream

QLzma2EncoderInStream::QLzma2EncoderInStream(QIODevice *device)
    : d(device)
{
    Q_CHECK_PTR(device);
    Q_ASSERT(device->isReadable());
    s.Read = &QLzma2EncoderInStream::read;
}

SRes QLzma2EncoderInStream::read(const ISeqInStream *p, void *rbuf, size_t *size)
{
    const QLzma2EncoderInStream *io = reinterpret_cast<const QLzma2EncoderInStream *>(p);

    int bytes = io->d->read(static_cast<char *>(rbuf), *size);
    if (bytes >= 0) {
        *size = bytes;
        return SZ_OK;
    }

    qWarning(lcLzma2Encoder, "failed to read data");

    return SZ_ERROR_READ;
}

// QLzma2EncoderOutStream

QLzma2EncoderOutStream::QLzma2EncoderOutStream(QIODevice *device)
    : d(device)
{
    Q_CHECK_PTR(device);
    Q_ASSERT(device->isWritable());
    s.Write = &QLzma2EncoderOutStream::write;
}

size_t QLzma2EncoderOutStream::write(const ISeqOutStream *p, const void *wbuf, size_t size)
{
    const QLzma2EncoderOutStream *io = reinterpret_cast<const QLzma2EncoderOutStream *>(p);

    qint64 actualBytes = io->d->write(static_cast<const char *>(wbuf), size);
    if (actualBytes != size) {
        qWarning(lcLzma2Encoder, "failed to write data");
    }

    return actualBytes;
}
