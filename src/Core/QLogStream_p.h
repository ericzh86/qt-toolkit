#ifndef QLOGSTREAM_P_H
#define QLOGSTREAM_P_H

#include <QSet>
#include <QVector>
#include <QBuffer>
#include <QFileDevice>
#include <QSharedPointer>

#include "QLogStream.h"

class QLogStreamPrivate
{
    Q_DECLARE_PUBLIC(QLogStream)

public:
    QLogStreamPrivate();
    virtual ~QLogStreamPrivate();
protected:
    QLogStream *q_ptr;

protected:
    QLogStream::ErrorType  error;
    QLogStream::FormatType format;

protected:
    QIODevice   *device;

protected:
    QByteArray   blocks;
    QBuffer     *blocksDevice;
    QDataStream *blocksStream;

    qint64 from;

protected:
    QByteArray key;
    QByteArray iv;

private:
    friend class QLogStreamWriter;
    friend class QLogStreamReader;
};

class QLogStreamWriterPrivate : public QLogStreamPrivate
{
    Q_DECLARE_PUBLIC(QLogStreamWriter)

public:
    QLogStreamWriterPrivate();
    virtual ~QLogStreamWriterPrivate();

protected:
    QLogStreamWriter::WriterMode mode;

protected:
    bool write(const char *data, qint64 size);
    void writeEncrypt(qint16 size);
    bool writeBlocks();
protected:
    QSet<qint64> contextIds;
    QFileDevice *fileDevice;

protected:
    qint64 processId;
};

class QLogStreamReaderPrivate : public QLogStreamPrivate
{
    Q_DECLARE_PUBLIC(QLogStreamReader)

public:
    QLogStreamReaderPrivate();
    virtual ~QLogStreamReaderPrivate();

protected:
    typedef QSharedPointer<QMessageLogContext> Context;
    QVector<QByteArray> contextData;
    QHash<qint64, Context> contexts;

protected:
    void readDecrypt(qint16 size);

protected:
    QtMsgType type;
    QString message;
    Context context;

protected:
    qint64 processId;
    qint64 contextId;
    qint64 timestamp;

protected:
    bool isGracefulEnding;
    qint64 exitCode;
};

#endif // QLOGSTREAM_P_H
