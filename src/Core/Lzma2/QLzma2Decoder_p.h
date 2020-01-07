#ifndef QLZMA2DECODER_P_H
#define QLZMA2DECODER_P_H

#include "../3rdparties/lzma/lzma/Lzma2Dec.h"

#include "QLzma2Decoder.h"

// class QLzma2DecoderPrivate

class QLzma2DecoderPrivate
{
    Q_DECLARE_PUBLIC(QLzma2Decoder)

public:
    QLzma2DecoderPrivate();
    ~QLzma2DecoderPrivate();
protected:
    QLzma2Decoder *q_ptr;
};

// class QLzma2DecoderHandler

class QLzma2DecoderHandler
{
public:
    QLzma2DecoderHandler();
    ~QLzma2DecoderHandler();

public:
    bool init(uchar decProp);
    bool decode(QIODevice *i, QIODevice *o, qint64 uncompressedSize);

private:
    static void *allocFunc(ISzAllocPtr, size_t size);
    static void freeFunc(ISzAllocPtr, void *address);
private:
    static ISzAlloc allocator;
    CLzma2Dec       decHandle;
};

#endif // QLZMA2DECODER_P_H
