#include "fanmotorui.h"
#include "ui_fanmotorui.h"

#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QStandardItemModel>
#include <QTimer>

#include<qdebug.h>

FanMotorUi::FanMotorUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FanMotorUi)
{
    ui->setupUi(this);

    m_pollingTimer = new QTimer(this);
    connect(m_pollingTimer, SIGNAL(timeout()), this, SLOT(pollingTimerUpdate()));

    m_modbusUi = ModbusUi::getInstance();
    modbusDevice = m_modbusUi->getModbusDevice();
    connect(ui->connectButton, SIGNAL(clicked()),
            m_modbusUi, SLOT(on_connectButton_clicked()));

    connect(modbusDevice, &QModbusClient::stateChanged,
            this, &FanMotorUi::onStateChanged);
    for(int i = 0; i< ui->spinBox_motorNum->value(); i++){
        m_motor.push_back(new QMotor{0});

    }
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
    int startAddress = 0x0040;

    int numberOfEntries = sizeof(QCommunicationState);
    qDebug() <<numberOfEntries;

    return QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);
}
void FanMotorUi::on_pushButton_read_clicked()//read settings
{
    if (!modbusDevice)
        return;
//    statusBar()->clearMessage();
    if (auto *reply = modbusDevice->sendReadRequest(readRequest(), ui->spinBox_currentaddress->value())) {
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, &FanMotorUi::readReady);
            qDebug() <<"wait";
        }
        else
            delete reply; // broadcast replies return immediately
    } else {
        qDebug() << "error:" <<modbusDevice->errorString() ;
//        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
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
                                     .arg(QString::number(unit.value(i),
                                          unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
            quint16 *buff = (quint16 *)&m_motor.at(reply->serverAddress()-ui->spinBox_startaddress->value())->m_initSetttings;
            *buff++ = unit.value(i);

        }
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        qDebug() <<reply->errorString();
//        statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
//                                    arg(reply->errorString()).
//                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        qDebug() <<reply->errorString();
//        statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
//                                    arg(reply->errorString()).
//                                    arg(reply->error(), -1, 16), 5000);
    }

    if(m_currentServerAddress == reply->serverAddress()){
        if(m_pollingTimer->isActive()){
            qDebug()<<"timer timer";
            for(m_currentServerAddress++;m_currentServerAddress < m_motor.count()+ui->spinBox_startaddress->value(); m_currentServerAddress++){
                if(m_motor.at(m_currentServerAddress-ui->spinBox_startaddress->value())->m_communicationState == QCommunicationState::m_connect){

                    sendOnePolling(m_currentServerAddress);
                    qDebug()<<"send send";
                    break;
                }
            }
            qDebug()<<"timer ccc";
        }
    }
    reply->deleteLater();
}


QModbusDataUnit FanMotorUi::writeRequest() const
{
    const QModbusDataUnit::RegisterType table = QModbusDataUnit::HoldingRegisters;

    int startAddress = 0x0040;

    int numberOfEntries = sizeof(QMotorSettings)/2;
    return QModbusDataUnit(table, startAddress, numberOfEntries);
}
void FanMotorUi::on_pushButton_write_clicked()//write settings
{
    if (!modbusDevice)
        return;
//    statusBar()->clearMessage();

    QModbusDataUnit writeUnit = writeRequest();

    quint16 *buff = (quint16 *)&m_motor.at(ui->spinBox_currentaddress->value()-ui->spinBox_startaddress->value())->m_initSetttings;
    for (uint i = 0; i < writeUnit.valueCount(); i++) {

        writeUnit.setValue(i, *buff++);
    }
    qDebug() <<writeUnit.valueCount();
    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, ui->spinBox_currentaddress->value())) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
//                    statusBar()->showMessage(tr("Write response error: %1 (Mobus exception: 0x%2)")
//                        .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16),
//                        5000);
                } else if (reply->error() != QModbusDevice::NoError) {
//                    statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2)").
//                        arg(reply->errorString()).arg(reply->error(), -1, 16), 5000);
                }
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
//        statusBar()->showMessage(tr("Write error: ") + modbusDevice->errorString(), 5000);
    }
}

void FanMotorUi::on_startButton_clicked()
{
    if(!m_pollingTimer->isActive())
        m_pollingTimer->start(3000);

}

void FanMotorUi::on_stopButton_clicked()
{
    if(m_pollingTimer->isActive())
        m_pollingTimer->stop();
    qDebug()<<"timer stop?"<< m_pollingTimer->isActive();
}

void FanMotorUi::pollingTimerUpdate()
{
    qDebug()<<"timer over";
    m_currentServerAddress = 1;
    sendOnePolling(m_currentServerAddress);

}

void FanMotorUi::sendOnePolling(int address)
{
    qDebug()<<"reply aaa";
    if (!modbusDevice)
        return;

    int startAddress = 0x0060;

    int numberOfEntries = sizeof(QMotorController)/2;

    QModbusDataUnit __dataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);

    qDebug()<<"reply bbb";
    if (auto *reply = modbusDevice->sendReadRequest(__dataUnit, address)) {
        qDebug()<<"reply ccc";
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, &FanMotorUi::readReady);
        }
        else
            delete reply; // broadcast replies return immediately
    } else {
        qDebug() << "error:" <<modbusDevice->errorString() ;
    }
}



