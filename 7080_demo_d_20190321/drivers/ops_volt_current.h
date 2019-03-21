/**
 * @author: emlsyx
 * @email:  yangx_1118@163.com
 * @create: date 2018-11-23 18:50:56
 * @modify: date 2018-11-23 18:50:56
 * @desc:   [description]
*/
#ifndef __OPS_VOLT_CURRENT_H__
#define __OPS_VOLT_CURRENT_H__

#include "board.h"

#pragma diag_suppress 231

/* 电压采集*/
#define VOLT_MATCHING_R 100000 /* 匹配电阻*/
#define VOLT_BLEEDER_R 10000  /* 通道分压电阻*/

/* 电源电压采集*/
#define BAT_MATCHING_R 150000 /* 电源电压匹配电阻*/
#define BAT_BLEEDER_R 15000   /* 电源电压分压电阻*/

/* 电流采集*/
#define CU_MATCHING_R 720 /* 电流采集匹配电阻 720Ω*/
#define CU_BLEEDER_R 120  /* 电流采集对象电阻值 120Ω*/

#define SAMPLI_VOLT (VOLT_MATCHING_R + VOLT_BLEEDER_R) / VOLT_BLEEDER_R /* 电压采集分压比*/
#define SAMPLI_BAT (BAT_MATCHING_R + BAT_BLEEDER_R) / BAT_BLEEDER_R     /* 电源电压采集分压比*/
#define SAMPLI_CURR (CU_MATCHING_R + CU_BLEEDER_R) / CU_BLEEDER_R       /* 电流采集分压比*/

enum VOLT_CH
{
    VOLT_CH_1 = 1,   /* 电压采集通道1 电压采集范围0~5v; 电流采集范围0~0.25mA*/
    VOLT_CH_2 = 2,   /* 电压采集通道2 电压采集范围0~5v; 电流采集范围0~0.25mA*/
    VOLT_CH_3 = 3,   /* 电压采集通道3 电压采集范围0~5v; 电流采集范围0~0.25mA*/
    VOLT_CH_4 = 4,   /* 电压采集通道4 电压采集范围0~5v; 电流采集范围0~0.25mA*/
    CURR_CH_1 = 5,   /* 电流采集通道1 采集范围0~23.1v 电流采集范围0~27.49mA*/
    CURR_CH_2 = 6,   /* 电流采集通道2 采集范围0~23.1v 电流采集范围0~27.49mA*/
    VOLT_CH_BAT = 7, /* 电源输入电压通道 电压范围 8~32V*/
    VOLT_CH_ALL = 8, /* 采集全部4路电压通道电压 (参数对象指针,必须足够4个,否则内存溢出)*/
    CURR_CH_ALL = 9, /* 采集全部4路电压采集2路电流采集通道电压 (参数对象指针,必须足够6个,否则内存溢出)*/
    VOLT_CH          /* 采集全部所有可采集通道 电压 (参数对象指针,必须足够7个,否则内存溢出)*/
};

#pragma pack(1)
struct volt_current
{
    uint8_t ch_num;      /* 通道号*/
    float adc_value;     /* adc 原始值*/
    uint32_t matching_r; /* 匹配电阻 单位Ω*/
    uint32_t bleeder_r;  /* 匹配电阻 单位Ω*/
    uint16_t proportion; /* 分压比 */
    float vol_num;    /*电压采集次数 */
    float volt_per;    /* d单次电压值 单位v*/
    float volt_value;    /* 电压值 单位v*/
    float current_value; /* 电流值 单位mA*/
};
#pragma pack()


/**
 * @desc  : 获取电压/电流测量结果
 * @param : adc: adc输出原始值;
 *          data: 电压/电流测量值存放地址
 * @return: none
 * @Date  : 2018-11-28 11:14:11
 */
extern void get_volt_current_measure_result(enum VOLT_CH ch,
                                            struct adc_result *adc,
                                            struct volt_current *data);

#endif /* end of include guard: __POS_ADC_VOLT_H__ */
