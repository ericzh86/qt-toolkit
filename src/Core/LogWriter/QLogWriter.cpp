#include "QLogWriter.h"
#include "QLogWriter_p.h"

#if defined(Q_OS_WIN)
#  include <Windows.h>
#endif

#include <iostream>

#include <QUuid>
#include <QDateTime>
#include <QFileInfo>
#include <QThread>
#include <QFile>

#include <QCoreApplication>
#include <QStringBuilder>
#include <QLoggingCategory>

/*
#include "QSchedulerHost.h"
#include "QSchedulerNode.h"
#include "OlStandardPaths.h"

#include "OlRaven.h"
*/

Q_LOGGING_CATEGORY(lcLogWriter, "Ol.LogWriter")

// class QLogWriter

QLogWriter::QLogWriter(const QString &dataPath, QObject *parent)
    : QObject(parent)
    , d_ptr(new QLogWriterPrivate())
{
    d_ptr->q_ptr = this;

    Q_D(QLogWriter);

    d->encodeWriter.reset(new QLogStreamWriter());
    d->encodeWriter->setKey(QLogWriterPrivate::key);
    d->encodeWriter->setIv(QLogWriterPrivate::iv);

    if (!dataPath.isEmpty()) {
        for (int i = 0; i < 10; ++i) {
            QString baseName = QUuid::createUuid().toString(QUuid::WithoutBraces);
            QString fileName = dataPath % "/" % baseName % ".log";
            QScopedPointer<QFile> file(new QFile(fileName, this));
            if (file->open(QFile::WriteOnly)) {
                d->streamWriter.reset(new QLogStreamWriter(file.take(), QLogStreamWriter::WriteMode));
                d->streamWriter->setKey(QLogWriterPrivate::key);
                d->streamWriter->setIv(QLogWriterPrivate::iv);
                break;
            }
        }
    }

    QStringList arguments = QCoreApplication::arguments();
    if (arguments.contains("--developer-mode")) {
        d->isDeveloperMode = true;
    }
    if (!d->isDeveloperMode) {
        /*
        QString fileName = OlStandardPaths::rootPath("DeveloperMode.ini");
        d->isDeveloperMode = QFileInfo(fileName).exists();
        */
    }
}

QLogWriter::~QLogWriter()
{
}

void QLogWriter::initKey(const QByteArray &key)
{
    QLogWriterPrivate::key = key;
}

void QLogWriter::initIv(const QByteArray &iv)
{
    QLogWriterPrivate::iv = iv;
}

void QLogWriter::writeBlockBuffer(const QByteArray &data)
{
    QLogWriter::writeBlockBuffer(data.constData(), data.size());
}

void QLogWriter::writeBlockBuffer(const char *data, int size)
{
    auto inst = QLogWriterPrivate::instance;
    if (nullptr == inst) {
        return;
    }

    if (inst->streamWriter)
        inst->streamWriter->writeBlockBuffer(data, size);
}

void QLogWriter::writeGracefulEnding(qint64 exitCode)
{
    auto inst = QLogWriterPrivate::instance;
    if (nullptr == inst) {
        return;
    }

    if (inst->streamWriter) {
        qInfo(lcLogWriter, "C'est la vie");
        qInfo(lcLogWriter, "C'est la vie");
        qInfo(lcLogWriter, "C'est la vie");
        inst->streamWriter->writeGracefulEnding(exitCode);
        QFileDevice *file = qobject_cast<QFileDevice *>(inst->streamWriter->device());
        if (nullptr != file) {
            file->flush();
            file->close();
        }

        inst->streamWriter.clear();
    }
}

void QLogWriter::writePlainBuffer(const char *data, int size)
{
    static QByteArray buffer(0x4000, 0);
    if (size >= 0x4000) {
        size = 0x4000 - 1;
    }

    memcpy(buffer.data(), data, size);
    if (buffer[size - 1] != '\n') {
        buffer[size] = '\n';
        buffer[size + 1] = 0;
    } else {
        buffer[size] = 0;
    }

#if defined(Q_OS_WIN)
    OutputDebugStringA(buffer.constData());
#else
    std::cout << buffer.constData() << std::endl;
#endif
}

void QLogWriter::initMessageHandler(QMessageHandler handler)
{
    QLogWriterPrivate::messageHandler = handler;
}

// class QLogWriterPrivate

QLogWriterPrivate *QLogWriterPrivate::instance = nullptr;

