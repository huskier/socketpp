#include "IL_PICamera_over_UDT.h"

#include "IL_global.h"
//#include "IL_net_gUDT4PICam.h"
#include "util.h"

#include <vips/vips8>

#include "glog/logging.h"

using namespace vips;

PICamera_over_UDT::PICamera_over_UDT()
{

}

STATUS PICamera_over_UDT::acquire_to_save(string filename)
{
    time_t wait_seconds = 100;
    UDT_PACKET_STRUCT udt_pack;

    memset(&udt_pack, 0, sizeof(UDT_PACKET_STRUCT));

    unsigned short send_data_len = filename.length();
    char *p_send_data = new char[send_data_len];
    memcpy(p_send_data, filename.c_str(), send_data_len);

    udt_pack.head_union.head.cmd = gCMD::PICAM_ACQUIRE_AND_SAVE;
    udt_pack.head_union.head.total_length = UDT_PACKET_HEAD_LEN + send_data_len;
    udt_pack.head_union.head.cnt = htons(0);
    udt_pack.head_union.head.check = htons(0);
    udt_pack.head_union.head.priority = htons(0);
    udt_pack.head_union.head.status = htons(0);
    udt_pack.p_data = p_send_data;

    //ggNet::udt_Net.pack_post(gCMD::PICAM_ACQUIRE_AND_SAVE, UDT_PACKET_HEAD_LEN + send_data_len, 0, 0, 0, 0, p_send_data);
    ggNet::udt_Net.pack_post(udt_pack);


    memset(&udt_pack, 0, sizeof(UDT_PACKET_STRUCT));
    //waiting data from net......
    int status = ggNet::udt_Net.wait_data(gCMD::PICAM_ACQUIRE_AND_SAVE, wait_seconds, udt_pack);

    if(status != OK)
    {
        return status;
    }

    if(udt_pack.head_union.head.status == OK)
    {
        LOG(INFO)<<"acquire_to_save successful......";

        //VImage outImg(udt_pack.p_data, 512/*width*/, 512/*height*/, 1/*band*/,VImage::FMTUSHORT);
        //outImg.write(filename.c_str());
        VImage outImg = VImage::new_from_memory(udt_pack.p_data,512*512*2,512,512,1,VIPS_FORMAT_USHORT);
        outImg.write_to_file(filename.c_str());
    }
    else
    {
        LOG(ERROR)<<"acquire_to_save......"<< status;
    }

    if(udt_pack.p_data != NULL)
    {
        delete[] udt_pack.p_data;
    }

    return udt_pack.head_union.head.status;
}

void PICamera_over_UDT::initialize()
{
    time_t wait_seconds = 10;
    UDT_PACKET_STRUCT udt_pack;
    memset(&udt_pack, 0, sizeof(UDT_PACKET_STRUCT));

    unsigned short send_data_len = 0;
    ggNet::udt_Net.pack_post(gCMD::PICAM_INIT, UDT_PACKET_HEAD_LEN + send_data_len, 0, 0, 0, 0, NULL);

    //waiting data from net......
    int status = ggNet::udt_Net.wait_data(gCMD::PICAM_INIT, wait_seconds, udt_pack);

    if(status != OK)
    {
//        return status;
        LOG(INFO)<<"PICamer init error "<<status;
    }

    if(udt_pack.head_union.head.status == OK)
    {
        LOG(INFO)<<"PICamer init successful......";
    }
    else
    {
        LOG(ERROR)<<"PICamer init error is "<< status;
    }

    if(udt_pack.p_data != NULL)
    {
        delete[] udt_pack.p_data;
    }

//    return udt_pack.head_union.head.status;
}


