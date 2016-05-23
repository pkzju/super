#ifndef FANMOTORUI_H
#define FANMOTORUI_H

#include <QWidget>
#include "modbusui.h"
#include <QModbusDataUnit>
#include "fanmotor/fanmotor.h"

QT_BEGIN_NAMESPACE
class QModbusClient;
class QModbusReply;
class QTimer;

namespace Ui {
class FanMotorUi;
}
QT_END_NAMESPACE

class QcwIndicatorLamp;
class QMotor;
struct FanMotorController;

class FanMotorUi : public QWidget
{
    Q_OBJECT

public:
    explicit FanMotorUi(QWidget *parent = 0);
    ~FanMotorUi();

public:
    enum PollingState:char{
        Stop,
        SingleMotor,
        MultiMotor,
        Searching,
    };

private slots:
    void pollingTimerUpdate();

    void onStateChanged(int state);

    void on_pushButton_read_clicked();

    void readReady();

    void on_pushButton_write_clicked();

    void on_startButton_clicked();

    void on_searchButton_clicked();

    void on_spinBox_motorNum_valueChanged(int arg1);

    void on_spinBox_currentaddress_valueChanged(int arg1);

private:
    QModbusDataUnit readRequest() const;
    QModbusDataUnit writeRequest() const;

signals:
    void updatePlotUi(FanMotorController motorctr);

private:
    Ui::FanMotorUi *ui;
    ModbusUi *m_modbusUi;
    QModbusClient* modbusDevice;
    QVector<QMotor*> m_motors;
    QTimer *m_pollingTimer;
    int m_realTimeServerAddress;
    int m_currentServerAddress;
    int m_startServerAddress;
    int m_motorNum;
    PollingState m_pollingState;

    void sendOnePolling(int address);

};

#endif // FANMOTORUI_H
