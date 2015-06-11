//7/10/2013
//huskier
//IL_VA.cpp
//VA
//

#include "IL_VA.h"
#include "IL_global.h"
#include "glog/logging.h"
#include <arpa/inet.h>

#include "util.h"

//extern UDPServer vx_Net;

VA::VA()
{
    LOG(INFO)<<"initialize VA::VA()......";
}

/*
STATUS VA::initialize(void *p_data)
{
    return initialize();
}
*/

/*
STATUS VA::set_trans(void *p_data)
{
    return set_trans(*(float *)p_data);
}

STATUS VA::set_angle(void *p_data)
{
    return set_angle(*(float *)p_data);
}

STATUS VA::get_status(void *p_data)
{
    return get_status((VA_STATUS_STRUCT *)p_data);
}

STATUS VA::calibration(void *p_data)
{
    return calibration();
}

*/


STATUS VA::initialize()
{
    //local variables definition......
    time_t wait_seconds = 20;
    NET_PACKET_STRUCT net_pack;
    memset(&net_pack, 0, sizeof(NET_PACKET_STRUCT));

    ushort send_data_len = 0;

    net_pack.head_union.head.cmd = gCMD::VA_INIT;
    net_pack.head_union.head.total_length = NET_PACKET_HEAD_LEN + send_data_len;
    net_pack.head_union.head.cnt = htons(0);
    net_pack.head_union.head.check = htons(0);
    net_pack.head_union.head.priority = htons(0);
    net_pack.head_union.head.status = htons(0);
    net_pack.p_data = NULL;

    ggNet::vx_Net.pack_post(net_pack);

    memset(&net_pack, 0, sizeof(NET_PACKET_STRUCT));
    //int status = ggNet::vx_Net.wait_data(gCMD::VA_INIT, wait_seconds, net_pack);

    int status = OK;


    if(status != OK)
    {
        LOG(INFO)<<"VA::initialize command status is "<<status;
        return status;
    }

    if(net_pack.head_union.head.status == OK)
    {
        LOG(INFO)<<"VA initialize successful......";
    }
    else
    {
        LOG(ERROR)<<"VA initialize error......";
    }

    if(net_pack.p_data != NULL)
    {
        delete[] net_pack.p_data;
    }
    return net_pack.head_union.head.status;
}

/*
STATUS VA::set_trans(float trans)
{
    //local variables definition......
    time_t wait_seconds = 100;
    NET_PACKET_STRUCT net_pack;
    memset(&net_pack, 0, sizeof(NET_PACKET_STRUCT));

    //preparing data.........
    LOG(INFO)<<"In VA::set_trans(...), trans = "<<trans;
    int send_data_len = sizeof(trans);
    char *p_send_data = new char[send_data_len];
    trans = htonf(trans);
    memcpy(p_send_data, &trans, send_data_len);
    //memcpy(p_send_data, "abc", 4);

    gNet::pack_post(gCMD::VA_SET_TRANSMISSIVITY, NET_PACKET_HEAD_LEN + send_data_len, 0, 0, 0, 0, p_send_data);

    //waiting data from net......
    int status = gNet::wait_data_from_net(gCMD::VA_SET_TRANSMISSIVITY, wait_seconds, net_pack);

    if(status != OK)
    {
        return status;
    }

    if(net_pack.head_union.head.status == OK)
    {
        LOG(INFO)<<"VA set transmission successful......";
    }
    else
    {
        LOG(ERROR)<<"VA set transmission error......";
    }

    if(net_pack.p_data != NULL)
    {
        delete[] net_pack.p_data;
    }
    return net_pack.head_union.head.status;
}

STATUS VA::get_status(VA_STATUS_STRUCT * pVA_status)
{
    //local variables definition......
    time_t wait_seconds = 100;
    NET_PACKET_STRUCT net_pack;
    memset(&net_pack, 0, sizeof(NET_PACKET_STRUCT));

    ushort send_data_len = 0;

    gNet::pack_post(gCMD::VA_GET_STATUS, NET_PACKET_HEAD_LEN + send_data_len, 0, 0, 0, 0, NULL);

    //waiting data from net......
    int status = gNet::wait_data_from_net(gCMD::VA_GET_STATUS, wait_seconds, net_pack);

    if(status != OK)
    {
        return status;
    }

    if(net_pack.head_union.head.status == OK)
    {
        LOG(INFO)<<"VA get status successful......";
        //接下来是对status结构体做解析......

        memcpy(pVA_status, net_pack.p_data, sizeof(VA_STATUS_STRUCT));

        pVA_status->VA_motor_status.motor_current = ntohf(pVA_status->VA_motor_status.motor_current);
        pVA_status->VA_motor_status.motor_position = ntohf(pVA_status->VA_motor_status.motor_position);
        pVA_status->VA_motor_status.motor_pos_error = ntohf(pVA_status->VA_motor_status.motor_pos_error);
        pVA_status->VA_motor_status.motor_velocity = ntohf(pVA_status->VA_motor_status.motor_velocity);
        pVA_status->VA_motor_status.motor_fault = ntohl(pVA_status->VA_motor_status.motor_fault);
        pVA_status->VA_motor_status.power_flag = ntohl(pVA_status->VA_motor_status.power_flag);
        pVA_status->VA_act_transmission = ntohf(pVA_status->VA_act_transmission);
    }
    else
    {
        LOG(ERROR)<<"VA get status error......";
    }

    if(net_pack.p_data != NULL)
    {
        delete[] net_pack.p_data;
    }
    return net_pack.head_union.head.status;
}

STATUS VA::calibration()
{
    //local variables definition......
    time_t wait_seconds = 100;
    NET_PACKET_STRUCT net_pack;
    memset(&net_pack, 0, sizeof(NET_PACKET_STRUCT));

    ushort send_data_len = 0;

    gNet::pack_post(gCMD::VA_CALIBRATION, NET_PACKET_HEAD_LEN + send_data_len, 0, 0, 0, 0, NULL);

    //waiting data from net......
    int status = gNet::wait_data_from_net(gCMD::VA_CALIBRATION, wait_seconds, net_pack);

    if(status != OK)
    {
        return status;
    }

    if(net_pack.head_union.head.status == OK)
    {
        LOG(INFO)<<"VA calibration successful......";
    }
    else
    {
        LOG(ERROR)<<"VA calibration error......";
    }

    if(net_pack.p_data != NULL)
    {
        delete[] net_pack.p_data;
    }
    return net_pack.head_union.head.status;
}
*/

