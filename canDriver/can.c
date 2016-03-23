/*
 CAN driver interface.
*/

#include <windows.h>
#include "can.h"



// GetProcAddress doesn't have an UNICODE version for NT
#ifdef UNDER_CE
  #define myTEXT(str) TEXT(str)
#else
  #define myTEXT(str) str
#endif

#define MAX_NB_CAN_PORTS 16



CANPort canports[MAX_NB_CAN_PORTS] = {{0,},{0,},{0,},{0,},{0,},{0,},{0,},{0,},{0,},{0,},{0,},{0,},{0,},{0,},{0,},{0,}};


/***************************************************************************/
UNS8 usbCanReceive(void* inst, Message *m)
{
    UNS8 ret;
    VCI_CAN_OBJ vco = {0};
    VCI_ERR_INFO errinfo;
    ret = (UNS8)VCI_Receive(VCI_USBCAN2, 0, 0, &vco, 1, 1000);
    if(ret > 0)
    {
        m->cob_id = vco.ID;
        m->rtr = vco.RemoteFlag;
        m->len = vco.DataLen;
        for (int i = 0 ; i < vco.DataLen ; i++) {
           m->data[i] = vco.Data[i];
        }
    }
    else
    {
        VCI_ReadErrInfo(VCI_USBCAN2, 0, 1,&errinfo);
    }
    return ret;
}

UNS8 usbCanSend(void* inst, const Message *m)
{
    ULONG ret;
    VCI_CAN_OBJ pSend;
    pSend.ID = m->cob_id;
    pSend.RemoteFlag = m->rtr;
    pSend.ExternFlag = 0;
    pSend.DataLen = m->len;
    for (int i = 0 ; i < m->len ; i++) {
       pSend.Data[i] = m->data[i];
    }
    ret = VCI_Transmit(VCI_USBCAN2,0,1,&pSend,1);

    return (UNS8)ret;
}

CAN_HANDLE usbCanOpen(s_BOARD *board)
{
    VCI_INIT_CONFIG init_config;
    init_config.AccCode = 0;
    init_config.AccMask = 0xffffffff;  //all pass
    init_config.Filter = 1;  //one filter
    init_config.Mode = 0;    //normel mode
    init_config.Timing0 = 0x03;
    init_config.Timing1 = 0x1c; //baud rate set 125kbps
    VCI_INIT_CONFIG init_config1;
    init_config1.AccCode = 0;
    init_config1.AccMask = 0xffffffff;  //all pass
    init_config1.Filter = 1;  //one filter
    init_config1.Mode = 0;    //normel mode
    init_config1.Timing0 = 0x03;
    init_config1.Timing1 = 0x1c; //baud rate set 125kbps

    if(VCI_OpenDevice(VCI_USBCAN2,0,0) != STATUS_OK)
    {
        return NULL;
    }
    else
    {
        if(VCI_InitCAN(VCI_USBCAN2,0,0,&init_config) != STATUS_OK)
        {
            VCI_CloseDevice(VCI_USBCAN2,0);
            return NULL;
        }
        else
        {
            VCI_InitCAN(VCI_USBCAN2,0,1,&init_config1);
            if(VCI_StartCAN(VCI_USBCAN2, 0, 0) == 1)
            {
                VCI_StartCAN(VCI_USBCAN2, 0, 1);
                return (CAN_HANDLE)board;
            }
            else
                return NULL;
        }
    }
}

int usbCanClose(s_BOARD *board)
{
    int ret;
    ret = VCI_ResetCAN(VCI_USBCAN2, 0, 0);
    if(ret)
        return VCI_CloseDevice(VCI_USBCAN2,0);
    else
        return ret;
}

UNS8 usbCanChangeBaudrate(void* fd, char* baud)
{
    int ret;
    if(baud = "125")
        ret = VCI_SetReference(VCI_USBCAN2, 0, 0, 0, (PVOID)0x1C0011);
    return ret;
}


/***************************************************************************/
UNS8 canSend(CAN_PORT port, Message *m)
{
    if (port && (usbCanSend != NULL))
	{
        return usbCanSend(((CANPort*)port)->fd, m);
	}
	return 1; /* NOT OK */	
}



/***************************************************************************/
DWORD canReceiveLoop(CAN_PORT port)
{
	Message m;
	while(((CANPort*)port)->used)
	{
        if(usbCanReceive(((CANPort*)port)->fd, &m) != 0)
            break;
	}
	return 0;
}

/***************************************************************************/
CAN_PORT canOpen(s_BOARD *board, CO_Data * d)
{
	int i;
	CAN_HANDLE fd0;


	  /* Fix of multiple opening one data set, added by J.Fojtik. */
	if(d->canHandle)
	{
	  canClose(d);
	}

	for(i=0; i < MAX_NB_CAN_PORTS; i++)
	{
		if(!canports[i].used)
		break;
	}

    fd0 = usbCanOpen(board);
	if(fd0)
    {
		canports[i].used = 1;
		canports[i].fd = fd0;
		canports[i].d = d;
		d->canHandle = (CAN_PORT)&canports[i];
     //   CreateReceiveTask(&(canports[i]), &canports[i].receiveTask, (void *)canReceiveLoop);
		return (CAN_PORT)&canports[i];
	}
	else
	{
//		MSG("CanOpen : Cannot open board {busname='%S',baudrate='%S'}\n",board->busname, board->baudrate);
		return NULL;
	}
}

/***************************************************************************/
int canClose(CO_Data * d)
{
    int res = 0;
    CANPort* port = (CANPort*)d->canHandle;
    if(port){
        ((CANPort*)d->canHandle)->used = 0;

        res = usbCanClose(port->fd);

     //   WaitReceiveTaskEnd(&port->receiveTask);

        d->canHandle = NULL;
    }

    return res;
}

UNS8 canChangeBaudRate(CAN_PORT port, char* baud)
{
   if(port){
		UNS8 res;
	    //LeaveMutex();
        res = usbCanChangeBaudrate(((CANPort*)port)->fd, baud);
		//EnterMutex();
		return res; // OK
	}
	return 1; // NOT OK
}