//STATUS PICamera_over_UDT::configure(PICamera_Params_STRUCT config)
//{
//    time_t wait_seconds = 10;
//    UDT_PACKET_STRUCT udt_pack;
//    memset(&udt_pack, 0, sizeof(UDT_PACKET_STRUCT));
//    unsigned short send_data_len = sizeof(config);
//    char *p_send_data = new char[send_data_len];
//    config.temperature_setpoint = htond(config.temperature_setpoint);
//    config.exposure_time = htond(config.exposure_time);
//    config.readout_control_mode =PicamReadoutControlMode(htonl(config.readout_control_mode));
//    config.adc_quality =PicamAdcQuality( htonl(config.adc_quality));
//    config.adc_speed = PicamAdcSpeed(htonl(config.adc_speed));
//    config.adc_analog_gain = PicamAdcAnalogGain(htonl(config.adc_analog_gain));
//    config.adc_em_gain = htonl(config.adc_em_gain);
//    config.trigger_response = PicamTriggerResponse(htonl(config.trigger_response));
//    config.shutter_mode = PicamShutterTimingMode(htonl(config.shutter_mode));

//    memcpy(p_send_data,&config,sizeof(PICamera_Params_STRUCT));
//    gUDT4PICam::pack_post(gCMD::PICAM_CONFIG, UDT_PACKET_HEAD_LEN + send_data_len, 0, 0, 0, 0,p_send_data);

//    //waiting data from net......
//    int status = gUDT4PICam::wait_data_from_net(gCMD::PICAM_CONFIG, wait_seconds, udt_pack);

//    if(status != OK)
//    {
//        LOG(ERROR)<<"PICamer configure error is "<<status;
//        return status;
//    }

//    if(udt_pack.head_union.head.status == OK)
//    {
//        LOG(INFO)<<"exposure configure successfully......";
//    }
//    else
//    {
//        LOG(ERROR)<<"PICamer configure error is "<< status;
//    }

//    if(udt_pack.p_data != NULL)
//    {
//        delete[] udt_pack.p_data;
//    }

//    return udt_pack.head_union.head.status;
//}


std::string PICamera_over_UDT::get_camera_model()
{
    time_t wait_seconds = 10;
    UDT_PACKET_STRUCT udt_pack;
    std::string str="";

    memset(&udt_pack, 0, sizeof(UDT_PACKET_STRUCT));

    unsigned short send_data_len = 0;
    ggNet::udt_Net.pack_post(gCMD::PICAM_GET_CAMERA_MODEL, UDT_PACKET_HEAD_LEN + send_data_len, 0, 0, 0, 0, NULL);

    //waiting data from net......
    int status = ggNet::udt_Net.wait_data(gCMD::PICAM_GET_CAMERA_MODEL, wait_seconds, udt_pack);

    if(status != OK)
    {
        //return status;
        LOG(ERROR)<<"PICamer get_camera_model error is "<<status;
    }

    if(udt_pack.head_union.head.status == OK)
    {
        LOG(INFO)<<"PICamer get model successful......";
        char model[udt_pack.head_union.head.total_length-UDT_PACKET_HEAD_LEN];
        memset(model, 0, udt_pack.head_union.head.total_length-UDT_PACKET_HEAD_LEN);
        memcpy(model, udt_pack.p_data, udt_pack.head_union.head.total_length-UDT_PACKET_HEAD_LEN);
        LOG(INFO)<< "model is " <<model;
        str.assign(model);
        LOG(INFO)<<"PICamer get model OK!";
    }
    else
    {
        LOG(ERROR)<<"PICamer get model error is "<< status;
    }

    if(udt_pack.p_data != NULL)
    {
        delete[] udt_pack.p_data;
    }

    return str;
    //return udt_pack.head_union.head.status;
}