void basicMessageHandler(QtMsgType type,
                         const QMessageLogContext &context,
                         const QString &message)
{
    static QMutex guard;

    auto inst = QLogWriterPrivate::instance;
    if (nullptr == inst) {
        return;
    }

    if (message.isEmpty()) {
        return;
    }

    if (QLogWriterPrivate::messageHandler) {
        QLogWriterPrivate::messageHandler(type, context, message);
    }

    /*
    auto node = QSchedulerNode::instance();
    if (inst->encodeWriter) {
        qint16 size = 0;
        if (inst->streamWriter) {
            if (message.startsWith("QWindowsPipeWriter: asynchronous write failed.")) {
                // ignore this warning
                return;
            }

            QMutexLocker locker(&guard);

            if (inst->isDeveloperMode) {
                QByteArray data = inst->plainMessage(type, context, message);
                QLogWriter::writePlainBuffer(data.constData(), data.size());
            } else {

            }
            do {
                if ((type == QtDebugMsg) || (type == QtInfoMsg)) {
                    if (message.startsWith("pick@")) {
                        auto raven = OlRaven::instance();
                        if (!raven)
                            raven = new OlRaven(QString());

                        int i = message.indexOf("=", 5);
                        if (i >= 0) {
                            QString k = message.mid(5, i - 5);
                            QString v = message.mid(i + 1);
                            if (v.isEmpty()) {
                                v = "<empty>";
                            }

                            raven->setTagValue(k, v);
                        }
                        break;
                    } else if (!message.startsWith("sentry@")) {
                        break;
                    }
                }

                auto raven = OlRaven::instance();
                if (!raven)
                    raven = new OlRaven(QString());

                raven->send(type, context, message);
            } while (false);

            inst->encodeWriter->write(type, message, context);
            QByteArray body = inst->encodeWriter->blockBuffer(size);
            inst->streamWriter->writeBlockBuffer(body.constData(), size);
        } else if (node && (QLocalSocket::ConnectedState == node->state())) {
            QMutexLocker locker(&guard);

            if (inst->isDeveloperMode) {
                QByteArray data = inst->plainMessage(type, context, message);
                node->writeDatagram(4, data);
            } else {
                ;
            }
            do {
                if ((type == QtDebugMsg) || (type == QtInfoMsg)) {
                    if (message.startsWith("pick@")) {
                        node->writeDatagram(11, message.toUtf8());
                        break;
                    } else if (!message.startsWith("sentry@")) {
                        break;
                    }
                }

                QString v = OlRaven::pack(type, context, message);
                node->writeDatagram(12, v.toUtf8());
            } while (false);

            inst->encodeWriter->write(type, message, context);
            QByteArray body = inst->encodeWriter->blockBuffer(size);
            node->writeDatagram(3, body.constData(), size);
        }
    }
    */
}

QByteArray QLogWriterPrivate::key;
QByteArray QLogWriterPrivate::iv;

QMessageHandler QLogWriterPrivate::messageHandler = nullptr;

QLogWriterPrivate::QLogWriterPrivate()
    : q_ptr(nullptr)
    , isDeveloperMode(false)
{
    Q_ASSERT(nullptr == instance);
    instance = this;
    qInstallMessageHandler(basicMessageHandler);
}

QLogWriterPrivate::~QLogWriterPrivate()
{
    qInstallMessageHandler(nullptr);
    instance = nullptr;
}

QByteArray QLogWriterPrivate::plainMessage(QtMsgType type,
                                            const QMessageLogContext &context,
                                            const QString &message)
{
#if defined(Q_OS_WIN)
    static qint64 processId = GetCurrentProcessId();
#else
    static qint64 processId = 0;
#endif

    QString timeText = QTime::currentTime().toString(Qt::ISODateWithMs);

    QString typeText;
    switch (type) {
    case QtDebugMsg:
        typeText = QLatin1Literal("DEBG");
        break;
    case QtInfoMsg:
        typeText = QLatin1Literal("INFO");
        break;
    case QtWarningMsg:
        typeText = QLatin1Literal("WARG");
        break;
    case QtCriticalMsg:
        typeText = QLatin1Literal("CRIT");
        break;
    case QtFatalMsg:
        typeText = QLatin1Literal("FATL");
        break;
    }

    QString result;
    if (!context.category) {
        result = QString::fromLatin1("%1 %2 default(pid:%3): %4").arg(typeText, timeText).arg(processId).arg(message);
    } else {
        result = QString::fromLatin1("%1 %2 %3(pid:%4): %5").arg(typeText, timeText, QString::fromLatin1(context.category)).arg(processId).arg(message);
    }

    return result.toLocal8Bit();
}
