#ifndef __PRODUCT_H__
#define __PRODUCT_H__

#include "lib_config.h"

#define PRO_OVER_CAN_ID 0x1ffffff0 /* 生产结束响应id*/

#define RES_SEND_CAN_ID 0x1ffffff1  /* 电阻信息发送id*/
#define VOLT_SEND_CAN_ID 0x1ffffff2 /* 电压信息发送id*/
#define CURR_SEND_CAN_ID 0x1ffffff3 /* 电流信息发送id*/
#define SW_SEND_CAN_ID 0x1ffffff4   /* 开关量状态发送id*/
#define MCU_ID_CAN_ID 0x1ffffff5    /* mcu uid */
#define DEV_ID_CAN_ID 0x1ffffff6    /* 设备编号*/

#define PRO_DATA_SIZE ENV_BUFF_SIZE - 5

/* 注意：本结构体对象总大小不可超过95字节,否则造成内存溢出，不可预估的风险*/
#pragma pack(1)
struct product_s
{
    uint8_t dev_id[20];       /* 设备编号*/
    uint8_t over_time[6];     /* 检验结束时间*/
    uint8_t hard_version[20]; /* 硬件版本号*/
    uint8_t soft_version[20]; /* 软件本号*/
    uint32_t key_word;        /* 生产标志 0x5f5f55aa 已经配置过; 其他值，未配置过*/
};
#pragma pack()

extern void pro_verify_can_recv(uint32_t id, uint8_t *msg);
extern struct product_s *get_soft_library_info(void);
extern void view_product_info(struct product_s *data);
#endif /* end of include guard: __PRODUCT_H__ */
