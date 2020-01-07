#ifndef QLOGSTREAM_H
#define QLOGSTREAM_H

#include "../QtkCoreGlobal.h"

#include <QIODevice>

// class QLogStream

class Q_TKCORE_EXPORT QLogStreamPrivate;
class Q_TKCORE_EXPORT QLogStream
{
    Q_DECLARE_PRIVATE(QLogStream)

protected:
    explicit QLogStream(QLogStreamPrivate &dd, QIODevice *device);
protected:
    QScopedPointer<QLogStreamPrivate> d_ptr;
public:
    virtual ~QLogStream();

public:
    enum ErrorType {
        NoError,
        ReadError,
        WriteError,
        FormatError,
    };

    enum FormatType {
        Invalid,
        Default,
        Process,
    };

    enum TokenType {
        ContextToken,
        MessageToken,
        EndmarkToken,
    };

public:
    QIODevice *device() const;
    ErrorType error() const;
    FormatType format() const;

public:
    void setKey(const QByteArray &v);
    void setIv(const QByteArray &v);
public:
    QByteArray key() const;
    QByteArray iv() const;
};

// class QLogStreamWriter

class Q_TKCORE_EXPORT QLogStreamWriterPrivate;
class Q_TKCORE_EXPORT QLogStreamWriter : public QLogStream
{
    Q_DECLARE_PRIVATE(QLogStreamWriter)

public:
    enum WriterMode { ReadMode, WriteMode, StreamMode };

public:
    explicit QLogStreamWriter();
    explicit QLogStreamWriter(QIODevice *device);
    explicit QLogStreamWriter(QIODevice *device, WriterMode mode);
    virtual ~QLogStreamWriter();

public:
    bool write(QtMsgType type,
               const QString &message,
               const QMessageLogContext &context);

    bool writeGracefulEnding(qint64 exitCode = 0);
    const QByteArray &blockBuffer(qint16 &bufferSize) const;
    bool writeBlockBuffer(const char *data, int size);
    bool writeBlockBuffer(const QByteArray &data);
};

// class QLogStreamReader

class Q_TKCORE_EXPORT QLogStreamReaderPrivate;
class Q_TKCORE_EXPORT QLogStreamReader : public QLogStream
{
    Q_DECLARE_PRIVATE(QLogStreamReader)

public:
    explicit QLogStreamReader(QIODevice *device);
    virtual ~QLogStreamReader();

public:
    bool next();
    QtMsgType type() const;
    const QString &message() const;
    const QMessageLogContext &context() const;

public:
    qint64 processId() const;
    qint64 contextId() const;
    qint64 timestamp() const;
public:
    bool isGracefulEnding() const;
    qint64 exitCode() const;
};

#endif // QLOGSTREAM_H
