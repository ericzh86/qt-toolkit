#ifndef QLZMA2ENCODER_P_H
#define QLZMA2ENCODER_P_H

#include "3rdparties/lzma/lzma/Lzma2Enc.h"

#include "QLzma2Encoder.h"

// class QLzma2EncoderOptionsPrivate

class QLzma2EncoderOptionsPrivate : public QSharedData
{
public:
    QLzma2EncoderOptionsPrivate();
    ~QLzma2EncoderOptionsPrivate();

public:
    CLzma2EncProps props;
};

// class QLzma2EncoderPrivate

class QLzma2EncoderPrivate
{
    Q_DECLARE_PUBLIC(QLzma2Encoder)

public:
    QLzma2EncoderPrivate();
    ~QLzma2EncoderPrivate();
protected:
    QLzma2Encoder *q_ptr;
};

// class QLzma2EncoderHandler

class QLzma2EncoderHandler
{
public:
    QLzma2EncoderHandler();
    ~QLzma2EncoderHandler();

public:
    bool init(const CLzma2EncProps &encProps, uchar &decProp);
    bool encode(QIODevice *i, QIODevice *o);

private:
    static void *allocFunc(ISzAllocPtr, size_t size);
    static void freeFunc(ISzAllocPtr, void *address);
private:
    static ISzAlloc allocator;
    CLzma2EncHandle encHandle;
};

// class QLzma2EncoderInStream

class QLzma2EncoderInStream
{
public:
    explicit QLzma2EncoderInStream(QIODevice *device);

private:
    friend class QLzma2EncoderHandler;
    static SRes read(const ISeqInStream *p,
                     void *rbuf, size_t *size);
private:
    ISeqInStream s;
    QIODevice   *d;

};

// class QLzma2EncoderOutStream

class QLzma2EncoderOutStream
{
public:
    explicit QLzma2EncoderOutStream(QIODevice *device);

private:
    friend class QLzma2EncoderHandler;
    static size_t write(const ISeqOutStream *p,
                        const void *wbuf, size_t size);
private:
    ISeqOutStream s;
    QIODevice    *d;
};

#endif // QLZMA2ENCODER_P_H
