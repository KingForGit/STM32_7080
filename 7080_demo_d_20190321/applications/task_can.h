#ifndef __TASK_CAN_H__
#define __TASK_CAN_H__

#include "lib_config.h"

extern OS_TID tid_task_can;
extern uint8_t task_can_heap[CAN_HEAP_SIZE];
 extern timer_t beep_en_start ;
 extern  timer_t beep_en_on;
#define can_item_verify(item) (item > CAN_FIFO_MAX_ITEM) ? printf("- can item > CAN_FIFO_MAX_ITEM .item:%d\r\n", item) : 0;

#define CAN1_RX_ID0 0x18FF8035 /* 测试ID1*/
#define CAN1_RX_ID1 0x18FF8036 /* 测试ID2*/
#define CAN1_RX_ID2 0x18EE0011 /* 测试ID3*/
#define CAN1_RX_ID3 0x1FFFFFA3 /* 测试ID4*/
#define CAN1_RX_ID4 0x1FFFFFA4 /* 测试ID5*/
#define CAN1_RX_ID5 0x1FFFFFA5 /* 测试ID6*/
#define CAN1_RX_ID6 0x1FFFFFA6 /* 测试ID7*/
#define CAN1_RX_ID7 0x1FFFFFA7 /* 测试ID8*/
#define CAN1_RX_ID8 0x1FFFFFA8 /* 测试ID9*/
#define CAN1_RX_ID9 0x1FFFFFA9 /* 测试ID10*/

#pragma pack(1)
struct my_can_recv
{
    uint32_t id;
    uint8_t data[8];
};
#pragma pack()

struct can_info
{
    OS_SEM send_semaphore;   /* can 发送信号量*/
    bool connect_state;      /* can 连接状态 连接正常:FALSE; 连接异常: TRUE*/
    uint32_t cnt_disconnect; /* can 断开的时间 单位:次/10ms*/
    bool can_a20_reset;//can 异常 重启A20
};

struct excitation_info //励磁
{
    bool exc_can;
    bool exc_en;//励磁发送使能状态，true使能发送，false停止发送
    bool exc_state;//励磁当前发送状态，true发送，false停止发送
    uint8_t exc_paragraph;//励磁段数
    uint8_t exc_cycle;//波形占空比 
    uint8_t exc_system;//电压系统
};


/**
 * @desc  : 发送can数据(调用本函数会有一个1ms的硬件延时)
 * @param :
 * @return:
 * @Date  :
 */
extern void can_send_data(uint32_t id, uint8_t *data);
#endif /* end of include guard: __USER_CAN_H__ */
