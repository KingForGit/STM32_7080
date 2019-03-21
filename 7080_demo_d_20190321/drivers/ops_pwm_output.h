#ifndef __OPS_PWM_OUTPUT_H__
#define __OPS_PWM_OUTPUT_H__

#include "board.h"

enum PWM_OUT_CH
{
    PWM_OUT_1 = 1,
    PWM_OUT_2 = 2,
    PWM_OUT_3 = 3,
    PWM_OUT_4 = 4,
};
enum PWM_DAC_CH
{
    PWM_DAC_1 = 1,
    PWM_DAC_2 = 2,
	PWM_DAC_3 = 3,
    PWM_DAC_4 = 4,
};
struct pwm_out
{
    bool status;          /* false 未设置频率; true 已设置*/
    float duty_cycle;     /* 当前占空比*/
    uint16_t period;      /* 自动重装载值*/
    uint32_t timer_clock; /* 计数频率*/
};

/**
 * @desc  : 配置和打开pwm输出
 * @param : ch 打开的通道号;
 *          out_freq 输出频率单位Hz(0.6 ~ 1000000 hz);
 *          duty_cycle(0~100%)
 * @return: None
 * @Date  : 2018-12-2 20:33:28
 */
extern void open_pwm_outup(enum PWM_OUT_CH ch,
                                  float out_freq,
                                  float duty_cycle);

extern void close_pwm_output(enum PWM_OUT_CH ch);
/**
 * @desc  : 设置pwm输出占空比
 * @param : duty_cycle 占空比 (1~100%)
 * @return: 设置成功 0; 设置失败 -1; 返回-1时，需要设置频率
 * @Date  : 2018-12-3 11:23:30
 */
extern int set_pwm_duty_cycle(enum PWM_OUT_CH ch,
                              float duty_cycle);

/**
 * @desc  : 获取pwm输出的信息
 * @param : struct pwm_out 型指针
 * @return: None
 * @Date  : 2018-12-3 11:33:28
 */
extern void get_pwm_conf(struct pwm_out *data);
extern void pwm_dac_init(void);
#endif /* end of include guard: __POS_PWM_OUT_H */
