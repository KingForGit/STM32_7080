#ifndef __DEV_BEEP_H__
#define __DEV_BEEP_H__

#include "board.h"

#define TIME3_MILLISECOND 19 /*TIME3_MILLISECOND * 10us计数*/
#define TIME3_CLOCK 100000

#define BEEP_EN_EVENT (uint16_t)(1 << 1) /* 蜂鸣器激活事件---*/
struct beep_info
{
    bool can_beep_en;//通过can 收到蜂鸣器控制状态
 //   bool can_beep_change;//通过can 收到蜂鸣器控制状态改变
    uint16_t BeepOnTime;//通过can 收到蜂鸣器鸣叫间隔
    uint8_t freq;//IO变化频率 
};
extern struct beep_info beep_data;

extern void beep_start_handle( void *parameter);
//extern void hw_time3_init( void *parameter);
//--gol

// extern uint32_t get_hw_time_tick(void);

// extern void irq_timer6_sethook(void (*hook)(void *pram));

// #ifdef EN_OPS_SOFT_TIME


//     #define SOFT_TIME_NAME_MAX 20
//     #define SOFT_TIMER_FLAG_DEACTIVATED 0x0 /* 定时器不激活*/
//     #define SOFT_TIMER_FLAG_ACTIVATED 0x1   /* 定时器激活*/
//     #define SOFT_TIMER_TYPE_ONE_SHOT 0x0    /* 单次定时器*/
//     #define SOFT_TIMER_TYPE_PERIODIC 0x2    /* 周期定时器*/

//     #define SOFT_TIMER_CTRL_SET_TIME 0x0     /* 设置超时时间*/
//     #define SOFT_TIMER_CTRL_GET_TIME 0x1     /* 获取当前时间*/
//     #define SOFT_TIMER_CTRL_SET_ONESHOT 0x2  /* 修改定时器为单次 */
//     #define SOFT_TIMER_CTRL_SET_PERIODIC 0x3 /* 修改定时器为周期*/
//     extern uint8_t task_soft_time_heap[SOFT_TIME_HEAP_SIZE];
// #else
//     extern uint8_t task_soft_time_heap[1];
// #endif

// struct tcb_soft_time
// {
//     char name[SOFT_TIME_NAME_MAX]; /* 软件定时器名称*/
//     uint8_t num;                   /* 定时器编号*/
//     uint8_t type;
//     uint8_t flag;
//     int32_t init_tick;
//     int32_t free_tick;
//     void *parameter;
//     void (*fun)(void *parameter);
// };

// typedef struct tcb_soft_time *timer_t;



// extern __task void task_soft_time(void);
// extern void soft_time_view(timer_t t);
// extern void soft_time_printf_list(void);
// extern int soft_time_control(timer_t t, int cmd, void *arg);
// extern int soft_time_delete(timer_t *t);
// extern int soft_time_stop(timer_t t);
// extern int soft_time_start(timer_t t);
// extern timer_t soft_time_create(const char *name,
//                                 void (*fun)(void *parameter),
//                                 void *parameter,
//                                 uint32_t time,
//                                 uint8_t type);




#endif /* end of include guard: __DEV_BEEP_H__ */
