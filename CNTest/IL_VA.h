//7/10/2013
//huskier
//IL_VA.h
//VA
//

#ifndef IL_VA_H
#define IL_VA_H

#include "IL_types.h"
#include "IL_net_struct.h"

namespace gCMD
{
    //VA unit
    //0x8000~0x8FFF
    const int VA_INIT = 0x8000;
    const int VA_SET_TRANSMISSIVITY = 0x8001;
    const int VA_GET_STATUS = 0x8002;
    const int VA_CALIBRATION = 0x8003;
    const int VA_SET_ANGLE = 0x8004;
}

/*VA电机状态信息*/
#pragma pack(4)
typedef struct
{
    float motor_velocity;       /*电机速度*/
    float motor_position;       /*电机实际位置*/
    float motor_pos_error;      /*电机跟随误差*/
    float motor_current;        /*电机电流*/
    int   power_flag;           /*电机功率达到90% Max.Power标志*/
    int   motor_fault;          /*电机出现错误，不工作*/
}VA_MOTOR_STATUS_STRUCT;
#pragma pack()

/*VA-FPGA上传给VG5的完整状态信息*/
#pragma pack(4)
typedef struct
{
    VA_MOTOR_STATUS_STRUCT VA_motor_status;        /*VA电机状态信息，包括上面6个指标*/
    float           VA_act_transmission;    /*VA实际透过率*/
}VA_STATUS_STRUCT;
#pragma pack()

class VA
{
public:
    VA();

    //STATUS initialize(void *p_data);
//    STATUS set_trans(void *p_data);
//    STATUS set_angle(void *p_data);
//    STATUS get_status(void *p_data);
//    STATUS calibration(void *p_data);
    //STATUS terminate(void *p_data);

public:
    STATUS initialize(void);
 //   STATUS set_trans(float trans);
    STATUS set_angle(float angle);   //单位是度
 //   STATUS get_status(VA_STATUS_STRUCT * pVA_status);
 //   STATUS calibration(void);
    //STATUS terminate(void);

};

#endif // IL_VA_H
