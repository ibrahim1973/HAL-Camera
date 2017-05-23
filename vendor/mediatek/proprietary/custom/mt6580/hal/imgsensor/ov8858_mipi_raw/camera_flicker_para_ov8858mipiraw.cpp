#define MTK_LOG_ENABLE 1
#include <utils/Log.h>
#include <fcntl.h>
#include <math.h>

#include "camera_custom_nvram.h"
#include "camera_custom_sensor.h"
#include "image_sensor.h"
#include "kd_imgsensor_define.h"
#include "camera_AE_PLineTable_ov8858mipiraw.h"
#include "camera_custom_AEPlinetable.h"
#include "camera_custom_flicker_para.h"
#include <cutils/log.h>

static void get_flicker_para(FLICKER_CUST_PARA* para, int flickerGradThreshold, int flickerSearchRange)
{
  int i;
  int freq[9] = { 70, 90, 100, 110, 120, 140, 160, 190, 230};
  for(i=0;i<9;i++)
  {
    para->flickerFreq[i]=freq[i];
  }
  para->flickerGradThreshold=flickerGradThreshold;
  para->flickerSearchRange=flickerSearchRange;
  para->minPastFrames=3;
  para->maxPastFrames=14;
  para->EV50_thresholds[0]=-30;
  para->EV50_thresholds[1]=12;
  para->EV60_thresholds[0]=-30;
  para->EV60_thresholds[1]=12;
  para->freq_feature_index[0]=4;
  para->freq_feature_index[1]=2;
}

static void get_flicker_para_by_Preview(FLICKER_CUST_PARA* para)
{
  FLICKER_CUST_STATISTICS EV50_L50 = {-194, 4721, 381, -766};
  FLICKER_CUST_STATISTICS EV50_L60 = {877, 529, 989, -455};
  FLICKER_CUST_STATISTICS EV60_L50 = {1046, 558, 1473, -530};
  FLICKER_CUST_STATISTICS EV60_L60 = {-162, 2898, 247, -642};
  para->EV50_L50=EV50_L50;
  para->EV50_L60=EV50_L60;
  para->EV60_L50=EV60_L50;
  para->EV60_L60=EV60_L60;
  get_flicker_para(para, 27, 20);
}

static void get_flicker_para_by_Video(FLICKER_CUST_PARA* para)
{
  FLICKER_CUST_STATISTICS EV50_L50 = {-194, 4721, 381, -766};
  FLICKER_CUST_STATISTICS EV50_L60 = {881, 527, 985, -454};
  FLICKER_CUST_STATISTICS EV60_L50 = {1051, 555, 1466, -529};
  FLICKER_CUST_STATISTICS EV60_L60 = {-162, 2898, 247, -642};
  para->EV50_L50=EV50_L50;
  para->EV50_L60=EV50_L60;
  para->EV60_L50=EV60_L50;
  para->EV60_L60=EV60_L60;
  get_flicker_para(para, 24, 40);
}

static void get_flicker_para_by_Capture(FLICKER_CUST_PARA* para)
{
  FLICKER_CUST_STATISTICS EV50_L50 = {-194, 4721, 381, -766};
  FLICKER_CUST_STATISTICS EV50_L60 = {881, 527, 985, -454};
  FLICKER_CUST_STATISTICS EV60_L50 = {1051, 555, 1466, -529};
  FLICKER_CUST_STATISTICS EV60_L60 = {-162, 2898, 247, -642};
  para->EV50_L50=EV50_L50;
  para->EV50_L60=EV50_L60;
  para->EV60_L50=EV60_L50;
  para->EV60_L60=EV60_L60;
  get_flicker_para(para, 24, 40);
}


typedef NSFeature::RAWSensorInfo<0x8858> SensorInfoSingleton_T;
namespace NSFeature {
template <>
UINT32
SensorInfoSingleton_T::
impGetFlickerPara(MINT32 sensorMode, MVOID*const pDataBuf) const
{
    ALOGD("impGetFlickerPara+ mode=%d", sensorMode);
    ALOGD("prv=%d, vdo=%d, cap=%d, zsd=%d",
        (int)e_sensorModePreview, (int)e_sensorModeVideoPreview, (int)e_sensorModeCapture, (int)e_sensorModeZsd );
    FLICKER_CUST_PARA* para;
    para =  (FLICKER_CUST_PARA*)pDataBuf;
    if(sensorMode == e_sensorModePreview)//0
        get_flicker_para_by_Preview(para);

    else if(sensorMode == e_sensorModeVideo)//1
    {
        get_flicker_para_by_Video(para);
    }
    else if(sensorMode == e_sensorModeCapture)//2
    {
        get_flicker_para_by_Capture(para);
    }
    else
    {
        ALOGD("impGetFlickerPara ERROR ln=%d", __LINE__);
        return -1;
    }
    ALOGD("impGetFlickerPara-");
    return 0;
}
}

