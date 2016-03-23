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

#ifndef __can_h__
#define __can_h__

#include "can.h"
#include "applicfg.h"
#include "ControlCAN.H"
#pragma comment(lib,"ControlCAN.lib")

/**
 * @brief The CAN message structure
 * @ingroup can
 */
typedef struct {
  UNS16 cob_id;	/**< message's ID */
  UNS8 rtr;		/**< remote transmission request. (0 if not rtr message, 1 if rtr message) */
  UNS8 len;		/**< message's length (0 to 8) */
  UNS8 data[8]; /**< message's datas */
} Message;

#define Message_Initializer {0,0,0,{0,0,0,0,0,0,0,0}}

typedef UNS8 (*canSend_t)(Message *);

struct struct_s_BOARD;

typedef struct struct_s_BOARD s_BOARD;

struct struct_s_BOARD {
  char * busname;  /**< The bus name on which the CAN board is connected */
  char * baudrate; /**< The board baudrate */
};

typedef struct struct_CO_Data CO_Data;

struct struct_CO_Data {
    /* Object dictionary */
    UNS8 *bDeviceNodeId;
    const UNS8 *iam_a_slave;

    /* General */
    UNS8 toggle;
    CAN_PORT canHandle;

};

/* CAN port structure */
typedef struct
{
  char used;  /**< flag indicating CAN port usage, will be used to abort Receiver task*/
  CAN_HANDLE fd; /**< CAN port file descriptor*/
  CO_Data* d; /**< CAN object data*/
}CANPort;

/**
 *2016.2.19
 */
#ifdef __cplusplus
extern "C" {
#endif

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



#endif /* __can_h__ */
