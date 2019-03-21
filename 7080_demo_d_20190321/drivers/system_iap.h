#ifndef __SYSTEM_IAP__
#define __SYSTEM_IAP__

#include "board.h"

#define BOOTLOADER_SIZE (uint32_t)(1024 * 10) /* bootloader size*/
#define DATA_FILED_SIZE (uint32_t)(1024 * 2)  /* iap mark*/
#define APP_DATA_SIZE (uint32_t)(1024 * 100)  /* app data size*/

#define ADDR_BOOTLOADER 0x08000000
#define ADDR_DATA_FILED (ADDR_BOOTLOADER + BOOTLOADER_SIZE)
#define ADDR_APP_RUN (ADDR_DATA_FILED + DATA_FILED_SIZE)
#define ADDR_APP_BAK (ADDR_APP_RUN + APP_DATA_SIZE)

#if defined(STM32F10X_MD) || defined(STM32F10X_MD_VL)
    #define PAGE_SIZE (0x400)    /* 1 Kbyte */
    #define FLASH_SIZE (0x20000) /* 128 KBytes */
#elif defined STM32F10X_CL
    #define PAGE_SIZE (0x800)    /* 2 Kbytes */
    #define FLASH_SIZE (0x40000) /* 256 KBytes */
#elif defined STM32F10X_HD || defined(STM32F10X_HD_VL)
    #define PAGE_SIZE (0x800)    /* 2 Kbytes */
    #define FLASH_SIZE (0x80000) /* 512 KBytes */
#elif defined STM32F10X_XL
    #define PAGE_SIZE (0x800)     /* 2 Kbytes */
    #define FLASH_SIZE (0x100000) /* 1 MByte */
#else
    #error "Please select first the STM32 device to be used (in stm32f10x.h)"
#endif

struct iap_info
{
    volatile bool mode;                  /* 处于升级模式: true; 未处于升级模式:false*/
    volatile uint8_t type_mark;          /* 升级类型标记*/
    volatile uint32_t get_total_size;    /* 升级文件的总大小*/
    volatile uint16_t get_total_verify;  /* 升级文件的总校验和*/
    volatile uint32_t rev_total_size;    /* 实际接收的总大小*/
    volatile uint16_t rev_total_verify;  /* 实际接收的总校验和*/
    volatile uint32_t save_wirite_index; /* 存储写索引*/
    volatile uint32_t save_read_index;   /* 存储读索引*/
    volatile uint32_t tick;              /* tick计数*/
};
extern bool flash_area_erase(uint32_t const add, uint32_t len);
extern bool flash_area_write(uint32_t *const flash_add, uint8_t *const data, int32_t length);
extern bool flash_update_mark(uint32_t mark);
#endif /* end of include guard: __SYSTEM_IAP__ */
