#include "consolepart0.h"
#include "ui_consolepart0.h"
#include <QMessageBox>
#include "mainwindow/mainwindow.h"

consolepart0::consolepart0(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::consolepart0)
{
    initData();
    initUI();
    initThread();
    initConnect();
}

void consolepart0::initData()
{
}

void consolepart0::initUI()
{
    ui->setupUi(this);
    QGridLayout *frame_lamp_gridLayout = new QGridLayout;
    lamp = new QcwIndicatorLamp;
    frame_lamp_gridLayout->addWidget(lamp);
    ui->frame_lamp->setLayout(frame_lamp_gridLayout);

    QGridLayout *groupBox_console_gridLayout = new QGridLayout;
    console = new Console;
    groupBox_console_gridLayout->addWidget(console);
    ui->groupBox_console->setLayout(groupBox_console_gridLayout);

    connectButton = new FSetPushButton("connectButton");
    connectButton->setObjectName("connectButton");
    disConnectButton = new FSetPushButton("disConnectButton");
    disConnectButton->setObjectName("disConnectButton");
    clearButton = new FSetPushButton("clearButton");
    clearButton->setObjectName("clearButton");
    QGridLayout *groupBox_button_gridLayout = new QGridLayout;
    groupBox_button_gridLayout->addWidget(connectButton,1,0);
    groupBox_button_gridLayout->addWidget(disConnectButton,2,0);
    groupBox_button_gridLayout->addWidget(clearButton,3,0);
    groupBox_button_gridLayout->setAlignment(Qt::AlignCenter);
    groupBox_button_gridLayout->setContentsMargins(0,0,0,0);
    ui->groupBox_button->setLayout(groupBox_button_gridLayout);
    ui->gridLayout->setContentsMargins(5,5,5,5);
    ui->gridLayout->setSpacing(5);
    QLabel *label = new QLabel(this);
    label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label->setText("功能\n演示");
    label->setFixedWidth(100);
    frame_lamp_gridLayout->addWidget(label,1,0);

    uiTimer = new QTimer(this);
    consoleString = "hello !\n";


}

void consolepart0::initThread()
{
    uiTimer->start(100);//consoleString

}

void consolepart0::initConnect()
{
    connect(uiTimer, SIGNAL(timeout()), this, SLOT(onUiTimerTimeOut()) );
}

void consolepart0::onUiTimerTimeOut()
{
    if(console->isEnabled())
    {
        console->putData(consoleString);
        consoleString.clear();
    }
}

void consolepart0::messageShow(const QString &s)
{
    consoleString.append(s+"\n");

}

consolepart0::~consolepart0()
{
    if(uiTimer->isActive())
        uiTimer->stop();
    delete ui;
}
