#ifndef QSCHEDULINGNODE_H
#define QSCHEDULINGNODE_H

#include <QLocalSocket>

class QSchedulingNodePrivate;
class QSchedulingNode : public QLocalSocket
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSchedulingNode)

public:
    QSchedulingNode();
    virtual ~QSchedulingNode();
protected:
    QScopedPointer<QSchedulingNodePrivate> d_ptr;

public:
    void setRouteName(const QString &name);
    QString routeName() const;

signals:
    void callRequested(const QString &routeName, const QStringList &arguments);
    void quitRequested(const QString &routeName);
public:
    void call(const QStringList &arguments);
    void quit();

public:
    bool writeDatagram(qint16 type, const char *data, int size);
    bool writeDatagram(qint16 type, const QByteArray &datagram);

public:
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

public:
    void processDatagram();
    void dispatch();
};

#endif // QSCHEDULINGNODE_H
