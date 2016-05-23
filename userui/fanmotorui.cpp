#include "fanmotorui.h"
#include "ui_fanmotorui.h"

#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QStandardItemModel>
#include <QTimer>

#include<qdebug.h>
#include "lamp/qcw_indicatorlamp.h"

#include"fanmotor/qmotor.h"
#include "userui/mplotui.h"

FanMotorUi::FanMotorUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FanMotorUi)
{
    ui->setupUi(this);

    m_pollingState = PollingState::Stop;

    m_realTimeServerAddress = ui->spinBox_currentaddress->value();
    m_motorNum = ui->spinBox_motorNum->value();
    m_startServerAddress = 1;

    for(int i = 0; i< m_motorNum; i++){
        m_motors.push_back(new QMotor(i+m_startServerAddress));
        ui->table_realtime->setCellWidget(i, 0, m_motors.at(i)->m_motorAddressLabel);
        ui->table_realtime->setCellWidget(i, 1, m_motors.at(i)->m_ratedPowerLCD);
        ui->table_realtime->setCellWidget(i, 2, m_motors.at(i)->m_targetPowerLCD);
        ui->table_realtime->setCellWidget(i, 3, m_motors.at(i)->m_nowPowerLCD);
        ui->table_realtime->setCellWidget(i, 4, m_motors.at(i)->m_ratedSpeedLCD);
        ui->table_realtime->setCellWidget(i, 5, m_motors.at(i)->m_speedRefLCD);
        ui->table_realtime->setCellWidget(i, 6, m_motors.at(i)->m_speedFbkLCD);
        ui->table_realtime->setCellWidget(i, 7, m_motors.at(i)->m_runLamp);
        ui->table_realtime->setCellWidget(i, 8, m_motors.at(i)->m_commLamp);

    }

    m_pollingTimer = new QTimer(this);
    connect(m_pollingTimer, SIGNAL(timeout()), this, SLOT(pollingTimerUpdate()));

    m_modbusUi = ModbusUi::getInstance();
    modbusDevice = m_modbusUi->getModbusDevice();
    connect(ui->connectButton, SIGNAL(clicked()),
            m_modbusUi, SLOT(on_connectButton_clicked()));

    connect(modbusDevice, &QModbusClient::stateChanged,
            this, &FanMotorUi::onStateChanged);

    m_motors.at(0)->m_communicationState =FanCommunicationState::m_connect;
    m_motors.at(1)->m_communicationState =FanCommunicationState::m_connect;
    m_motors.at(2)->m_communicationState =FanCommunicationState::m_connect;

    MPlotUi*__mPlotUi = MPlotUi::getInstance();
    connect(this, &FanMotorUi::updatePlotUi, __mPlotUi, &MPlotUi::realtimeDataSlot);

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

    int numberOfEntries = sizeof(FanMotorSettings) >> 1; //16bit register number
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

    int __currentAddress = reply->serverAddress() ;
    int __address = __currentAddress - m_startServerAddress;

    if (reply->error() == QModbusDevice::NoError) {

        const QModbusDataUnit unit = reply->result();

        quint16 *buff;
        if(unit.startAddress() == g_mSettingsRegisterAddress){//setting read
            buff = (quint16 *)&m_motors.at(__address)->m_initSetttings;
        }
        else if(unit.startAddress() == g_mControllerRegisterAddress){//single motor mode
            buff = (quint16 *)&m_motors.at(__address)->m_motorController;
        }
        else if(unit.startAddress() == g_mRealTimeRegisterAddress){//multi motor mode
            buff = (quint16 *)&m_motors.at(__address)->m_motorController;
            buff += g_mRatedRegisterCount;
        }
        else if(unit.startAddress() == g_mPIParaRegisterAddress){//pi read
            buff = (quint16 *)&m_motors.at(__address)->m_PIPara;
        }


        for (uint i = 0; i < unit.valueCount(); i++) {
            const QString entry = tr("Address: %1, Value: %2").arg(QString::number(unit.startAddress()+i, 16))
                    .arg(QString::number(unit.value(i), 16));
            ui->textBrowser->append(entry);

            *buff++ = unit.value(i);

        }

        m_motors.at(__address)->m_communicationState =FanCommunicationState::m_connect;

    } else if (reply->error() == QModbusDevice::ProtocolError) {
        qDebug() <<reply->errorString();
        ui->textBrowser->append(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16));

        m_motors.at(__address)->m_communicationState =FanCommunicationState::m_error;
    } else {
        qDebug() << reply->errorString();
        ui->textBrowser->append(tr("Read response error: %1 (code: 0x%2)").
                                arg(reply->errorString()).arg(reply->error(), -1, 16));

        m_motors.at(__address)->m_communicationState =FanCommunicationState::m_disconnect;

    }

    if(m_currentServerAddress == __currentAddress){

        if(m_pollingState == PollingState::MultiMotor){//multi motor polling...

            qDebug()<<"multi motor polling...";
            for(m_currentServerAddress++;m_currentServerAddress < m_motorNum + m_startServerAddress; \
                m_currentServerAddress++){
                if(m_motors.at(m_currentServerAddress-m_startServerAddress)->m_communicationState == \
                        FanCommunicationState::m_connect){
                    sendOnePolling(m_currentServerAddress);
                    break;
                }
            }

        }
        else if(m_pollingState == PollingState::Searching){//searching...

            qDebug()<<"searching...";
            for(m_currentServerAddress++;m_currentServerAddress < m_motorNum + m_startServerAddress;\
                m_currentServerAddress++){
                sendOnePolling(m_currentServerAddress);
                break;
            }
            if(m_currentServerAddress >= m_motorNum + m_startServerAddress){
                m_pollingState = PollingState::Stop;
            }

        }
    }

    m_motors.at(__address)->update();

    if(m_realTimeServerAddress == __currentAddress){
        ui->lamp_comState->setLampState(m_motors.at(__address)->m_commLamp->getLampState());
        ui->lamp_runState->setLampState(m_motors.at(__address)->m_runLamp->getLampState());
        ui->lcdNumber_targetPower->display(m_motors.at(__address)->m_motorController.m_targetpower);
        ui->lcdNumber_nowPower->display(m_motors.at(__address)->m_motorController.m_nowpower);
        ui->lcdNumber_speedRef->display(m_motors.at(__address)->m_motorController.m_speedRef);
        ui->lcdNumber_speedFbk->display(m_motors.at(__address)->m_motorController.m_speedFbk);

        if(m_pollingState == PollingState::Stop){
            QAbstractItemModel *__model = ui->table_settings->model();
            quint16 *__buffPtr = (quint16 *)&m_motors.at(__address)->m_initSetttings;
            for(unsigned char i = 0; i<3; i++){
                for(unsigned char j = 1; j<5; j++){
                    __model->setData(__model->index(i,j), *__buffPtr);
                    __buffPtr++;
                }
            }
        }
        else if(m_pollingState == PollingState::SingleMotor){
            emit updatePlotUi(m_motors.at(__address)->m_motorController);
            qDebug()<<"emit.....";
        }
    }

    reply->deleteLater();
}


