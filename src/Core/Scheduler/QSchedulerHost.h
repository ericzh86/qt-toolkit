#ifndef QSCHEDULERHOST_H
#define QSCHEDULERHOST_H

#include <QLocalServer>
#include <QLocalSocket>

class QSchedulerHostPrivate;
class QSchedulerHost : public QLocalServer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSchedulerHost)

public:
    QSchedulerHost();
    virtual ~QSchedulerHost();
    static QSchedulerHost *instance();
protected:
    QScopedPointer<QSchedulerHostPrivate> d_ptr;

public:
    typedef std::function<bool()> Functor;
    enum Mode { Unknown, Program, Service, Cleanup };

    void addProgramRoute(const QString &route, Functor functor);
    void addServiceRoute(const QString &route, Functor functor);
    void addCleanupRoute(Functor functor);

public:
    void call(const QString &routeName, const QStringList &arguments);
    void quit(const QString &routeName);

public:
    bool isDeveloperMode() const;

private:
    void incomingConnection(quintptr s) final;

    void onError(QLocalSocket::LocalSocketError error);
    void onStateChanged(QLocalSocket::LocalSocketState state);

private:
    void cleanTasks();
};

#endif // QSCHEDULERHOST_H