STATUS PICamera_over_UDT::acquire()
{
    time_t wait_seconds = 100;
    UDT_PACKET_STRUCT udt_pack;
    memset(&udt_pack, 0, sizeof(UDT_PACKET_STRUCT));

    unsigned short send_data_len = 0;

    ggNet::udt_Net.pack_post(gCMD::PICAM_ACQUIRE, UDT_PACKET_HEAD_LEN + send_data_len, 0, 0, 0, 0, NULL);

    //waiting data from net......
    int status = ggNet::udt_Net.wait_data(gCMD::PICAM_ACQUIRE, wait_seconds, udt_pack);

    if(status != OK)
    {
        return NULL;
    }

    if(udt_pack.head_union.head.status == OK)
    {
        LOG(INFO)<<"CCD acquire successful......";

        CCD_Data.size = 512*512*sizeof(pi16u);

        memset(CCD_Data.data,0,1024*1024*sizeof(pi16u));
        memcpy(CCD_Data.data,udt_pack.p_data,512*512*sizeof(pi16u));

        /*
        VImage in_img;
        // reading image
        in_img = VImage::new_from_memory(CCD_Data.data,512*512*2,512,512,1,VIPS_FORMAT_USHORT);
        in_img.write_to_file("test_over_UDT.png",VImage::option()->set( "access", VIPS_ACCESS_SEQUENTIAL ) );

        LOG(INFO)<<"After VImage::write_to_file.......";
        */

    }
    else
    {
        LOG(ERROR)<<"acquire error......"<< status;
    }

    if(udt_pack.p_data != NULL)
    {
        delete[] udt_pack.p_data;
    }

    return OK;
}



void PICamera_over_UDT::set_temperature(piflt temp)
{
    time_t wait_seconds = 10;
    UDT_PACKET_STRUCT udt_pack;
    memset(&udt_pack, 0, sizeof(UDT_PACKET_STRUCT));
    unsigned short send_data_len = sizeof(piflt);
    char *p_send_data = new char[send_data_len];
    temp = htond(temp);
    memcpy(p_send_data,&temp,sizeof(piflt));
    ggNet::udt_Net.pack_post(gCMD::PICAM_SET_TEMPERATURE, UDT_PACKET_HEAD_LEN + send_data_len, 0, 0, 0, 0,p_send_data);

    //waiting data from net......
    int status = ggNet::udt_Net.wait_data(gCMD::PICAM_SET_TEMPERATURE, wait_seconds, udt_pack);

    if(status != OK)
    {
        //return status;
        LOG(ERROR)<<"PICamer set tempearture error is "<<status;
    }

    if(udt_pack.head_union.head.status == OK)
    {
        LOG(INFO)<<"tempearture set successfully......";
    }
    else
    {
        LOG(ERROR)<<"PICamer tempearture set error is "<< status;
    }

    if(udt_pack.p_data != NULL)
    {
        delete[] udt_pack.p_data;
    }

}

void PICamera_over_UDT::read_temperature(piflt& temperature, PicamSensorTemperatureStatus& status1)
{
    time_t wait_seconds = 10;
    UDT_PACKET_STRUCT udt_pack;
    float tmp;
    memset(&udt_pack, 0, sizeof(UDT_PACKET_STRUCT));
    unsigned short send_data_len = 0;

    ggNet::udt_Net.pack_post(gCMD::PICAM_READ_TEMPERATURE, UDT_PACKET_HEAD_LEN + send_data_len, 0, 0, 0, 0,NULL);

    //waiting data from net......
    int status = ggNet::udt_Net.wait_data(gCMD::PICAM_READ_TEMPERATURE, wait_seconds, udt_pack);

    LOG(INFO)<<"exposure time before status....";
    if(status != OK)
    {
        LOG(ERROR)<<"PICamer set exposure time error is "<<status;
    }

    if(udt_pack.head_union.head.status == OK)
    {
        LOG(INFO)<<"get temperature set successfully......";
        char temp[udt_pack.head_union.head.total_length-UDT_PACKET_HEAD_LEN];
        memset(&tmp, 0, udt_pack.head_union.head.total_length-UDT_PACKET_HEAD_LEN);
        memcpy(&tmp, udt_pack.p_data, udt_pack.head_union.head.total_length-UDT_PACKET_HEAD_LEN);
        LOG(INFO)<<"before htond, temperature = "<<tmp;
        temperature =double(ntohf(tmp));
        LOG(INFO)<< "temp is " <<temperature;
    }
    else
    {
        LOG(ERROR)<<"PICamer exposure time set error is "<< status;
    }

    if(udt_pack.p_data != NULL)
    {
        delete[] udt_pack.p_data;
    }

}


