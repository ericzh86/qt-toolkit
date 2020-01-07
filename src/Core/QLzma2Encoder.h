#ifndef QLZMA2ENCODER_H
#define QLZMA2ENCODER_H

#include <QIODevice>
#include <QSharedDataPointer>
#include "QtkCoreGlobal.h"

// class QLzma2EncoderOptions

class QTK_CORE_EXPORT QLzma2EncoderOptionsPrivate;
class QTK_CORE_EXPORT QLzma2EncoderOptions
{
public:
    QLzma2EncoderOptions(); // QLzma2EncoderOptions
    QLzma2EncoderOptions(const QLzma2EncoderOptions &other);
    QLzma2EncoderOptions &operator=(const QLzma2EncoderOptions &rhs);
    ~QLzma2EncoderOptions(); // QLzma2EncoderOptions

public:
    void setCompressionLevel(int level);
    int compressionLevel() const;

private:
    QSharedDataPointer<QLzma2EncoderOptionsPrivate> d_ptr;

    friend class QLzma2Encoder;
};

// class QLzma2Encoder

class QTK_CORE_EXPORT QLzma2EncoderPrivate;
class QTK_CORE_EXPORT QLzma2Encoder : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QLzma2Encoder)

public:
    explicit QLzma2Encoder(QObject *parent = nullptr);
    virtual ~QLzma2Encoder();
protected:
    QScopedPointer<QLzma2EncoderPrivate> d_ptr;

public:
    static bool blockingEncode(QIODevice *in, QIODevice *out, uchar &prop);
    static bool blockingEncode(const QByteArray &in, QByteArray &out, uchar &prop);
    static QByteArray blockingEncode(const QByteArray &in, uchar &prop);
    static bool blockingEncode2(QByteArray &io, uchar &prop);
public:
    static bool blockingEncode(QIODevice *in, QIODevice *out, uchar &prop, const QLzma2EncoderOptions &options);
    static bool blockingEncode(const QByteArray &in, QByteArray &out, uchar &prop, const QLzma2EncoderOptions &options);
    static QByteArray blockingEncode(const QByteArray &in, uchar &prop, const QLzma2EncoderOptions &options);
    static bool blockingEncode2(QByteArray &io, uchar &prop, const QLzma2EncoderOptions &options);
};

#endif // QLZMA2ENCODER_H
