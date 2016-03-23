#ifndef CANUI_H
#define CANUI_H

#include "candriver/can.h"
#include <QFrame>
#include "thread/canthread.h"

namespace Ui {
class CANUi;
}

class CANUi : public QFrame
{
    Q_OBJECT

public:
    explicit CANUi(QWidget *parent = 0);
    ~CANUi();

    static CANUi *getS_Instance();

private slots:
    void on_pushButton_Open_clicked();

    void on_pushButton_Close_clicked();

    void on_pushButton_Clear_clicked();

    void messageShow(const QString &s);

    void on_pushButton_Send_clicked();

private:
    Ui::CANUi *ui;
    static CANUi *s_Instance;
    s_BOARD *m_masterBoard;
    CO_Data *m_masterData;
    CanThread *m_canThread;

private:
    void initData();
    void initUI();
    void initThread();
    void initConnect();

};

#endif // CANUI_H
