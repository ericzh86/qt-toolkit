#ifndef QSCHEDULER_H
#define QSCHEDULER_H

#include "../QtkCoreGlobal.h"

#include <functional>
#include <QCoreApplication>
#include <QSettings>

typedef void (* OlMessageHandler)(QtMsgType type,
                                  const QMessageLogContext &context,
                                  const QString &message);

class QTK_CORE_EXPORT QSchedulerPrivate;
class QTK_CORE_EXPORT QScheduler : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QScheduler)

protected:
    explicit QScheduler();
protected:
    QScopedPointer<QSchedulerPrivate> d_ptr;
public:
    virtual ~QScheduler();

public:
    static QScheduler *instance();
    static QSettings  *settings();
public:
    static void initialize();

public:
    static void initKey(const QByteArray &key);
    static void initIv(const QByteArray &iv);

public:
    static void call(const QString &routeName);
    static void call(const QString &routeName, const QStringList &arguments);
    static void quit(const QString &routeName);

public:
    typedef std::function<bool()> Functor;
    static void addProgramRoute(const QString &route, Functor functor);
    static void addServiceRoute(const QString &route, Functor functor);
    static void addCleanupRoute(Functor functor);

signals:
    void called(const QStringList &arguments);
public:
    static QStringList lastArguments();
    static bool isDeveloperMode();
    static QString dataPath();

public:
    static void initMessageHandler(OlMessageHandler handler);
};

#define theScheduler QScheduler::instance()

#endif // QSCHEDULER_H
