#include "serialportui.h"
#include "ui_serialportui.h"

SerialPortUi::SerialPortUi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialPortUi)
{
    ui->setupUi(this);
    connect(ui->radioButton_CRCNO, SIGNAL(clicked()), this, SLOT(radioButton_CRC_clicked()));
    connect(ui->radioButton_CRC8, SIGNAL(clicked()), this, SLOT(radioButton_CRC_clicked()));
    connect(ui->radioButton_CRC16, SIGNAL(clicked()), this, SLOT(radioButton_CRC_clicked()));
}

SerialPortUi::~SerialPortUi()
{
    delete ui;
}

void SerialPortUi::on_pushButton_Open_clicked()
{

}

void SerialPortUi::on_pushButton_Close_clicked()
{

}

void SerialPortUi::on_pushButton_Clear_clicked()
{

}

void SerialPortUi::on_pushButton_Send_clicked()
{

}

void SerialPortUi::radioButton_CRC_clicked()
{
    QRadioButton *rtn = dynamic_cast<QRadioButton*>(sender());
    int key = rtn->whatsThis().toInt();
    switch (key) {
    case 1:
        ui->textBrowser->append("CRCNO");
        break;
    case 2:
        ui->textBrowser->append("CRC8");
        break;
    case 3:
        ui->textBrowser->append("CRC16");
        break;
    default:
        break;
    }
}
