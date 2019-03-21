#ifndef __TASK_MONITOR_H__
#define __TASK_MONITOR_H__

#include "lib_config.h"

extern OS_TID tid_task_monitor;
extern uint8_t task_monitor_heap[MONITOR_HEAP_SIZE];

#define DEV_WORK_TIME_SAVE_FREQ 3600  /* 累计工时存储频率 单位s*/
/******************************************************************************
*                     应用数据存储定义地区,不可随意插入,只可在末尾追加
******************************************************************************/
#define STORAGE_MIRROR_LEN 1024

struct applications_s
{
    uint32_t dev_work_time; /* 设备累计工作时间*/
    uint8_t demo_1;         /* 演示变量1*/
    uint8_t demo_2;         /* 演示变量2*/
    uint16_t demo_3;        /* 演示变量3*/
    uint16_t demo_4;        /* 演示变量4*/
    //...
    //...
    //...
};

struct application_flag
{
    uint8_t a20_rest_flag; /* A20电压检测状态*/
    uint8_t demo_1;         /* 演示变量1*/
    uint8_t demo_2;         /* 演示变量2*/
    uint16_t demo_3;        /* 演示变量3*/
    uint16_t demo_4;        /* 演示变量4*/
    //...
    //...
    //...
};
extern struct applications_s *const application_s;

extern void led_handle(uint32_t tick);
extern void read_application_info(void);
extern void write_application_info(void);
extern void modify_app_save_handle(uint8_t *const data, uint32_t const len);
extern void ResetA20(void);

#endif /* end of include guard: __TASK_MONITOR_H__ */
