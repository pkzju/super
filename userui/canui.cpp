#include "canui.h"
#include "ui_canui.h"
#include<qlibrary.h>


CANUi *CANUi::s_Instance = 0;

CANUi::CANUi(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CANUi),m_masterBoard(new s_BOARD()),m_masterData(new CO_Data()),
    m_canThread(CanThread::getInstance())
{
    ui->setupUi(this);
    initData();
    initUI();
    initThread();
    initConnect();
}

void CANUi::initData()
{

}

void CANUi::initUI()
{

}

void CANUi::initThread()
{

}

void CANUi::initConnect()
{
    connect(m_canThread, SIGNAL(message(QString)),
            this, SLOT(messageShow(QString)));
}

CANUi::~CANUi()
{
    delete ui;
}

CANUi *CANUi::getS_Instance()
{
    if(!s_Instance)
    {
        s_Instance = new CANUi;
    }
    return s_Instance;
}

void CANUi::on_pushButton_Open_clicked()
{
    canOpen(m_masterBoard, m_masterData);
    m_canThread->mStart();
}

void CANUi::on_pushButton_Close_clicked()
{
    m_canThread->mStop();
    canClose(m_masterData);
}

void CANUi::on_pushButton_Clear_clicked()
{
    ui->textBrowser->clear();
}

void CANUi::messageShow(const QString &s)
{

    ui->textBrowser->append(s);
}

void CANUi::on_pushButton_Send_clicked()
{
    int ret=0;
    Message *m = new Message();
    m->cob_id = 3;
    m->len = 8;
    m->data[0] = 5;
    ret = usbCanSend(0, m);

    QString str;
    str.setNum(ret);
    ui->textBrowser->append(str);
    delete m;
}
