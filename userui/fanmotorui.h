#ifndef FANMOTORUI_H
#define FANMOTORUI_H

#include <QWidget>
#include "modbusui.h"
#include <QModbusDataUnit>

#include"qmotor/qmotor.h"

QT_BEGIN_NAMESPACE
class QModbusClient;
class QModbusReply;
class QTimer;

namespace Ui {
class FanMotorUi;
}
QT_END_NAMESPACE


class FanMotorUi : public QWidget
{
    Q_OBJECT

public:
    explicit FanMotorUi(QWidget *parent = 0);
    ~FanMotorUi();

private slots:
    void pollingTimerUpdate();

    void onStateChanged(int state);

    void on_pushButton_read_clicked();
    void readReady();

    void on_pushButton_write_clicked();

    void on_startButton_clicked();

    void on_stopButton_clicked();

private:
    QModbusDataUnit readRequest() const;
    QModbusDataUnit writeRequest() const;

private:
    Ui::FanMotorUi *ui;
    ModbusUi *m_modbusUi;
    QModbusClient* modbusDevice;
    QVector<QMotor*> m_motor;
    QTimer *m_pollingTimer;
    int m_currentServerAddress;

    void sendOnePolling(int address);
};

#endif // FANMOTORUI_H
