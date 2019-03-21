#ifndef __DEV_FM25CLXX_H__
#define __DEV_FM25CLXX_H__

#include "board.h"

#define FM25CL64B

#define SPI_DEV SPI2
#define SPI_PRO GPIOB

// #define SPI_CS_PIN GPIO_Pin_12
// #define SPI_SCK_PIN GPIO_Pin_13
// #define SPI_MISO_PIN GPIO_Pin_14
// #define SPI_MOSI_PIN GPIO_Pin_15

/* fm25cl 保护端口未使用*/
//#define FM25CL_HOLD_PORT GPIOB
//#define FM25CL_HOLD_PIN GPIO_Pin_0
//#define FM25CL_WP_PORT GPIOB
//#define FM25CL_WP_PIN GPIO_Pin_1

#define FM25_TIMEOUT 0xfffff

#ifdef FM25CL64B

    /*  寄存器说明:
    |bit  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0 |
    | ---------------------------------------------------|
    |说明 | wpen|  0  |  0  |  0  | bp1 | bp0 | wel |  0 |
    */

    #define FM25_MAX_ADD 8192 /* 最大字节数 8192 byte*/

    #define FM25_WREN 0x06  /* 写使能*/
    #define FM25_WRDI 0x04  /* 写关闭*/
    #define FM25_RDSR 0x05  /* 读状态寄存器*/
    #define FM25_WRSR 0x01  /* 写状态寄存器*/
    #define FM25_READ 0x03  /* 读数据*/
    #define FM25_WRITE 0x02 /* 写存储器数据*/
#endif
// #define FM25_WEL 0x02
// #define FM25_WPEN 0x80

#define fm25cl_cs_high() GPIO_SetBits(SPI_PRO, SPI_CS_PIN)  /* 片选置高 停止操作*/
#define fm25cl_cs_low() GPIO_ResetBits(SPI_PRO, SPI_CS_PIN) /* 片选置低 开始操作*/

#endif /* end of include guard: __DEV_FM25CLXX_H__*/