QModbusDataUnit FanMotorUi::writeRequest() const
{
    const QModbusDataUnit::RegisterType table = QModbusDataUnit::HoldingRegisters;

    int startAddress = g_mSettingsRegisterAddress;

    int numberOfEntries = sizeof(FanMotorSettings) >> 1;
    return QModbusDataUnit(table, startAddress, numberOfEntries);
}

void FanMotorUi::on_pushButton_write_clicked()//write settings
{
    if (!modbusDevice)
        return;

    QAbstractItemModel *__model = ui->table_settings->model();
    int __address = ui->spinBox_currentaddress->value()-m_startServerAddress;
    quint16 *buff = (quint16 *)&m_motors.at(__address)->m_initSetttings;

    //get data from tableview
    for(unsigned char i = 0; i<3; i++){
        for(unsigned char j = 1; j<5; j++){
            *buff = (quint16)__model->data(__model->index(i, j)).toInt();
            buff++;
        }
    }

    QModbusDataUnit writeUnit = writeRequest();
    buff = (quint16 *)&m_motors.at(__address)->m_initSetttings;

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
    //Searching mode start
    m_pollingState = PollingState::Searching;
    if(m_pollingTimer->isActive()){
        m_pollingTimer->stop();
        ui->startButton->setText(QStringLiteral("   start   "));
    }
    m_currentServerAddress = m_startServerAddress;
    sendOnePolling(m_currentServerAddress);
}

void FanMotorUi::on_startButton_clicked()
{
    //Multi motor mode start
    if(!m_pollingTimer->isActive()){

        if(ui->radioButton_signleMotorMode->isChecked()){
            m_pollingState = PollingState::SingleMotor;
            sendOnePolling(ui->spinBox_currentaddress->value());
            m_pollingTimer->start(ui->spinBox_pollingPeriod->value());
        }
        else{
            m_pollingState = PollingState::MultiMotor;

            m_currentServerAddress = m_startServerAddress;
            for(m_currentServerAddress;m_currentServerAddress < m_motorNum + m_startServerAddress; \
                m_currentServerAddress++){
                if(m_motors.at(m_currentServerAddress-m_startServerAddress)->m_communicationState == \
                        FanCommunicationState::m_connect){
                    sendOnePolling(m_currentServerAddress);
                    break;
                }
            }
            m_pollingTimer->start(ui->spinBox_timePeriod->value());
        }

        ui->startButton->setText(QStringLiteral("   stop   "));
    }
    else{
        m_pollingState = PollingState::Stop;
        m_pollingTimer->stop();
        ui->startButton->setText(QStringLiteral("   start   "));
    }

}

