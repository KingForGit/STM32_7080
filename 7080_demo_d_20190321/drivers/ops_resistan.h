#ifndef __OPS_RESISTAN_H__
#define __OPS_RESISTAN_H__

#include "board.h"

#pragma diag_suppress 231

#define RES_VREF_V (float)3300        /* 电阻采集参考电压 单位 mv*/
#define RES_LOW_SAMPLI (float)3000    /* 低档位匹配电阻 单位Ω*/
#define RES_HIGH_SAMPLI (float)186000 /* 高档位匹配电阻 单位Ω*/
#define RES_PARALLEL (float)100000    /* 并联支路电阻 单位Ω*/

/* 测量点临界电压 3300mv/11 = 299mV; 299mv*3.3k/(3300mv-299) = 328*/
#define RES_GRAE_CRITICAL_R (float)328                           /* 档位临界值 单位Ω*/
#define RES_MAX_GRAE_THRESHOLD (float)(RES_GRAE_CRITICAL_R - 10) /* 低档位测量阈值 单位Ω (低于318Ω的使用低档位)*/
#define RES_MIN_GRAE_THRESHOLD (float)(RES_GRAE_CRITICAL_R - 20) /* 高档位测量阈值 单位Ω (高于308Ω的使用高档位)*/

/* 测量范围 3Ω - 21k*/
#define RES_COLLECT_MIN (float)3     /* 总路最小电阻值 最小分辨率33mv/11 = 3mv; 3mv*3.3k /(3300mv-3); = 3*/
#define RES_COLLECT_MAX (float)17920 /* 总路最大电阻值 最大分辨率3190mv/11 = 290mV; 290mv*186k/(3300mv-290) = 17920*/

enum RES_CH
{
    RES_CH_1 = 0,
    RES_CH_2 = 1,
    RES_CH_3 = 2,
    RES_CH_4 = 3,
    RES_CH_ALL
};
enum RES_GEARS
{
    LOW_GEARS = 1,
    HIGH_GEARS = 2
};

#pragma pack(1)
struct res_status
{
    uint8_t ch_num;   /* 通道号*/
    bool status;      /* 当前通道采集状态 true：采集成功; false：采集失败; */
    bool grae;        /* 当前采集使用的档位, false: 高档位; true: 低档位*/
    float branch_v;   /* 采集点支路电压 (单位mv)*/
    float branch_r;   /* 采集点支路电阻 (单位Ω)*/
    float geae_min_r; /* 当前档位最小量程*/
    float geae_max_r; /* 当前档位最大量程*/
    float res_value;  /* 需要采集的电阻值*/
};
#pragma pack()

/**
 * @desc  : 设置采集电阻使用的档位
 * @param : ch: 通道号; *res_value: 通道对应内存; gears 设置的档位
 * @return: none
 * @Date  : 2018-12-27 15:47:42
 */
extern void set_res_gears(enum RES_CH ch, struct res_status *data, enum RES_GEARS gears);
/**
 * @desc  : 获取电阻测量结果
 * @param : adc: adc 原始值; *res_value: 电阻测量结果
 * @return: none
 * @Date  : 2018-11-23 11:30:06
 */
extern void get_res_measure_result(enum RES_CH ch, struct adc_result *adc, struct res_status data[]);
#endif /* end of include guard: __POS_ADC_RES_H__ */
