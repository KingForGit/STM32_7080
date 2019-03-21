#ifndef __OPS_IO_INPUT_H__
#define __OPS_IO_INPUT_H__

#include "board.h"

enum sw_mode
{
    LEVITATE = 0,
    PULL_UP = 1,
    PULL_DOWN = 2
};

enum SW_CH
{
    SW_CH_1 = 1,
    SW_CH_2 = 2,
    SW_CH_3 = 3,
    SW_CH_4 = 4,
    SW_CH_5 = 5,
    SW_CH_6 = 6,
    SW_CH_7 = 7,
    SW_CH_8 = 8,
    SW_CH_9 = 9,
    SW_CH_10 = 10,
    SW_CH_11 = 11,
    SW_CH_12 = 12,
    SW_CH_13 = 13,
    SW_CH_14 = 14,
    SW_CH_15 = 15,
    SW_CH_16 = 16,
    SW_CH_17 = 17,
    SW_CH_18 = 18,
    SW_CH_19 = 19,
    SW_CH_ALL
};

/* io inout operators*/
#pragma pack(1)
struct switch_status
{
    uint8_t ch_num;
    uint8_t sw_mode;   /* 0: 浮空模式; 上拉模式 1; 下拉模式 2*/
    uint8_t sw_status; /* 0: 开关处于低; 1: 开关处于高*/
    uint8_t reuse;     /* 0：不可以高低复用; 1：具有上拉电阻,可以采集低有效*/
    uint8_t down_res;  /* 0：通道没有下拉电阻; 1：具有下拉电阻*/
    float ref_volt;    /* 比较器当前比较电压 单位mv*/
};
#pragma pack()

/**
 * @desc  : 配置开关量采集模式
 * @param : ch 通道号; mode 配置模式(PULL_UP：上拉; PULL_DOWN: 下拉; LEVITATE 浮空)
 *          *data 更新sw采集模式
 * @return: none
 * @Date  : 2018-11-28 14:51:43
 */
extern void conf_switch_mode(enum SW_CH ch, enum sw_mode mode, struct switch_status *data);

/**
 * @desc  : 配置开关量比较电压
 * @param : ch 配置通道号; volt 比较的电压(单位 volt:0~3300  表示0-3.3v)
 * @return: none
 * @Date  : 2018-11-28 14:50:45
 */
extern void conf_switch_compare_volt(enum SW_CH ch, uint16_t volt);

/**
 * @desc  : 获取开关量状态
 * @param : ch 指定通道; data 当前状态
 * @return: none
 * @Date  : 2018-11-24 20:18:53
 */
extern void get_switch_status(enum SW_CH ch, struct switch_status *data);
#endif /* end of include guard: __POS_IO_SW_H__*/
