#ifndef QMOTOR_H
#define QMOTOR_H

typedef signed char qint8;         // 8 bit signed
typedef unsigned char quint8;      // 8 bit unsigned
typedef short qint16;              // 16 bit signed
typedef unsigned short quint16;    // 16 bit unsigned
typedef int qint32;                // 32 bit signed
typedef unsigned int quint32;      // 32 bit unsigned


/********************
 *
 *******************/
#pragma pack(push)
#pragma pack(1)
struct QPIParameters{
    float  m_kp;
    float  m_ki;
};
#pragma pack(pop)

/********************
 *
 *******************/
#pragma pack(push)
#pragma pack(1)
struct QMotorSettings{
    quint16 m_dryHighPower:16;    //2 byte
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
    quint16 m_wetLowSpeed:16;     //24 byte

    quint16 m_motorType:4;
    quint16 m_staticPressure:2;
    quint16 m_regulation:2;       //25 byte
    quint16 m_control:1;
    quint16 m_address:7;          //26 byte

    QPIParameters  m_speedPI;     //30 byte
    QPIParameters  m_idPI;        //34 byte
    QPIParameters  m_iqPI;        //38 byte
};
#pragma pack(pop)

enum class QMotorState{
    m_stop = 0,
    m_error,
    m_run,
    m_stall,   // 堵转状态
    m_overSpd
};

/********************
 *
 *******************/
#pragma pack(push)
#pragma pack(1)
struct QMotorController{
    quint8 m_poles;           // 电机极性
    quint16 m_ratedPower;     // 额定功率
    quint16 m_ratedSpeedRPM;  // 额定转速 in RPM Format
    quint16 m_speedRPM;       // 实时转速 in RPM Format
    quint16 m_targetpower;    // 实时目标功率
    quint16 m_nowpower;       // 实时实际功率
    qint16 m_speedRpmRef;     // motor speed command value
    qint32 m_speedfbk;        // speed feedback
    qint32 m_speedRef; 	      // speed reference set-point
    QMotorState m_runState;	  // Motor run state
};
#pragma pack(pop)

/********************
 *
 *******************/
#pragma pack(push)
#pragma pack(1)
struct QPIController{
    qint32  m_ref;   		// reference set-point
    qint32  m_fbk;   		// feedback
    qint32  m_out;   		// Output: controller output
    QPIParameters m_PI;     // PI parameters
    qint32  m_pOut;			// proportional term
    qint32  m_iOut;			// integral term
};
#pragma pack(pop)


/********************
 *
 *******************/
#pragma pack(push)
#pragma pack(1)
struct QMotor{
    QMotorSettings m_initSetttings;
    QMotorController m_motorController;
    QPIController m_speedPIController;
    QPIController m_idPIController;
    QPIController m_iqPIController;
};
#pragma pack(pop)



#endif // QMOTOR_H
