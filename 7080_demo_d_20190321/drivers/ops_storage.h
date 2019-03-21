#ifndef __OPS_STORAGE_H__
#define __OPS_STORAGE_H__

#include "board.h"

#define ENV_BUFF_SIZE 100
#define PRO_STORAGE_START_ADD (FM25_MAX_ADD - ENV_BUFF_SIZE) /* 存储设备生产检测信息*/
/**
 * @desc  : 存储器初始化(默认已经打开)
 * @param : none
 * @return: none
 * @Date  : 2018-11-28 10:05:33
 */
extern void storage_open(void);

/**
 * @desc  : 存储器读写端口关闭，需要重新打开才可读写数据
 * @param : none
 * @return: none
 * @Date  : 2018-11-28 10:05:33
 */
extern void storage_close(void);

/**
 * @desc  : 格式化存储器(擦除所有数据，填充0xFF)
 * @param : none
 * @return: 成功:0; 失败: -1
 * @Date  : 2018-11-28 10:00:39
 */
extern int32_t storage_format(void);

/**
 * @desc  : 擦除指定位置数据
 * @param : address; 擦除的起始位置； size 擦除大小
 * @return: 成功:0; 失败: -1
 * @Date  : 2018-11-29 14:53:06
 */
extern int32_t storage_erasure(uint32_t address, uint16_t size);

/**
 * @desc  : 写数据
 * @param : address 写地址; *buff 写入数据的buff; size 读取数据的大小;
 * @return: 成功:实际写入的字节数; 失败: -1
 * @Date  : 2018-11-25 20:43:00
 */
extern int32_t storage_write(uint32_t address, uint8_t *buff, uint32_t size);

/**
 * @desc  : 读数据
 * @param : address 读取地址 *buff 存放数据的buff; size 读取数据的大小;
 * @return: 成功:实际读出的字节数; 失败: -1
 * @Date  : 2018-11-25 20:43:00
 */
extern int32_t storage_read(uint32_t address, uint8_t *buff, uint32_t size);

/**
 * @desc  : 读 生产数据存储(返回ascii格式信息)
 * @param : *buff 存放数据的buff
 * @return: 成功:实际读出的字节数; 失败: -1
 * @Date  : 2018-12-11 19:11:39
 */
int32_t pro_storage_read(uint8_t *buff);
#endif /* end of include guard: __POS_STORAGE_H__ */
