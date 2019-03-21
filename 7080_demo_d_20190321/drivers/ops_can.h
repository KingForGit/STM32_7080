#ifndef __OPS_CAN_H__
#define __OPS_CAN_H__

#include "board.h"

#define CAN1_GPIO_PORT GPIOA
#define CAN1_GPIO_RX GPIO_Pin_11
#define CAN1_GPIO_TX GPIO_Pin_12

#define ID_LIST 0
#define ID_MAKS 1
#define SCASE_32 0
#define SCASE_16 1

#define CAN_CMD_RESET 0x13       /* 复位can*/
#define CAN_CMD_GET_MSG_NUM 0x20 /* 获取fifo中报文的个数*/

#ifdef CAN_TYPE_EXTID
#define __CAN_Filter(index, mode, scale, list_id, mask_id)   \
    {                                                        \
        index, mode, scale,                                  \
            (uint16_t)((uint32_t)list_id >> 13),             \
            (uint16_t)((uint32_t)list_id << 3) | CAN_ID_EXT, \
            (uint16_t)((uint32_t)mask_id >> 13),             \
            (uint16_t)((uint32_t)mask_id << 3) | CAN_ID_EXT  \
    }
#define CAN_PRO_REV1_ID 0x1eeeeeee /* 接收开始测试id*/
#define CAN_PRO_REV2_ID 0x1eeeeeec /* 接收控制开关量采集模式id*/
#define CAN_PRO_REV3_ID 0x1eeeeeeb /* 接收电阻控制*/
#define CAN_PRO_REV4_ID 0x1eeeeeea /* 接收开关量输出高低控制id*/

#else
#define __CAN_Filter(index, mode, scale, list_id, mask_id)                               \
    {                                                                                    \
        index, mode, scale,                                                              \
            (uint16_t)((uint32_t)list_id << 5), 0, (uint16_t)((uint32_t)mask_id << 5), 0 \
    }
#define CAN_PRO_REV1_ID 0x1ee /* 接收开始测试id*/
#define CAN_PRO_REV2_ID 0x1ec /* 接收控制开关量采集模式id*/
#define CAN_PRO_REV3_ID 0x1eb /* 接收电阻控制*/
#define CAN_PRO_REV4_ID 0x1ea /* 接收开关量输出高低控制id*/
#endif

struct can_filter_item
{
    uint32_t number;
    uint32_t mode;  //0-id_mask 1-id_list
    uint32_t scale; //32
    uint16_t id_high;
    uint16_t id_low;
    uint16_t mask_id_high;
    uint16_t mask_id_low;
};

struct can_config
{
    uint32_t baud_rate;
    uint32_t count;
    struct can_filter_item *items;
};

extern void can_open(struct can_config *conf);
extern void can_close(void);
extern void can_control(int cmd, void *args);
extern int can_read(void *buffer, int item);
extern int can_write(CanTxMsg *buffer);
extern void can_status(void);
#endif /* end of include guard: __OPS_CAN_H__ */
