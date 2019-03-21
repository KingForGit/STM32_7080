#ifndef __OPS_FREQ_INPUT_H__
#define __OPS_FREQ_INPUT_H__

#include "board.h"

enum FREQ_CH
{
    FREQ_CH_1 = 0,
    FREQ_CH_2 = 1,
    FREQ_CH_3 = 2,
    FREQ_CH_4 = 3,
    FREQ_CH_ALL
};

#pragma pack(1)
struct freq_status
{
    volatile uint8_t ch_num;        /* 通道号*/
    volatile bool status;           /* 当前通道采集状态 true：采集成功; false：采集失败; */
    volatile uint8_t mode;          /* 获取模式: 0 外部中断捕获模式; 1 定时器捕获模式*/
    volatile uint8_t duty_cycle;    /* 正脉冲占空比 单位%*/
    float freq_value;   /* 频率值 单位hz*/
    volatile uint32_t pulse_width;  /* 正脉宽值 单位us*/
    volatile uint32_t pulse_total;  /* 脉冲总个数*/
    volatile uint32_t pulse_second; /* 秒脉冲个数*/
    volatile uint32_t this_time;    /* 采样时间 单位s*/
};
#pragma pack()

/**
 * @desc  : 打开指定通道频率采集
 * @param : ch 打开的通道号; sw_mode 采集通道的io模式; dac_v dac比较电压
 * @return: None
 * @Date  : 2018-12-3 14:06:00
 */
extern void open_freq_collect(enum FREQ_CH ch, enum sw_mode mode, uint16_t dac_v);

/**
 * @desc  : 关闭通道频率采集
 * @param : ch 打开的通道号
 * @return: none
 * @Date  : 2018-12-3 16:44:50
 */
extern void close_freq_collect(enum FREQ_CH ch);

/**
 * @desc  : 获取频率采集结果
 * @param : ch 通道号; dat：采集状态数据存储放地址
 * @return: none
 * @Date  : 2018-12-4 10:40:17
 */
extern void get_freq_measure_result(enum FREQ_CH ch,
                                    struct freq_status *data);

#endif /* end of include guard: __OPS_FREQ_INPUT_H__ */
