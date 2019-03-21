#ifndef __LIB_CONFIG_H__
#define __LIB_CONFIG_H__

#define APP_VERSION ((uint16_t)1)

#define IWDG_TIME_OUT 10000 /* 看门狗超时时间 单位ms*/
#define EN_OPS_SOFT_TIME    /* soft time switch*/

/******************************************************************************
*                                 debug config
******************************************************************************/
 //#define DEBUG_ADC          /* adc采集调试开关*/
 #define DEBUG_RES_COLLECT   /* 电阻采集调试开关*/
 #define DEBUG_VOLT_COLLECT /* 电压和电流采集调试开关*/
 #define DEBUG_SW_COLLECT   /* 开关量采集调试开关*/
 #define DEBUG_FREQ_COLLECT /* 频率采集调试*/

/******************************************************************************
*                                 can config
******************************************************************************/
#define CAN_TYPE_EXTID       /* can id类型*/
#define CAN_FIFO_MAX_ITEM 50 /* can队列缓冲报文条数*/
#define CAN_BUFF_MAX_INDEX (sizeof(CanRxMsg) * CAN_FIFO_MAX_ITEM)

/******************************************************************************
*                                 thread config
******************************************************************************/
#define KEYPAD_CMD_HEAP_SIZE (uint32_t)1024                   /* keypad cmd thread heap size (byte)*/
#define UART_CMD_HEAP_SIZE (uint32_t)1024                   /* uart cmd thread heap size (byte)*/
#define MONITOR_HEAP_SIZE (uint32_t)1024                    /* monitor thread heap size (byte)*/
#define CAN_HEAP_SIZE (uint32_t)(CAN_BUFF_MAX_INDEX + 1024) /* can cmd thread heap size (byte)*/

#ifdef EN_OPS_SOFT_TIME
    #define SOFT_TIME_HEAP_SIZE 1024 /* soft time task heap (byte)*/
    #define SOFT_TIME_NUM_MAX 32     /* soft time max num*/
    #define SOFT_TIME_INTERVAL 10    /* soft time interval(ms)*/
#endif

#ifndef CAN_TYPE_EXTID
    #define CAN_TYPE_STDID /* 默认设置标准帧*/
#endif

//DAC ThresHold
#define IN_GROUP_1_ThresHold			1
#define IN_GROUP_2_ThresHold			2
#define IN_GROUP_3_ThresHold			3
#define IN_GROUP_4_ThresHold			4
#define IN_GROUP_5_ThresHold			5
#define IN_GROUP_6_ThresHold			6

#include "board.h"
#define GET_STM32_MCU_ID(id) sprintf(id, "%x%x%x", stm32_mcu_id[0], stm32_mcu_id[1], stm32_mcu_id[2])

#endif /* end of include guard: __LIB_CONFIG_H__ */