void PICamera_over_UDT::set_exposure_time(piflt exposure_time)
{
    //std::cout << "Set 500 ms exposure time: ";
    time_t wait_seconds = 10;
    UDT_PACKET_STRUCT udt_pack;
    memset(&udt_pack, 0, sizeof(UDT_PACKET_STRUCT));
    unsigned short send_data_len = sizeof(piflt);
    char *p_send_data = new char[send_data_len];
    exposure_time = htond(exposure_time);
    memcpy(p_send_data,&exposure_time,sizeof(piflt));
    ggNet::udt_Net.pack_post(gCMD::PICAM_SET_EXPOSURE_TIME, UDT_PACKET_HEAD_LEN + send_data_len, 0, 0, 0, 0,p_send_data);

    //waiting data from net......
    int status = ggNet::udt_Net.wait_data(gCMD::PICAM_SET_EXPOSURE_TIME, wait_seconds, udt_pack);

    LOG(INFO)<<"exposure time before status....";
    if(status != OK)
    {
        //return status;
        LOG(ERROR)<<"PICamer set exposure time error is "<<status;
    }

    if(udt_pack.head_union.head.status == OK)
    {
        LOG(INFO)<<"exposure time set successfully......";
    }
    else
    {
        LOG(ERROR)<<"PICamer exposure time set error is "<< status;
    }

    if(udt_pack.p_data != NULL)
    {
        delete[] udt_pack.p_data;
    }


    //return udt_pack.head_union.head.status;

    //LOG(INFO)<<error<<"exposure time set successfully...";
}

void PICamera_over_UDT::set_readout_mode(PicamReadoutControlMode readout_mode)
{
    PicamError error;
    error =
        Picam_SetParameterIntegerValue(
            camera,
            PicamParameter_ReadoutControlMode,
            readout_mode );
    LOG(INFO)<<error<<"readout mode set successfully...";
}

void PICamera_over_UDT::set_adc_quality(PicamAdcQuality quality)
{
    PicamError error;
    error =
        Picam_SetParameterIntegerValue(
            camera,
            PicamParameter_AdcQuality,
            quality );
    LOG(INFO)<<error<<"AdcQuality set successfully...";
}

void PICamera_over_UDT::set_adc_speed(piflt speed)
{
    PicamError error;
    error =
        Picam_SetParameterFloatingPointValue(
            camera,
            PicamParameter_AdcSpeed,
            speed );
    LOG(INFO)<<error<<"AdcSpeed set successfully...";
}

void PICamera_over_UDT::set_adc_analog_gain(PicamAdcAnalogGain analog_gain)
{
    PicamError error;
    error =
        Picam_SetParameterIntegerValue(
            camera,
            PicamParameter_AdcAnalogGain,
            analog_gain );
    LOG(INFO)<<error<<"AdcAnalogGain set successfully...";
}

void PICamera_over_UDT::set_adc_em_gain(piint em_gain)
{
    PicamError error;
    error =
        Picam_SetParameterIntegerValue(
            camera,
            PicamParameter_AdcEMGain,
            em_gain );
    LOG(INFO)<<error<<"AdcEMGain set successfully...";
}

void PICamera_over_UDT::set_trigger_mode(PicamTriggerResponse trigger_mode)
{
    PicamError error;
    error =
        Picam_SetParameterIntegerValue(
            camera,
            PicamParameter_TriggerResponse,
            trigger_mode );
    LOG(INFO)<<error<<"TriggerResponse set successfully...";
}

