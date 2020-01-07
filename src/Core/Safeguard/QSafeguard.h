#ifndef QSAFEGUARD_H
#define QSAFEGUARD_H

#include <QObject>

class QSafeguardPrivate;
class QSafeguard : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSafeguard)

    Q_PROPERTY(QString dumpPath READ dumpPath WRITE setDumpPath NOTIFY dumpPathChanged)
    Q_PROPERTY(QString pipeName READ pipeName WRITE setPipeName NOTIFY pipeNameChanged)

public:
    explicit QSafeguard(const QString &dumpPath, QObject *parent = nullptr);
    explicit QSafeguard(QObject *parent = nullptr);
    virtual ~QSafeguard();
protected:
    QScopedPointer<QSafeguardPrivate> d_ptr;

signals:
    void dumpPathChanged(const QString &path);
    void pipeNameChanged(const QString &name);
public slots:
    void setDumpPath(const QString &path);
    void setPipeName(const QString &name);
public:
    const QString &dumpPath() const;
    const QString &pipeName() const;

public slots:
    void createServer();
    void createClient();
    void makeSnapshot();
};

#endif // QSAFEGUARD_H
