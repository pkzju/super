#ifndef CANTHREAD_H
#define CANTHREAD_H

#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>


class CanThread : public QThread
{
    Q_OBJECT

public:
    CanThread(QObject *parent = 0);
    ~CanThread();
    static CanThread* getInstance();
    void mStart();
    void mStop();
    void run();

signals:
    void message(const QString &s);

private:
    static CanThread* s_instance;
    QMutex m_canMutex;
    bool m_isStopped;

};

#endif // CANTHREAD_H