void PICamera_over_UDT::set_shutter_mode(PicamShutterTimingMode shutter_mode)
{
    PicamError error;
    error =
        Picam_SetParameterIntegerValue(
            camera,
            PicamParameter_ShutterTimingMode,
            shutter_mode);
    LOG(INFO)<<error<<"shutter mode set successfully...";
}

void PrintEnumString( PicamEnumeratedType type, piint value )
{
    const pichar* string;
    Picam_GetEnumerationString( type, value, &string );
    //std::cout << string;
    LOG(INFO)<<string;
    Picam_DestroyString( string );
}

STATUS PICamera_over_UDT::commit_common_parameters(PICamera_Params_STRUCT config)
{
    time_t wait_seconds = 10;
    UDT_PACKET_STRUCT udt_pack;
    memset(&udt_pack, 0, sizeof(UDT_PACKET_STRUCT));
    unsigned short send_data_len = sizeof(config);
    char *p_send_data = new char[send_data_len];
    config.temperature_setpoint = htond(config.temperature_setpoint);
    config.exposure_time = htond(config.exposure_time);
    config.readout_control_mode =PicamReadoutControlMode(htonl(config.readout_control_mode));
    config.adc_quality =PicamAdcQuality( htonl(config.adc_quality));
    config.adc_speed = PicamAdcSpeed(htonl(config.adc_speed));
    config.adc_analog_gain = PicamAdcAnalogGain(htonl(config.adc_analog_gain));
    config.adc_em_gain = htonl(config.adc_em_gain);
    config.trigger_response = PicamTriggerResponse(htonl(config.trigger_response));
    config.shutter_mode = PicamShutterTimingMode(htonl(config.shutter_mode));

    memcpy(p_send_data,&config,sizeof(PICamera_Params_STRUCT));
    ggNet::udt_Net.pack_post(gCMD::PICAM_CONFIG, UDT_PACKET_HEAD_LEN + send_data_len, 0, 0, 0, 0,p_send_data);

    //waiting data from net......
    int status = ggNet::udt_Net.wait_data(gCMD::PICAM_CONFIG, wait_seconds, udt_pack);

    if(status != OK)
    {
        LOG(ERROR)<<"PICamer configure error is "<<status;
        return status;
    }

    if(udt_pack.head_union.head.status == OK)
    {
        LOG(INFO)<<"PIcamer configure successfully......";
    }
    else
    {
        LOG(ERROR)<<"PICamer configure error is "<< status;
    }

    if(udt_pack.p_data != NULL)
    {
        delete[] udt_pack.p_data;
    }

    return udt_pack.head_union.head.status;
}

QString PICamera_over_UDT::read_common_parameters()
{
    time_t wait_seconds = 10;
    UDT_PACKET_STRUCT udt_pack;
    float tmp;
    memset(&udt_pack, 0, sizeof(UDT_PACKET_STRUCT));
    unsigned short send_data_len = 0;
//    char *p_send_data = new char[send_data_len];
//    temp = htond(temp);
//    memcpy(p_send_data,&temp,sizeof(piflt));
    ggNet::udt_Net.pack_post(gCMD::PICAM_READ_COMMON_PARAMETERS, UDT_PACKET_HEAD_LEN + send_data_len, 0, 0, 0, 0,NULL);

    //waiting data from net......
    int status = ggNet::udt_Net.wait_data(gCMD::PICAM_READ_COMMON_PARAMETERS, wait_seconds, udt_pack);

    LOG(INFO)<<"exposure time before status.... status = "<<status;
    if(status != OK)
    {
        //return status;
        LOG(ERROR)<<"PICamer set exposure time error is "<<status;
    }

    if(udt_pack.head_union.head.status == OK)
    {
        LOG(INFO)<<"get param info successfully......";

        char temp[udt_pack.head_union.head.total_length-UDT_PACKET_HEAD_LEN];
        memset(&temp, 0, udt_pack.head_union.head.total_length-UDT_PACKET_HEAD_LEN);
        memcpy(&temp, udt_pack.p_data, udt_pack.head_union.head.total_length-UDT_PACKET_HEAD_LEN);
        LOG(INFO)<<"param info is :\n"<<temp;

        return QString(QLatin1String(temp));
    }
    else
    {
        LOG(ERROR)<<"PICamer get param error is "<< status;
        return QString("PICamer get param error");
    }

    if(udt_pack.p_data != NULL)
    {
        delete[] udt_pack.p_data;
    }

    return QString("PICamer get param error");

}

