#ifndef IL_PICAMERA_OVER_UDT_H
#define IL_PICAMERA_OVER_UDT_H

#include <string>
#include "picam.h"
#include <QString>

#include "IL_PICamera.h"
#include "IL_types.h"

using namespace std;

class PICamera_over_UDT
{
public:
    PICamera_over_UDT();
    ~PICamera_over_UDT();

public:

    void initialize();
//    STATUS configure(PICamera_Params_STRUCT config);

    STATUS acquire();

    STATUS acquire_to_save(string filename);

    std::string get_camera_model();
    void set_temperature(piflt temp);
    void read_temperature(piflt& temperature, PicamSensorTemperatureStatus& status1);

    void set_exposure_time(piflt exposure_time);
    void set_readout_mode(PicamReadoutControlMode readout_mode);
    void set_adc_quality(PicamAdcQuality quality);
    void set_adc_speed(piflt speed);
    void set_adc_analog_gain(PicamAdcAnalogGain analog_gain);
    void set_adc_em_gain(piint em_gain);
    void set_trigger_mode(PicamTriggerResponse trigger_mode);
    void set_shutter_mode(PicamShutterTimingMode shutter_mode);

    STATUS commit_common_parameters(PICamera_Params_STRUCT config);
    QString read_common_parameters();

public:
    void open_shutter();
    void close_shutter();

private:
    pibln* inited;
    PicamHandle camera;
    PicamCameraID id;
    const pichar* picam_string;
    PicamAvailableData image_data;
    PicamAcquisitionErrorsMask errors;
    piint readoutstride;
    //PICAM_CONFIG_STRCUT config;
    PICamera_Params_STRUCT params_to_commit;

public:
    CCD_Data_STRUCT CCD_Data;

};

#endif // IL_PICAMERA_OVER_UDT_H
