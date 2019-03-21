#ifndef __DRV_ADC_H__
#define __DRV_ADC_H__

#include "board.h"

//#define SAMPLE_CNT (char)15              /* 采样求平均次数*/
#define SAMPLE_CNT (char)15           /* 采样求平均次数*/
#define ADC_RESOLUTION (float)3.3 / 4096 /* 单位 v*/
#define ADC_VSOLUTION (float)3300 / 4096 /* 单位 Mv*/
struct adc_channel
{
    uint8_t number;
    GPIO_TypeDef *port;
    uint16_t pin;
    uint8_t channel;
    uint8_t sample_time;
};

struct ADC_VALUE
{
    uint16_t temperature;
    uint16_t resistance_1;
    uint16_t resistance_2;
  //  uint16_t resistance_3;
 //   uint16_t resistance_4; /* 4路电阻采集*/
    uint16_t voltage_1;
    uint16_t voltage_2;   //	24V电源电压
    uint16_t voltage_3;
 //   uint16_t voltage_4; /* 4路电压采集*/
    uint16_t current_1;
 //   uint16_t current_2; /* 2路电流采集*/
 //   uint16_t car_bat_v;
};
#pragma pack(1)
struct adc_result
{
    bool status; /* adc 采集结果 true：成功; false：失败*/
    float temperature;
    float resistance_1;
    float resistance_2;
//    float resistance_3;
//    float resistance_4; /* 4路电阻采集*/
    float voltage_1;
    float voltage_2;
    float voltage_3;
//    float voltage_4; /* 4路电压采集*/
    float current_1;
//    float current_2; /* 2路电流采集*/
//    float car_bat_v;
};
#pragma pack()

struct ADC_SUM
{
    uint32_t temperature;
    uint32_t resistance_1;
    uint32_t resistance_2;
//    uint32_t resistance_3;
//    uint32_t resistance_4; /* 4路电阻采集*/
    uint32_t voltage_1;
    uint32_t voltage_2;
    uint32_t voltage_3;
//    uint32_t voltage_4; /* 4路电压采集*/
    uint32_t current_1;
//    uint32_t current_2; /* 2路电流采集*/
//    uint32_t car_bat_v; /* 车身电瓶采集*/
};

struct ADC_SAMPLE
{
    uint16_t temperature[SAMPLE_CNT];
    uint16_t resistance_1[SAMPLE_CNT];
    uint16_t resistance_2[SAMPLE_CNT];
//    uint16_t resistance_3[SAMPLE_CNT];
//    uint16_t resistance_4[SAMPLE_CNT]; /* 4路电阻采集*/
    uint16_t voltage_1[SAMPLE_CNT];
    uint16_t voltage_2[SAMPLE_CNT];
    uint16_t voltage_3[SAMPLE_CNT];
//    uint16_t voltage_4[SAMPLE_CNT]; /* 4路电压采集*/
    uint16_t current_1[SAMPLE_CNT];
//    uint16_t current_2[SAMPLE_CNT]; /* 2路电流采集*/
//    uint16_t car_bat_v[SAMPLE_CNT]; /* 2路电流采集*/
};

extern void adc_read_value(struct adc_result *adc_result);
#endif /* end of include guard: __DRV_ADC_H__ */
