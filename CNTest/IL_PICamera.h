#ifndef IL_PICAMERA_H
#define IL_PICAMERA_H

#include <string>
#include "picam.h"
#include <QString>

#include "IL_types.h"

using namespace std;

namespace gCMD
{
    //PICamera
    const int PICAM_INIT = 0xFF01;
    const int PICAM_ACQUIRE = 0xFF02;

    const int PICAM_ACQUIRE_AND_SAVE = 0xFF03;

    const int PICAM_GET_CAMERA_MODEL = 0xFF04;
    const int PICAM_SET_TEMPERATURE = 0xFF05;
    const int PICAM_READ_TEMPERATURE = 0xFF06;
    const int PICAM_SET_EXPOSURE_TIME = 0xFF07;
    const int PICAM_SET_READOUT_MODE = 0xFF08;
    const int PICAM_SET_ADC_QUALITY = 0xFF09;
    const int PICAM_SET_ADC_SPEED = 0xFF0A;
    const int PICAM_SET_ADC_ANALOG_GAIN = 0xFF0B;
    const int PICAM_SET_ADC_EM_GAIN = 0xFF0C;
    const int PICAM_SET_TRIGGER_MODE = 0xFF0D;
    const int PICAM_SET_SHUTTER_MODE = 0xFF0E;

    const int PICAM_COMMIT_COMMON_PARAMETERS = 0xFF0F;
    const int PICAM_READ_COMMON_PARAMETERS = 0xFF10;
    const int PICAM_OPEN_SHUTTER = 0xFF11;
    const int PICAM_CLOSE_SHUTTER = 0xFF12;

    const int PICAM_CONFIG = 0xFF13;
}

typedef enum PicamAdcSpeed
{
    PicamAdcSpeed_10MHz   = 1,
    PicamAdcSpeed_5MHz    = 2,
    PicamAdcSpeed_1MHz    = 3,
    PicamAdcSpeed_100KHz  = 4
}PicamAdcSpeed;

struct PICamera_Params_STRUCT
{
    piflt temperature_setpoint;
    piflt exposure_time;
    PicamReadoutControlMode readout_control_mode;
    PicamAdcQuality adc_quality;
    //piflt adc_speed;
    PicamAdcSpeed adc_speed;
    PicamAdcAnalogGain adc_analog_gain;
    piint adc_em_gain;
    PicamTriggerResponse trigger_response;
    PicamShutterTimingMode shutter_mode;
};

typedef struct
{
    int size;
    pi16u data[1024*1024];
}CCD_Data_STRUCT;


#endif // IL_PICAMERA_H
