#include "system_iap.h"

/**
 * @desc  : 根据长度位置计算flash页数量
 * @param : len 长度
 * @return: 页数
 * @Date  : 2019-1-10 15:08:58
 */
__inline uint32_t flash_count_pages(uint32_t len)
{
    return (len % PAGE_SIZE != 0) ? ((len / PAGE_SIZE) + 1) : (len / PAGE_SIZE);
}

/**
 * @desc  : 根据指定地址擦除指定的flash空间
 * @param : add: 擦除的地址; len 擦除的长度
 * @return: false 擦除成功; true 擦除失败
 * @Date  : 2019-1-10 15:10:28
 */
bool flash_area_erase(uint32_t const add, uint32_t len)
{
    FLASH_Status FLASHStatus = FLASH_COMPLETE;
    uint32_t pages_total = 0;
    uint32_t pages_index = 0;

    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    pages_total = flash_count_pages(len);

    /* close interruption*/
    __set_FAULTMASK(1);
    FLASH_Unlock();

    for (pages_index = 0; pages_index < pages_total; pages_index++)
    {
        iwdg_feed();
        FLASHStatus = FLASH_ErasePage(add + (PAGE_SIZE * pages_index));
        if (FLASHStatus != FLASH_COMPLETE)
        {
            printf("- Flash eease page failed, error_num [%d]\r\n", FLASHStatus);
            return true;
        }
    }
    FLASH_Lock();

    /* open interruption*/
    __set_FAULTMASK(0);

    return false;
}

/**
 * @desc  : flase_area_write(uint8_t *data, int32_t length)
 * @param : flash_add 数据写入的地址; data 写入的数据; int32_t 写入的长度
 * @return: false 成功; true 失败
 * @Date  : 2019-1-10 16:01:25
 */
bool flash_area_write(uint32_t *const flash_add, uint8_t *const data, int32_t length)
{
    uint32_t *w_data = NULL;
    uint32_t *r_data = NULL;
    int i;

    w_data = (uint32_t *)data;
    r_data = (uint32_t *)flash_add;

    /* close interruption*/
    __set_FAULTMASK(1);
    FLASH_Unlock();
    for (i = 0; i < length; i += 4)
    {
        if (r_data < (uint32_t *)(ADDR_APP_BAK + APP_DATA_SIZE))
        {
            iwdg_feed();
            FLASH_ProgramWord((uint32_t)r_data, *w_data);
            if (*(uint32_t *)r_data != *(uint32_t *)w_data)
            {
                printf("- Data to check failure.r_data:%p [%x], w_data:%p [%x]\r\n", r_data, *(uint32_t *)r_data, w_data, *(uint32_t *)w_data);
                return true; /*flash write data not same*/
            }

            r_data++;
            w_data++;
        }
        else
        {
            return true;
        }
    }
    FLASH_Lock();
    /* open interruption*/
    __set_FAULTMASK(0);
    return false;
}

/**
 * @desc  : 写入升级标记
 * @param : 升级标记(Y,R,N)
 * @return: 成功 false 失败 true
 * @Date  : 2018-6-16 19:29:012
 */
bool flash_update_mark(uint32_t mark)
{
    uint32_t w_data = 0;
    uint32_t r_data = 0;

    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    w_data = mark;
    w_data = (w_data << 8) & 0xFF00;

    FLASH_Unlock();
    FLASH_ErasePage(ADDR_DATA_FILED);
    FLASH_ProgramWord(ADDR_DATA_FILED, w_data);
    FLASH_Lock();

    r_data = *(uint32_t *)(ADDR_DATA_FILED);

    if (r_data == w_data)
        return false;
    else
        return true;
}
