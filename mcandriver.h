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

#ifndef MCANDRIVER
#define MCANDRIVER

#ifdef __cplusplus
extern "C" {
#endif

#include "canOpen/include/can_driver.h"
#include "canOpen/include/data.h"
#include "canOpen/include/timers_driver.h"

UNS8 usbCanReceive(void* inst, Message *m);
UNS8 usbCanSend(void* inst, const Message *m);
CAN_HANDLE usbCanOpen(s_BOARD *board);
int usbCanClose(s_BOARD *board);
UNS8 usbCanChangeBaudrate(void* fd, char* baud);

UNS8 canSend(CAN_PORT port, Message *m);
CAN_PORT canOpen(s_BOARD *board, CO_Data * d);
int canClose(CO_Data * d);
UNS8 canChangeBaudRate(CAN_PORT port, char* baud);

#ifdef __cplusplus
};

#endif

#endif // MCANDRIVER

