#include "fanmotorui.h"
#include "ui_fanmotorui.h"



FanMotorUi::FanMotorUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FanMotorUi)
{
    ui->setupUi(this);
}

FanMotorUi::~FanMotorUi()
{
    delete ui;
}
