#include "canoperate.h"
#include "ui_canoperate.h"

CANOperate *CANOperate::s_Instance = 0;

CANOperate::CANOperate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CANOperate), m_GLayout(new QGridLayout),
    m_Button_Open(new FBasePushButton), m_Button_Close(new FToolButton())
{
    ui->setupUi(this);
    m_Button_Close->setIcon(QIcon(QString(":/skin/icons/dark/appbar.lock.png")));
    m_GLayout->addWidget(m_Button_Open, 0, 0);
    m_GLayout->addWidget(m_Button_Close, 0, 1);
    setLayout(m_GLayout);

}

CANOperate::~CANOperate()
{
    delete ui;
}

CANOperate *CANOperate::GetInstance()
{
    if(!s_Instance)
    {
        s_Instance = new CANOperate();
    }
    return s_Instance;
}
