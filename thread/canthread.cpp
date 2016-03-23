#include "canthread.h"
#include "candriver/can.h"

CanThread *CanThread::s_instance = 0;

CanThread::CanThread(QObject *parent)
    : QThread(parent)
{

}

CanThread::~CanThread()
{
    wait();
}


CanThread* CanThread::getInstance()
{
    if(!s_instance)
    {
        s_instance = new CanThread();
    }
    return s_instance;
}

void CanThread::mStart()
{
    m_canMutex.lock();
    m_isStopped = false;
    m_canMutex.unlock();

    if (!this->isRunning())
        this->start();
}

void CanThread::mStop()
{
    m_canMutex.lock();
    m_isStopped = true;
    m_canMutex.unlock();
}

void CanThread::run()
{
    bool isStopped = true;
    m_canMutex.lock();
    isStopped = m_isStopped;
    m_canMutex.unlock();

    qDebug("thread running");

    Message *m = new Message();
    while(!isStopped)
    {
        int ret=0;
        ret = usbCanReceive(0, m);
        if(ret>0){
            QString str = QString("receive data: id:%1 len:%2 "
                                  "data:%3 %4 %5 %6 %7 %8 %9 %10")
                    .arg(m->cob_id).arg(m->len)
                    .arg(m->data[0]).arg(m->data[1]).arg(m->data[2]).arg(m->data[3])
                    .arg(m->data[4]).arg(m->data[5]).arg(m->data[6]).arg(m->data[7]);
            message(str);
        }

        m_canMutex.lock();
        isStopped = m_isStopped;
        m_canMutex.unlock();
    }
    delete m;

    qDebug("thread exit");
}
