#ifndef QSCHEDULERNODE_H
#define QSCHEDULERNODE_H

#include <QLocalSocket>

class QSchedulerNodePrivate;
class QSchedulerNode : public QLocalSocket
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSchedulerNode)

public:
    QSchedulerNode();
    virtual ~QSchedulerNode();
    static QSchedulerNode *instance();
protected:
    QScopedPointer<QSchedulerNodePrivate> d_ptr;
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
    void writeDatagram(qint16 type, const char *data, int size);
    void writeDatagram(qint16 type, const QByteArray &datagram);

signals:
    void called(const QStringList &arguments);
public:
    QStringList lastArguments() const;
    bool isDeveloperMode() const;
    QString dataPath() const;

private:
    void onError(QLocalSocket::LocalSocketError error);
    void onStateChanged(QLocalSocket::LocalSocketState state);

    void processDatagram();
    void dispatch();

private:
    template <typename Type>
    bool store(const Type *type, int size) {
        if (write(reinterpret_cast<const char *>(type), size) == size)
        { return true; } else { close(); return false; }
    }
    template <typename Type>
    bool load(Type *type, int size) {
        if (read(reinterpret_cast<char *>(type), size) == size)
        { return true; } else { close(); return false; }
    }

private:
    bool event(QEvent *event);
};

#endif // QSCHEDULERNODE_H
