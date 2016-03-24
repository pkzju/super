#include "serialportui.h"
#include "ui_serialportui.h"

SerialPortUi::SerialPortUi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialPortUi)
{
    ui->setupUi(this);
}

SerialPortUi::~SerialPortUi()
{
    delete ui;
}
