#ifndef __OPS_SOFT_TIME_H__
#define __OPS_SOFT_TIME_H__

#include "board.h"

#define TIME6_MILLISECOND 1 /* 1ms计数频率*/
#define TIME6_CLOCK 10000
/**
 * @desc  : 获取tick (时基 10ms)
 * @param : none
 * @return: 当前tick值
 * @Date  : 2018-12-5 12:29:48
 */
extern uint32_t get_hw_time_tick(void);

extern void irq_timer6_sethook(void (*hook)(void *pram));

#ifdef EN_OPS_SOFT_TIME

    #define SOFT_TIME_EVENT (uint16_t)(1 << 1) /* 软件定时器任务激活事件*/
    #define SOFT_TIME_NAME_MAX 20
    #define SOFT_TIMER_FLAG_DEACTIVATED 0x0 /* 定时器不激活*/
    #define SOFT_TIMER_FLAG_ACTIVATED 0x1   /* 定时器激活*/
    #define SOFT_TIMER_TYPE_ONE_SHOT 0x0    /* 单次定时器*/
    #define SOFT_TIMER_TYPE_PERIODIC 0x2    /* 周期定时器*/

    #define SOFT_TIMER_CTRL_SET_TIME 0x0     /* 设置超时时间*/
    #define SOFT_TIMER_CTRL_GET_TIME 0x1     /* 获取当前时间*/
    #define SOFT_TIMER_CTRL_SET_ONESHOT 0x2  /* 修改定时器为单次 */
    #define SOFT_TIMER_CTRL_SET_PERIODIC 0x3 /* 修改定时器为周期*/
    extern uint8_t task_soft_time_heap[SOFT_TIME_HEAP_SIZE];
#else
    extern uint8_t task_soft_time_heap[1];
#endif

struct tcb_soft_time
{
    char name[SOFT_TIME_NAME_MAX]; /* 软件定时器名称*/
    uint8_t num;                   /* 定时器编号*/
    uint8_t type;
    uint8_t flag;
    int32_t init_tick;
    int32_t free_tick;
    void *parameter;
    void (*fun)(void *parameter);
};

typedef struct tcb_soft_time *timer_t;

extern OS_TID tid_task_soft_time;

extern __task void task_soft_time(void);
extern void soft_time_view(timer_t t);
extern void soft_time_printf_list(void);
extern int soft_time_control(timer_t t, int cmd, void *arg);
extern int soft_time_delete(timer_t *t);
extern int soft_time_stop(timer_t t);
extern int soft_time_start(timer_t t);
extern timer_t soft_time_create(const char *name,
                                void (*fun)(void *parameter),
                                void *parameter,
                                uint32_t time,
                                uint8_t type);

/**
 * @desc  : 根据_nop 指令进行延时,一个_nop指令为1/7200000s 占10个机器指令周期
 * @param : us 值
 * @return: None
 * @Date  : 2018-5-22 10:31:15
 */
extern void delay_us(uint32_t us);
/**
 * @desc  : 1000次 delay_us()
 * @param : ms 值
 * @return: None
 * @Date  : 2019-1-29 09:32:03
 */
extern void delay_ms(uint32_t delayms);

/**
 * @desc  : 根据硬件定时器精确的一个ms
 * @param : ms 值
 * @return: None
 * @Date  : 2019-1-29 09:32:03
 */
extern void tick_delay_ms(uint32_t delayms);

#endif /* end of include guard: __DEV_TIME_H__ */
