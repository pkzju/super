#include "fanmotorui.h"
#include "ui_fanmotorui.h"

#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QStandardItemModel>
#include <QTimer>

#include<qdebug.h>
#include "lamp/qcw_indicatorlamp.h"

FanMotorUi::FanMotorUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FanMotorUi)
{
    ui->setupUi(this);

    m_isSearching = 0;
    m_motorNum = ui->spinBox_motorNum->value();
    m_startServerAddress = ui->spinBox_startaddress->value();
    for(int i = 0; i< m_motorNum; i++){
        m_motor.push_back(new QMotor{0});
        m_commLamp.push_back(new QcwIndicatorLamp);
        ui->table_realtime->setCellWidget(i, 8, m_commLamp.at(i));
        ui->table_realtime->model()->setData(ui->table_realtime->model()->index(i,0), 0x16);
    }

    m_pollingTimer = new QTimer(this);
    connect(m_pollingTimer, SIGNAL(timeout()), this, SLOT(pollingTimerUpdate()));

    m_modbusUi = ModbusUi::getInstance();
    modbusDevice = m_modbusUi->getModbusDevice();
    connect(ui->connectButton, SIGNAL(clicked()),
            m_modbusUi, SLOT(on_connectButton_clicked()));

    connect(modbusDevice, &QModbusClient::stateChanged,
            this, &FanMotorUi::onStateChanged);

    m_motor.at(0)->m_communicationState =QCommunicationState::m_connect;
    m_motor.at(1)->m_communicationState =QCommunicationState::m_connect;
    m_motor.at(2)->m_communicationState =QCommunicationState::m_connect;

}

void FanMotorUi::onStateChanged(int state)
{
    //    bool connected = (state != QModbusDevice::UnconnectedState);
    //    ui->actionConnect->setEnabled(!connected);
    //    ui->actionDisconnect->setEnabled(connected);

    if (state == QModbusDevice::UnconnectedState)
        ui->connectButton->setText(tr("   Connect   "));
    else if (state == QModbusDevice::ConnectedState)
        ui->connectButton->setText(tr("  Disconnect "));
}

FanMotorUi::~FanMotorUi()
{
    delete ui;
    delete m_pollingTimer;
}

QModbusDataUnit FanMotorUi::readRequest() const
{
    int startAddress = g_mSettingsRegisterAddress; //register address

    int numberOfEntries = sizeof(QMotorSettings) >> 1; //16bit register number
    qDebug() <<numberOfEntries;

    return QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);
}
void FanMotorUi::on_pushButton_read_clicked()//read settings
{
    if (!modbusDevice)
        return;

    if (auto *reply = modbusDevice->sendReadRequest(readRequest(), ui->spinBox_currentaddress->value())) {
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, &FanMotorUi::readReady);
            qDebug() <<"wait";
        }
        else
            delete reply; // broadcast replies return immediately
    } else {
        ui->textBrowser->append(tr("Read error: ") + modbusDevice->errorString());
    }
}
void FanMotorUi::readReady()
{
    QModbusReply *reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for (uint i = 0; i < unit.valueCount(); i++) {
            const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress())
                    .arg(QString::number(unit.value(i), 16));
            ui->textBrowser->append(entry);

            if(unit.startAddress() == g_mSettingsRegisterAddress){
                quint16 *buff = (quint16 *)&m_motor.at(reply->serverAddress()-m_startServerAddress)->m_initSetttings;
                *buff++ = unit.value(i);
            }
            else if(unit.startAddress() == g_mControllerRegisterAddress){

            }
            else if(unit.startAddress() == g_mPIParaRegisterAddress){

            }

        }
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        qDebug() <<reply->errorString();
        ui->textBrowser->append(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16));
    } else {
        qDebug() << reply->errorString();
        ui->textBrowser->append(tr("Read response error: %1 (code: 0x%2)").
                                arg(reply->errorString()).arg(reply->error(), -1, 16));

        //ui->table_realtime->
    }

    if(m_currentServerAddress == reply->serverAddress()){

        if(m_pollingTimer->isActive()){//polling...
            qDebug()<<"polling...";
            for(m_currentServerAddress++;m_currentServerAddress < m_motorNum + m_startServerAddress; m_currentServerAddress++){
                if(m_motor.at(m_currentServerAddress-m_startServerAddress)->m_communicationState == QCommunicationState::m_connect){
                    sendOnePolling(m_currentServerAddress);
                    break;
                }
            }
        }
        else{//searching...
            if(m_isSearching){
                qDebug()<<"searching...";
                for(m_currentServerAddress++;m_currentServerAddress < m_motorNum + m_startServerAddress; m_currentServerAddress++){
                    sendOnePolling(m_currentServerAddress);
                    break;
                }
                if(m_currentServerAddress >= m_motorNum + m_startServerAddress){
                    m_isSearching = false;
                }

            }
        }
    }
    reply->deleteLater();
}


