#ifndef QLOGWRITER_P_H
#define QLOGWRITER_P_H

#include <QMutex>
#include <QSharedPointer>

#include "QLogWriter.h"
#include "../LogStream/QLogStream.h"

class OlSchedulerHost;
class OlSchedulerNode;

class QLogWriterPrivate
{
    Q_DECLARE_PUBLIC(QLogWriter)

public:
    QLogWriterPrivate();
    virtual ~QLogWriterPrivate();
protected:
    QLogWriter *q_ptr;

public:
    static QLogWriterPrivate *instance;

protected:
    static QByteArray key;
    static QByteArray iv;
public:
    bool isDeveloperMode;

public:
    QSharedPointer<QLogStreamWriter> encodeWriter;
    QSharedPointer<QLogStreamWriter> streamWriter;

public:
    QByteArray plainMessage(QtMsgType type,
                            const QMessageLogContext &context,
                            const QString &message);

public:
    static QMessageHandler messageHandler;
};

#endif // QLOGWRITER_P_H
