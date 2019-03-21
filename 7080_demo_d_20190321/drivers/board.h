/**
 * @author: emlsyx
 * @email:  yangx_1118@163.com
 * @create: date 2018-11-27 19:46:55
 * @modify: date 2018-11-27 19:46:55
 * @desc:   [description]
*/
#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f10x.h"
#include <system_stm32f10x.h>
#include "stm32f10x_it.h"

/* rtos header file */
#include "RTL.h"

/* library config file*/
#include "lib_config.h"

#ifdef DEBUG_RES_COLLECT
    #define debug_res_collect() printf("\r\n- 通道号  采集状态  采集档位  匹配电阻1V(mV)  匹配电阻2(kΩ)  档位最小值(Ω)  档位最大值(Ω)  测量结果(Ω)\r\n");
#else
    #define debug_res_collect()
#endif

#ifdef DEBUG_VOLT_COLLECT
    #define debug_volt_current_collect() printf("\r\n-   通道类型     Adc 原始值(mV) 分压电阻1(kΩ)  分压电阻2(kΩ) 分压比  当前电压(v)  当前电流(mA)\r\n");
#else
    #define debug_volt_current_collect()
#endif

#ifdef DEBUG_SW_COLLECT
    #define debug_sw_collect() printf("\r\n-   通道类型  采集模式   Dac比较电压(v)  当前状态\r\n");
#else
    #define debug_sw_collect()
#endif

#ifdef DEBUG_FREQ_COLLECT
    #define debug_freq_collect() printf("\r\n- 通道号  采集状态  采集模式  脉冲总数   脉冲(个/s) 频率(Hz)  脉宽(us) 占空比  Tick(s)\r\n");
#else
    #define debug_freq_collect()
#endif

#define MONITOR_DUG

#include "dev_memory.h"
#include "dev_adc.h"
#include "dev_gpio.h" /* gpio初始化*/
#include "dev_uart.h" /* debug 串口*/

#include "data_tools.h"
#include "ops_can.h"          /* can 驱动接口*/
#include "ops_io_input.h"     /* 16路开关采集*/
#include "ops_io_output.h"    /* 10路普通io输出*/
#include "ops_resistan.h"     /* 4路电阻测量*/
#include "ops_volt_current.h" /* 6路电压电流测量 4路电压 2路电流 1路电源输入电压电流测量*/
#include "ops_pwm_output.h"   /* 2路pwm输出*/
#include "ops_freq_input.h"   /* 4路频率采集*/
#include "ops_storage.h"      /* 存储器接口*/
#include "ops_soft_time.h"    /* 软件定时器*/

typedef enum
{
    STM32F0 = 0,
    STM32F1,
    STM32F2,
    STM32F3,
    STM32F4,
    STM32F7,
    STM32L0,
    STM32L1,
    STM32L4,
    STM32H7
} st_mcu_type;

extern void iwdg_feed(void);
/*
* Nvic config
*/
#define NVIC_SubPriority 0

#define NVIC_TIM8_TIM8_CC_PreemptionPriority 0 /* timer8 输入捕获中断(边沿跳变触发)*/
#define NVIC_TIM6_PreemptionPriority 1         /* timer6 软件定时器(边沿跳变触发)*/
#define NVIC_EXTI_9_5_PreemptionPriority 2     /* 低频采集外部中断*/
#define NVIC_DMA1_PreemptionPriority 4         /* adc 转换通道 响应优先级*/
#define NVIC_CAN_PreemptionPriority 3          /* can 接收*/
#define NVIC_UART1_PreemptionPriority 10       /* uart1 接收 响应优先级*/
#define NVIC_TIM3_PreemptionPriority 5

#define HARD_VERSION "V1.0.1"
#define BSP_VERSION ((uint8_t)1)

extern uint32_t stm32_mcu_id[3];
#endif /* end of include guard: __BIOS_H__ */
