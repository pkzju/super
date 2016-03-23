/****************************************************************************
**
** Copyright (C) 2014 dragondjf
**
** QFramer is a frame based on Qt5.3, you will be more efficient with it. 
** As an Qter, Qt give us a nice coding experience. With user interactive experience(UE) 
** become more and more important in modern software, deveployers should consider business and UE.
** So, QFramer is born. QFramer's goal is to be a mature solution 
** which you only need to be focus on your business but UE for all Qters.
**
** Version	: 0.2.5.0
** Author	: dragondjf
** Website	: https://github.com/dragondjf
** Project	: https://github.com/dragondjf/QCFramer
** Blog		: http://my.oschina.net/dragondjf/home/?ft=atme
** Wiki		: https://github.com/dragondjf/QCFramer/wiki
** Lincence: LGPL V2
** QQ: 465398889
** Email: dragondjf@gmail.com, ding465398889@163.com, 465398889@qq.com
** 
****************************************************************************/

#ifndef CONSOLEPART0_H
#define CONSOLEPART0_H

#include <QWidget>
#include "lamp/qcw_indicatorlamp.h"
#include "console/console.h"
#include "QFramer/fsetpushbutton.h"
#include <qtimer>



namespace Ui {
class consolepart0;
}

class consolepart0 : public QWidget
{
    Q_OBJECT

public:
    explicit consolepart0(QWidget *parent = 0);
    ~consolepart0();

private:
    void initData();
    void initUI();
    void initThread();
    void initConnect();

private:
    Ui::consolepart0 *ui;
    Console *console;
    QcwIndicatorLamp *lamp;
    FSetPushButton *connectButton;
    FSetPushButton *disConnectButton;
    FSetPushButton *clearButton;
    QString consoleString;
    QTimer *uiTimer;

private slots:
    void onUiTimerTimeOut();
    void messageShow(const QString &s);


};

#endif // CONSOLEPART0_H
