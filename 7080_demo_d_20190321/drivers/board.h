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
    #define debug_res_collect() printf("\r\n- ͨ����  �ɼ�״̬  �ɼ���λ  ƥ�����1V(mV)  ƥ�����2(k��)  ��λ��Сֵ(��)  ��λ���ֵ(��)  �������(��)\r\n");
#else
    #define debug_res_collect()
#endif

#ifdef DEBUG_VOLT_COLLECT
    #define debug_volt_current_collect() printf("\r\n-   ͨ������     Adc ԭʼֵ(mV) ��ѹ����1(k��)  ��ѹ����2(k��) ��ѹ��  ��ǰ��ѹ(v)  ��ǰ����(mA)\r\n");
#else
    #define debug_volt_current_collect()
#endif

#ifdef DEBUG_SW_COLLECT
    #define debug_sw_collect() printf("\r\n-   ͨ������  �ɼ�ģʽ   Dac�Ƚϵ�ѹ(v)  ��ǰ״̬\r\n");
#else
    #define debug_sw_collect()
#endif

#ifdef DEBUG_FREQ_COLLECT
    #define debug_freq_collect() printf("\r\n- ͨ����  �ɼ�״̬  �ɼ�ģʽ  ��������   ����(��/s) Ƶ��(Hz)  ����(us) ռ�ձ�  Tick(s)\r\n");
#else
    #define debug_freq_collect()
#endif

#define MONITOR_DUG

#include "dev_memory.h"
#include "dev_adc.h"
#include "dev_gpio.h" /* gpio��ʼ��*/
#include "dev_uart.h" /* debug ����*/

#include "data_tools.h"
#include "ops_can.h"          /* can �����ӿ�*/
#include "ops_io_input.h"     /* 16·���زɼ�*/
#include "ops_io_output.h"    /* 10·��ͨio���*/
#include "ops_resistan.h"     /* 4·�������*/
#include "ops_volt_current.h" /* 6·��ѹ�������� 4·��ѹ 2·���� 1·��Դ�����ѹ��������*/
#include "ops_pwm_output.h"   /* 2·pwm���*/
#include "ops_freq_input.h"   /* 4·Ƶ�ʲɼ�*/
#include "ops_storage.h"      /* �洢���ӿ�*/
#include "ops_soft_time.h"    /* �����ʱ��*/

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

#define NVIC_TIM8_TIM8_CC_PreemptionPriority 0 /* timer8 ���벶���ж�(�������䴥��)*/
#define NVIC_TIM6_PreemptionPriority 1         /* timer6 �����ʱ��(�������䴥��)*/
#define NVIC_EXTI_9_5_PreemptionPriority 2     /* ��Ƶ�ɼ��ⲿ�ж�*/
#define NVIC_DMA1_PreemptionPriority 4         /* adc ת��ͨ�� ��Ӧ���ȼ�*/
#define NVIC_CAN_PreemptionPriority 3          /* can ����*/
#define NVIC_UART1_PreemptionPriority 10       /* uart1 ���� ��Ӧ���ȼ�*/
#define NVIC_TIM3_PreemptionPriority 5

#define HARD_VERSION "V1.0.1"
#define BSP_VERSION ((uint8_t)1)

extern uint32_t stm32_mcu_id[3];
#endif /* end of include guard: __BIOS_H__ */
