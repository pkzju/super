﻿#ifndef FANMOTOR_H
#define FANMOTOR_H

typedef signed char qint8;         // 8 bit signed
typedef unsigned char quint8;      // 8 bit unsigned
typedef short qint16;              // 16 bit signed
typedef unsigned short quint16;    // 16 bit unsigned
typedef int qint32;                // 32 bit signed
typedef unsigned int quint32;      // 32 bit unsigned


enum class FanMotorError : char{
    m_noError = 0,
    m_overCur,
    m_overSpd,
    m_stall,
    m_lowVolt,
    m_highVolt
};

enum class FanMotorState : char{
    m_stop = 0,
    m_error,
    m_run
};

enum class FanCommunicationState : char{
    m_disconnect = 0,
    m_connect,
    m_error
};


const quint16 g_mSettingsRegisterAddress = 0x0040;
const quint16 g_mControllerRegisterAddress = 0x0060;
const int g_mRatedRegisterCount = 2;
const quint16 g_mRealTimeRegisterAddress = 0x0062;
const int g_mRealTimeRegisterCount = 5;
const int g_mRealTimeRegisterMoreCount = 9;
const quint16 g_mPIParaRegisterAddress = 0x0060;
/********************
 *
 *******************/
#pragma pack(push)
#pragma pack(1)
struct FanMotorSettings{
    quint16 m_dryHighPower:16;    //2 byte  address:0x0040
    quint16 m_dryHighSpeed:16;    //4 byte
    quint16 m_wetHighPower:16;    //6 byte
    quint16 m_wetHighSpeed:16;    //8 byte

    quint16 m_dryMiddlePower:16;  //10 byte
    quint16 m_dryMiddleSpeed:16;  //12 byte
    quint16 m_wetMiddlePower:16;  //14 byte
    quint16 m_wetMiddleSpeed:16;  //16 byte

    quint16 m_dryLowPower:16;     //18 byte
    quint16 m_dryLowSpeed:16;     //20 byte
    quint16 m_wetLowPower:16;     //22 byte
    quint16 m_wetLowSpeed:16;     //24 byte  0x004b

    quint16 m_motorType:4;
    quint16 m_staticPressure:2;
    quint16 m_regulation:2;       //25 byte
    quint16 m_control:1;
    quint16 m_address:7;          //26 byte  0x004c
};
#pragma pack(pop)

/********************
 *
 *******************/
#pragma pack(push)
#pragma pack(1)
struct FanPIParameters{
    quint16  m_speedKp;    // x 1000
    quint16  m_speedKi;    // x 1000
    quint16  m_idKp;       // x 1000
    quint16  m_idKi;       // x 1000
    quint16  m_iqKp;       // x 1000
    quint16  m_iqKi;       // x 1000
};
#pragma pack(pop)

/********************
 *
 *******************/
#pragma pack(push)
#pragma pack(1)
struct FanPIController{
    FanPIParameters m_PI;     // PI parameters
    qint16  m_ref;   		// reference set-point
    qint16  m_fbk;   		// feedback
    qint16  m_out;   		// Output: controller output
};
#pragma pack(pop)

/********************
 *
 *******************/
#pragma pack(push)
#pragma pack(1)
struct FanMotorController{
    quint16 m_ratedPower;     // 额定功率    address:0x0060
    quint16 m_ratedSpeed;     // 额定转速

    quint16 m_targetpower;    // 实时目标功率  address:0x0062
    quint16 m_nowpower;       // 实时实际功率  address:0x0063
    qint16 m_speedRef;        //                     0x0064
    qint16 m_speedFbk;

    FanMotorState m_runState;	  // Motor run state
    FanMotorError m_runError;	  // Motor run Error

    qint16 m_idRef;
    qint16 m_idFbk;
    qint16 m_iqRef;
    qint16 m_iqFbk;

};
#pragma pack(pop)



/********************
 *
 *******************/
#pragma pack(push)
#pragma pack(1)
struct FanMotor{
    FanMotorSettings m_initSetttings;     //0x0040
    FanMotorController m_motorController; //0x0060
    FanPIParameters m_PIPara;             //0x0080
    FanCommunicationState m_communicationState;
};
#pragma pack(pop)



#endif // QMOTOR_H
