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

#ifndef CANOPERATE_H
#define CANOPERATE_H

#include <QWidget>
#include "QFramer/fbasepushbutton.h"
#include "QFramer/fsetpushbutton.h"
#include "QFramer/ftoolbutton.h"
#include <QLayout>

namespace Ui {
class CANOperate;
}

class CANOperate : public QWidget
{
    Q_OBJECT

public:
    explicit CANOperate(QWidget *parent = 0);
    ~CANOperate();
    static CANOperate *GetInstance();

private:
    Ui::CANOperate *ui;
    QGridLayout *m_GLayout;
    FBasePushButton *m_Button_Open;
    FToolButton *m_Button_Close;
    static CANOperate *s_Instance;
};

#endif // CANOPERATE_H