STATUS VA::set_angle(float angle)//单位是度
{
    //local variables definition......
    time_t wait_seconds = 20;
    NET_PACKET_STRUCT net_pack;
    memset(&net_pack, 0, sizeof(NET_PACKET_STRUCT));

    //preparing data.........
    //LOG(INFO)<<"In VA::set_angle(...), angle = "<<angle;

    int send_data_len = sizeof(angle);
    char *p_send_data = new char[send_data_len];
    angle = htonf(angle);
    memcpy(p_send_data, &angle, send_data_len);
    //memcpy(p_send_data, "abc", 4);

    net_pack.head_union.head.cmd = gCMD::VA_SET_ANGLE;
    net_pack.head_union.head.total_length = NET_PACKET_HEAD_LEN + send_data_len;
    net_pack.head_union.head.cnt = htons(0);
    net_pack.head_union.head.check = htons(0);
    net_pack.head_union.head.priority = htons(0);
    net_pack.head_union.head.status = htons(0);
    net_pack.p_data = p_send_data;

    //vx_Net.pack_post(gCMD::VA_SET_ANGLE, NET_PACKET_HEAD_LEN + send_data_len, 0, 0, 0, 0, p_send_data);
    //ggNet::vx_Net.pack_post(gCMD::VA_SET_ANGLE, NET_PACKET_HEAD_LEN + send_data_len, 0, 0, 0, 0, p_send_data);
    ggNet::vx_Net.pack_post(net_pack);


    memset(&net_pack, 0, sizeof(NET_PACKET_STRUCT));
    //waiting data from net......
    //int status = vx_Net.wait_data_from_net(gCMD::VA_SET_ANGLE, wait_seconds, net_pack);
    //int status = ggNet::vx_Net.wait_data(gCMD::VA_SET_ANGLE, wait_seconds, net_pack);

    int status = OK;

    if(status != OK)
    {
        LOG(ERROR)<<"VA set angle timeout pthread!";
        return status;
    }

    if(net_pack.head_union.head.status == OK)
    {
        LOG(INFO)<<"VA set angle "<< htonf(angle) <<" successful......";
        //gVar::IL_system_params.VA_angle = htonf(angle);
    }
    else
    {
        LOG(ERROR)<<"VA set angle error......";
    }

    if(net_pack.p_data != NULL)
    {
        delete[] net_pack.p_data;
    }
    return net_pack.head_union.head.status;

}

