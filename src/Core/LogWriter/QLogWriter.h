#ifndef QLOGWRITER_H
#define QLOGWRITER_H

#include "../QtkCoreGlobal.h"

#include <QObject>

typedef void (* QMessageHandler)(QtMsgType type,
                                 const QMessageLogContext &context,
                                 const QString &message);

class QTK_CORE_EXPORT QLogWriterPrivate;
class QTK_CORE_EXPORT QLogWriter : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QLogWriter)

public:
    QLogWriter(const QString &dataPath, QObject *parent = nullptr);
protected:
    QScopedPointer<QLogWriterPrivate> d_ptr;
public:
    virtual ~QLogWriter();

public:
    static void initKey(const QByteArray &key);
    static void initIv(const QByteArray &iv);

public:
    static void writeBlockBuffer(const QByteArray &data);
    static void writeBlockBuffer(const char *data, int size);
    static void writeGracefulEnding(qint64 exitCode = 0);

    static void writePlainBuffer(const char *data, int size);

public:
    static void initMessageHandler(QMessageHandler handler);
};

#endif // QLOGWRITER_H
