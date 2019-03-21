/**
 * @author: emlsyx
 * @email:  yangx_1118@163.com
 * @create: date 2018-11-28 09:36:14
 * @modify: date 2018-11-28 09:36:14
 * @desc:   [description]
*/
#include "ops_storage.h"
#include "dev_fm25clxx.h"

extern void fm25_spi_init(void);
extern void fm25_spi_close(void);
extern int32_t fm25_write(uint8_t *buff, uint32_t size, uint32_t address);
extern int32_t fm25_read(uint8_t *buff, uint32_t size, uint32_t address);
extern int32_t fm_erasure(uint16_t size, uint32_t address);

/**
 * @desc  : 存储器初始化
 * @param : none
 * @return: none
 * @Date  : 2018-11-28 10:05:33
 */
void storage_open()
{
    fm25_spi_init();
}
/**
 * @desc  : 写数据
 * @param : address 写地址; *buff 写入数据的buff; size 读取数据的大小;
 * @return: 成功:实际写入的字节数; 失败: -1
 * @Date  : 2018-11-25 20:43:00
 */
int32_t storage_write(uint32_t address, uint8_t *buff, uint32_t size)
{
    int res;

    if (address >= PRO_STORAGE_START_ADD)
        return -1;

    res = fm25_write(buff, size, address);
    return res;
}

/**
 * @desc  : 读数据
 * @param : address 读取地址 *buff 存放数据的buff; size 读取数据的大小;
 * @return: 成功:实际读出的字节数; 失败: -1
 * @Date  : 2018-11-25 20:43:00
 */
int32_t storage_read(uint32_t address, uint8_t *buff, uint32_t size)
{
    int res;
    res = fm25_read(buff, size, address);
    return res;
}
/**
 * @desc  : 擦除指定位置数据
 * @param : none
 * @return: 成功:0; 失败: -1
 * @Date  : 2018-11-28 10:00:39
 */
int32_t storage_erasure(uint32_t address, uint16_t size)
{
    int res;
    res = fm_erasure(size, address);
    return res;
}
/**
 * @desc  : 格式化存储器
 * @param : none
 * @return: 成功:0; 失败: -1
 * @Date  : 2018-11-28 10:00:39
 */
int32_t storage_format()
{
    int res;
    res = fm_erasure(PRO_STORAGE_START_ADD, 0);
    return res;
}
/**
 * @desc  : 关闭存储器
 * @param :
 * @return:
 * @Date  :
 */
void storage_close()
{
    fm25_spi_close();
}

/**
 * @desc  : 写 生产数据存储
 * @param : *buff 写入数据的buff
 * @return: 成功:实际写入的字节数; 失败: -1
 * @Date  : 2018-12-11 19:05:07
 */
int32_t pro_storage_write(uint8_t *buff)
{
    int res;
    res = fm25_write(buff, ENV_BUFF_SIZE, PRO_STORAGE_START_ADD);
    return res;
}
/**
 * @desc  : 读 生产数据存储
 * @param : *buff 存放数据的buff
 * @return: 成功:实际读出的字节数; 失败: -1
 * @Date  : 2018-12-11 19:11:39
 */
int32_t pro_storage_read(uint8_t *buff)
{
    int res;

    res = fm25_read(buff, ENV_BUFF_SIZE, PRO_STORAGE_START_ADD);
    return res;
}