void FanMotorUi::pollingTimerUpdate()
{
    qDebug()<<"timer update";

    if (!modbusDevice){
        return;
    }

    if(m_pollingState == PollingState::SingleMotor){
        sendOnePolling(ui->spinBox_currentaddress->value());
    }
    else{
        m_currentServerAddress = m_startServerAddress;
        for(m_currentServerAddress;m_currentServerAddress < m_motorNum + m_startServerAddress; \
            m_currentServerAddress++){
            if(m_motors.at(m_currentServerAddress-m_startServerAddress)->m_communicationState == \
                    FanCommunicationState::m_connect){
                sendOnePolling(m_currentServerAddress);
                break;
            }
        }
    }

}

void FanMotorUi::sendOnePolling(int address)
{
    qDebug()<<"send one polling";
    if (!modbusDevice){
        return;
    }

    int startAddress;

    int numberOfEntries;

    if(m_pollingState == PollingState::MultiMotor){
        startAddress = g_mRealTimeRegisterAddress;
        numberOfEntries  = g_mRealTimeRegisterCount;
    }
    else if(m_pollingState == PollingState::Searching){
        startAddress = g_mControllerRegisterAddress;
        numberOfEntries  = g_mRatedRegisterCount;
    }
    else{//single motor mode
        startAddress = g_mRealTimeRegisterAddress;
        numberOfEntries  = g_mRealTimeRegisterMoreCount;
    }

    QModbusDataUnit __dataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);

    if (auto *reply = modbusDevice->sendReadRequest(__dataUnit, address)) {
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, &FanMotorUi::readReady);
        }
        else
            delete reply; // broadcast replies return immediately
    } else {
        qDebug() << "error:" <<modbusDevice->errorString() ;
        ui->textBrowser->append(tr("error: ") + modbusDevice->errorString());
    }
}

void FanMotorUi::on_spinBox_motorNum_valueChanged(int arg1)
{
    m_motorNum = arg1;

    while(m_motors.count() < m_motorNum){
        int __motorCount = m_motors.count();
        m_motors.push_back(new QMotor(__motorCount+m_startServerAddress));
        ui->table_realtime->setCellWidget(__motorCount, 0, m_motors.back()->m_motorAddressLabel);
        ui->table_realtime->setCellWidget(__motorCount, 1, m_motors.back()->m_ratedPowerLCD);
        ui->table_realtime->setCellWidget(__motorCount, 2, m_motors.back()->m_targetPowerLCD);
        ui->table_realtime->setCellWidget(__motorCount, 3, m_motors.back()->m_nowPowerLCD);
        ui->table_realtime->setCellWidget(__motorCount, 4, m_motors.back()->m_ratedSpeedLCD);
        ui->table_realtime->setCellWidget(__motorCount, 5, m_motors.back()->m_speedRefLCD);
        ui->table_realtime->setCellWidget(__motorCount, 6, m_motors.back()->m_speedFbkLCD);
        ui->table_realtime->setCellWidget(__motorCount, 7, m_motors.back()->m_runLamp);
        ui->table_realtime->setCellWidget(__motorCount, 8, m_motors.back()->m_commLamp);
    }

    while(m_motors.count() > m_motorNum){
        int __motorCount = m_motors.count()-1;
        ui->table_realtime->removeCellWidget(__motorCount, 0);
        ui->table_realtime->removeCellWidget(__motorCount, 1);
        ui->table_realtime->removeCellWidget(__motorCount, 2);
        ui->table_realtime->removeCellWidget(__motorCount, 3);
        ui->table_realtime->removeCellWidget(__motorCount, 4);
        ui->table_realtime->removeCellWidget(__motorCount, 5);
        ui->table_realtime->removeCellWidget(__motorCount, 6);
        ui->table_realtime->removeCellWidget(__motorCount, 7);
        ui->table_realtime->removeCellWidget(__motorCount, 8);
        m_motors.pop_back();

    }
}

void FanMotorUi::on_spinBox_currentaddress_valueChanged(int arg1)
{
    m_realTimeServerAddress = arg1;
}

