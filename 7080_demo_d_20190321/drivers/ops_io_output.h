#ifndef __OPS_IO_OUTPUT_H__
#define __OPS_IO_OUTPUT_H__

#include "board.h"

#define OUT_CH_1 (uint32_t)(0x00000001)
#define OUT_CH_2 (uint32_t)(0x00000002)
#define OUT_CH_3 (uint32_t)(0x00000004)
#define OUT_CH_4 (uint32_t)(0x00000008)
#define OUT_CH_5 (uint32_t)(0x00000010)
#define OUT_CH_6 (uint32_t)(0x00000020)
#define OUT_CH_7 (uint32_t)(0x00000040)
#define OUT_CH_8 (uint32_t)(0x00000080)
#define OUT_CH_9 (uint32_t)(0x00000100)
#define OUT_CH_10 (uint32_t)(0x00000200)
#define OUT_CH_11 (uint32_t)(0x00000400)
#define OUT_CH_12 (uint32_t)(0x00000800)
#define OUT_CH_ALL (uint32_t)(0xfff) /* 1111 1111 1111 */

enum OUT_MODE
{
    IO_LOW = 1,
    IO_HIGH = 2,
};

/**
 * @desc  : 普通io输出配置
 * @param : ch 通道号; mode 输出模式(IO_HIGH/IO_LOW)
 * @return: none
 * @Date  : 2018-11-30 17:28:38
 */
extern void set_switch_output(uint32_t ch, enum OUT_MODE mode);

#endif /* end of include guard: __POS_IO_OUTPUT_H__ */