PICamera_over_UDT::~PICamera_over_UDT()
{
    if(inited)
    {
        LOG(INFO)<<"Picam_UninitializeLibrary......\n";
        Picam_UninitializeLibrary();
    }

}

void PICamera_over_UDT::open_shutter()
{
//    PicamError error;
//    error =
//        Picam_SetParameterIntegerValue(
//            camera,
//            PicamParameter_ShutterTimingMode,
//            PicamShutterTimingMode_AlwaysOpen);
//    LOG(INFO)<<error<<"shutter mode set successfully...";
//    commit_common_parameters();

    time_t wait_seconds = 10;
    UDT_PACKET_STRUCT udt_pack;
    float tmp;
    memset(&udt_pack, 0, sizeof(UDT_PACKET_STRUCT));
    unsigned short send_data_len = 0;
//    char *p_send_data = new char[send_data_len];
//    temp = htond(temp);
//    memcpy(p_send_data,&temp,sizeof(piflt));
    ggNet::udt_Net.pack_post(gCMD::PICAM_OPEN_SHUTTER, UDT_PACKET_HEAD_LEN + send_data_len, 0, 0, 0, 0,NULL);

    //waiting data from net......
    int status = ggNet::udt_Net.wait_data(gCMD::PICAM_OPEN_SHUTTER, wait_seconds, udt_pack);

    LOG(INFO)<<"open shutter before status....";
    if(status != OK)
    {
        //return status;
        LOG(ERROR)<<"PICamer open shutter error is "<<status;
    }

    if(udt_pack.head_union.head.status == OK)
    {
        LOG(INFO)<<"open shutter successfully......";

    }
    else
    {
        LOG(ERROR)<<"PICamer open shutter error is "<< status;
    }

    if(udt_pack.p_data != NULL)
    {
        delete[] udt_pack.p_data;
    }
}

void PICamera_over_UDT::close_shutter()
{
//    PicamError error;
//    error =
//        Picam_SetParameterIntegerValue(
//            camera,
//            PicamParameter_ShutterTimingMode,
//            PicamShutterTimingMode_AlwaysClosed);
//    LOG(INFO)<<error<<"shutter mode set successfully...";
//    commit_common_parameters();
    time_t wait_seconds = 10;
    UDT_PACKET_STRUCT udt_pack;
    float tmp;
    memset(&udt_pack, 0, sizeof(UDT_PACKET_STRUCT));
    unsigned short send_data_len = 0;
//    char *p_send_data = new char[send_data_len];
//    temp = htond(temp);
//    memcpy(p_send_data,&temp,sizeof(piflt));
    ggNet::udt_Net.pack_post(gCMD::PICAM_CLOSE_SHUTTER, UDT_PACKET_HEAD_LEN + send_data_len, 0, 0, 0, 0,NULL);

    //waiting data from net......
    int status = ggNet::udt_Net.wait_data(gCMD::PICAM_CLOSE_SHUTTER, wait_seconds, udt_pack);

    LOG(INFO)<<"open shutter before status....";
    if(status != OK)
    {
        //return status;
        LOG(ERROR)<<"PICamer close shutter error is "<<status;
    }

    if(udt_pack.head_union.head.status == OK)
    {
        LOG(INFO)<<"close shutter successfully......";

    }
    else
    {
        LOG(ERROR)<<"PICamer close shutter error is "<< status;
    }

    if(udt_pack.p_data != NULL)
    {
        delete[] udt_pack.p_data;
    }
}
