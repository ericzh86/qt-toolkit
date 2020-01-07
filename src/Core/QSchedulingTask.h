#ifndef QSCHEDULINGTASK_H
#define QSCHEDULINGTASK_H

#include <QProcess>

class QSchedulingNode;
class QSchedulingTaskPrivate;
class QSchedulingTask : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSchedulingTask)

public:
    QSchedulingTask(const QString &route, QObject *parent = nullptr);
protected:
    QScopedPointer<QSchedulingTaskPrivate> d_ptr;
public:
    virtual ~QSchedulingTask();

public:
    const QString &route() const;

public:
    enum Mode { Program, Service };
public:
    void setMode(Mode mode);
public:
    Mode mode() const;

signals:
    void quitAll();
public:
    void call();
    void call(const QStringList &arguments);
    void quit();

public:
    void attachNode(QSchedulingNode *node);
    QSchedulingNode *node() const;
    bool isClosed() const;
    void clear();

private:
    void onError(QProcess::ProcessError error);
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // QSCHEDULINGTASK_H
