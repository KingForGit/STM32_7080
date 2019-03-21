#ifndef __APP_CAN_IAP_H__
#define __APP_CAN_IAP_H__

#include "board.h"
#include "system_iap.h"

#define EC2612_CAN_IAP_TYPE_CODE (uint8_t)0x02
#define CAN_IAP_BIN_BUFF_LEN 2048
#define CAN_IAP_TIMEOUT 1 /* can升级超时间 1s*/

#define CAN_IAP_REV_STOP 0x1fffffb9   /* 接收暂停命令*/
#define CAN_IAP_REV_DATA 0x1fffffbb   /* 接收升级数据ID*/

#define CAN_VERIFY_OFFSET1 12 /* 生产流水号1*/
#define CAN_VERIFY_OFFSET2 13 /* 生产流水号2*/
#define CAN_VERIFY_OFFSET3 14 /* 生产流水号3*/
#define CAN_VERIFY_OFFSET4 15 /* 生产流水号3*/

struct can_iap_info
{
    struct iap_info parent;
    timer_t _timout;
    timer_t _w_data_time;
    uint8_t v_bsp;                /* mcu 主版本号*/
    uint8_t v_user;               /* 程序子版本号*/
    uint8_t *bin_buff;            /* 存放bin文件的buff*/
    uint16_t buff_offset;         /* 单次接收长度*/
    uint32_t total_can_frame_num; /* 升级的can报文总帧数*/
    uint32_t start_time;          /* 开始升级的时间 单位ms*/
};

extern struct can_iap_info can_iap_info;

extern void can_iap_info_init(uint8_t *const msg);
extern void can_iap_msg_handle(uint32_t id, uint8_t *msg,uint8_t dlc);
#endif /* end of include guard: __APP_CAN_IAP_H__ */
