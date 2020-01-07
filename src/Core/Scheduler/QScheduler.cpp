#include "QScheduler.h"
#include "QScheduler_p.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QStringBuilder>
#include <QFileInfo>

#include "QSchedulerHost.h"
#include "QSchedulerNode.h"

#include "../FastPaths/QFastPaths.h"

#include "../Safeguard/QSafeguard.h"
#include "../LogWriter/QLogWriter.h"

Q_LOGGING_CATEGORY(lcScheduler, "Ol.Scheduler")

static void createScheduler()
{
    QSchedulerPrivate::createScheduler();
}
Q_COREAPP_STARTUP_FUNCTION(createScheduler)

// class QScheduler

QScheduler::QScheduler()
    : d_ptr(new QSchedulerPrivate())
{
    d_ptr->q_ptr = this;
    Q_ASSERT(nullptr == QSchedulerPrivate::instance);
    QSchedulerPrivate::instance = this;

    QStringList arguments = QCoreApplication::arguments();
    if (arguments.contains("--scheduling-name")) {
        QSchedulerNode *node = new QSchedulerNode();
        QObject::connect(node, &QSchedulerNode::called,
                         this, &QScheduler::called);
    } else {
        new QSchedulerHost();
    }
}

QScheduler::~QScheduler()
{
    QSchedulerPrivate::instance = nullptr;
}

QScheduler *QScheduler::instance()
{
    return QSchedulerPrivate::instance;
}

QSettings *QScheduler::settings()
{
    if (!QSchedulerPrivate::instance) {
        qWarning(lcScheduler, "initialize before using it.");
        return nullptr;
    }

    static bool initialized = false;
    if (initialized)
        return QSchedulerPrivate::settings;
    initialized = true;

    QString fileName = qAppPath("DeveloperMode.ini");
    if (!QFileInfo(fileName).isFile()) {
        return nullptr;
    }

    QSchedulerPrivate::settings = new QSettings(fileName,
                                                QSettings::IniFormat,
                                                QSchedulerPrivate::instance);

    return QSchedulerPrivate::settings;
}

void QScheduler::initialize()
{
    QSchedulerPrivate::initialized = true;
}

void QScheduler::initKey(const QByteArray &key)
{
    QLogWriter::initKey(key);
}

void QScheduler::initIv(const QByteArray &iv)
{
    QLogWriter::initIv(iv);
}

void QScheduler::call(const QString &routeName)
{
    if (!QSchedulerPrivate::instance) {
        qWarning(lcScheduler, "initialize before using it.");
        return;
    }

    QScheduler::call(routeName, QStringList());
}

void QScheduler::call(const QString &routeName, const QStringList &arguments)
{
    if (!QSchedulerPrivate::instance) {
        qWarning(lcScheduler, "initialize before using it.");
        return;
    }

    if (QSchedulerNode::instance())
        QSchedulerNode::instance()->call(routeName, arguments);
}

void QScheduler::quit(const QString &routeName)
{
    if (!QSchedulerPrivate::instance) {
        qWarning(lcScheduler, "initialize before using it.");
        return;
    }

    if (QSchedulerNode::instance())
        QSchedulerNode::instance()->quit(routeName);
}

void QScheduler::addProgramRoute(const QString &route, QScheduler::Functor functor)
{
    if (!QSchedulerPrivate::instance) {
        qWarning(lcScheduler, "initialize before using it.");
        return;
    }

    if (QSchedulerHost::instance())
        QSchedulerHost::instance()->addProgramRoute(route, functor);
    if (QSchedulerNode::instance())
        QSchedulerNode::instance()->addProgramRoute(route, functor);
}

void QScheduler::addServiceRoute(const QString &route, QScheduler::Functor functor)
{
    if (!QSchedulerPrivate::instance) {
        qWarning(lcScheduler, "initialize before using it.");
        return;
    }

    if (QSchedulerHost::instance())
        QSchedulerHost::instance()->addServiceRoute(route, functor);
    if (QSchedulerNode::instance())
        QSchedulerNode::instance()->addServiceRoute(route, functor);
}

void QScheduler::addCleanupRoute(QScheduler::Functor functor)
{
    if (!QSchedulerPrivate::instance) {
        qWarning(lcScheduler, "initialize before using it.");
        return;
    }

    if (QSchedulerHost::instance())
        QSchedulerHost::instance()->addCleanupRoute(functor);
    if (QSchedulerNode::instance())
        QSchedulerNode::instance()->addCleanupRoute(functor);
}

QStringList QScheduler::lastArguments()
{
    if (!QSchedulerPrivate::instance) {
        qWarning(lcScheduler, "initialize before using it.");
        return QStringList();
    }

    if (QSchedulerNode::instance())
        return QSchedulerNode::instance()->lastArguments();
    return QCoreApplication::arguments().mid(1);
}

bool QScheduler::isDeveloperMode()
{
    if (!QSchedulerPrivate::instance) {
        qWarning(lcScheduler, "initialize before using it.");
        return false;
    }

    if (QSchedulerHost::instance())
        return QSchedulerHost::instance()->isDeveloperMode();
    if (QSchedulerNode::instance())
        return QSchedulerNode::instance()->isDeveloperMode();

    return false;
}

QString QScheduler::dataPath()
{
    if (!QSchedulerPrivate::instance) {
        qWarning(lcScheduler, "initialize before using it.");
        return QString();
    }

    if (QSchedulerNode::instance())
        return QSchedulerNode::instance()->dataPath();
    return QString();
}

void QScheduler::initMessageHandler(OlMessageHandler handler)
{
    QLogWriter::initMessageHandler(handler);
}

// class QSchedulerPrivate

bool QSchedulerPrivate::initialized = false;

QScheduler *QSchedulerPrivate::instance = nullptr;
QSettings   *QSchedulerPrivate::settings = nullptr;

QSchedulerPrivate::QSchedulerPrivate()
    : q_ptr(nullptr)
{
}

QSchedulerPrivate::~QSchedulerPrivate()
{
}

QScheduler *QSchedulerPrivate::createScheduler()
{
    if (QSchedulerPrivate::initialized) {
        return new QScheduler();
    }
    return nullptr;
}