QModbusDataUnit FanMotorUi::writeRequest() const
{
    const QModbusDataUnit::RegisterType table = QModbusDataUnit::HoldingRegisters;

    int startAddress = g_mSettingsRegisterAddress;

    int numberOfEntries = sizeof(QMotorSettings) >> 1;
    return QModbusDataUnit(table, startAddress, numberOfEntries);
}

void FanMotorUi::on_pushButton_write_clicked()//write settings
{
    if (!modbusDevice)
        return;

    QModbusDataUnit writeUnit = writeRequest();

    quint16 *buff = (quint16 *)&m_motor.at(ui->spinBox_currentaddress->value()-m_startServerAddress)->m_initSetttings;
    for (uint i = 0; i < writeUnit.valueCount(); i++) {

        writeUnit.setValue(i, *buff++);
    }
    qDebug() <<writeUnit.valueCount();
    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, ui->spinBox_currentaddress->value())) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    ui->textBrowser->append(tr("Write response error: %1 (Mobus exception: 0x%2)")
                                            .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16)
                                            );
                } else if (reply->error() != QModbusDevice::NoError) {
                    ui->textBrowser->append(tr("Write response error: %1 (code: 0x%2)").
                                            arg(reply->errorString()).arg(reply->error(), -1, 16));
                }
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        ui->textBrowser->append(tr("Write error: ") + modbusDevice->errorString());
    }
}

void FanMotorUi::on_searchButton_clicked()
{
    m_isSearching = true;
    if(m_pollingTimer->isActive()){
        m_pollingTimer->stop();
        ui->startButton->setText(QStringLiteral("   start   "));
    }
    m_currentServerAddress = m_startServerAddress;
    sendOnePolling(m_currentServerAddress);
}

void FanMotorUi::on_startButton_clicked()
{
    if(m_isSearching)
        m_isSearching = false;

    if(!m_pollingTimer->isActive()){
        m_currentServerAddress = m_startServerAddress;
        sendOnePolling(m_currentServerAddress);
        m_pollingTimer->start(3000);
        ui->startButton->setText(QStringLiteral("   stop   "));
    }
    else{
        m_pollingTimer->stop();
        ui->startButton->setText(QStringLiteral("   start   "));
    }

}

void FanMotorUi::pollingTimerUpdate()
{
    qDebug()<<"timer update";
    m_currentServerAddress = 1;
    sendOnePolling(m_currentServerAddress);

}

void FanMotorUi::sendOnePolling(int address)
{
    qDebug()<<"send one polling";
    if (!modbusDevice)
        return;

    int startAddress = g_mControllerRegisterAddress;

    int numberOfEntries = sizeof(QMotorController) >> 1;

    QModbusDataUnit __dataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);

    if (auto *reply = modbusDevice->sendReadRequest(__dataUnit, address)) {
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, &FanMotorUi::readReady);
        }
        else
            delete reply; // broadcast replies return immediately
    } else {
        qDebug() << "error:" <<modbusDevice->errorString() ;
    }
}

void FanMotorUi::on_spinBox_startaddress_valueChanged(int arg1)
{
    m_startServerAddress = arg1;
}

void FanMotorUi::on_spinBox_motorNum_valueChanged(int arg1)
{
    m_motorNum = arg1;
}
