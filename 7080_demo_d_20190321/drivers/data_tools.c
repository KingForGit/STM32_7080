/**
 * @author: emlsyx
 * @email: yangx_1118@163.com
 * @create: date 2018-07-13 03:14:41
 * @modify: date 2018-07-13 03:14:41
 * @desc: [description]
*/
#include "data_tools.h"

/**
 * @desc  : 字符串数字转换成hex数组
 * @param : des 目标地址
 *          src 待转换字符串地址
 *          size 带转换字符串字节数
 * @return: None
 * @date  : 2018-4-21 11:01:19
 */
void ascii_to_hex(char *des, char *src, int size)
{
    uint16_t i = 0;
    uint16_t j = 0;
    uint8_t high;
    uint8_t low;
    while (i < size)
    {
        if (src[i] <= '9' && src[i] >= '0')
            high = src[i] - '0';
        else
            high = src[i] - 'A' + 0xa;
        i++;
        if (src[i] <= '9' && src[i] >= '0')
            low = src[i] - '0';
        else
            low = src[i] - 'A' + 0xa;
        des[j] = low + (high << 4);
        j++;
        i++;
    }
}
void hex_to_ascii_print(char hex)
{
    char high;
    char low;

    high = hex >> 4;
    low = hex & 0xf;
    if (high > 9)
        high = high + 'A' - 0xA;
    else
        high = high + '0';
    if (low > 9)
        low = low + 'A' - 0xA;
    else
        low = low + '0';

    printf("%c", high);
    printf("%c", low);
    printf(" ");
}
/**
 * @desc  : 检查输入的字符串中是否在指定的区间范围。字符串全0也不合法
 * @param : *range 指定范围;r_num 范围个数;*data 需要检查的数据地址; len 需要检查的数据长度
 * @return: 数据合法返回 0; 不合法,返回这个字符的位置
 * @Date  : 2018-6-16 22:13:09
 */
int16_t check_data_in_assign_range(struct check_range range[], uint8_t r_num, uint8_t *data, uint16_t len)
{
    uint8_t n = 0;
    uint16_t i = 0;
    uint16_t c_cnt = 0;

    for (i = 0; i < len; i++)
    {
        for (n = 0; n < r_num; n++)
        {
            if ((data[i] < range[n].min) || (data[i] > range[n].max))
            {
                continue; /* 数据不在合法范围内,退出本次循环,判断下一个范围区间*/
            }
            else
            {
                break; /* 数据合法,退出循环,判断下一个字节*/
            }
        }
        if (data[i] == '0')
            c_cnt++;

        if ((n >= r_num) || (c_cnt >= len))
            return i + 1;
    }
    return 0;
}
uint16_t my_htons(uint16_t data)
{
    return (((data & 0xff00) >> 8) | ((data & 0x00ff) << 8));
}
uint32_t my_htonl(uint32_t data)
{
    return (((data & 0xff000000) >> 24) | ((data & 0x00ff0000) >> 8) |
            ((data & 0x0000ff00) << 8) | ((data & 0x000000ff) << 24));
}
/**
 * @desc  : 从字符串src中拷贝字符ch之前的内容到des
 * @param : src 原数据首地址
 *          ch 原数据中的指定字符
 *          des 目标地址
 * @return: offset 拷贝字节数+1
 */
uint8_t get_given_string(uint8_t *src, uint8_t ch, uint8_t *des)
{
    uint8_t offset = 0;

    while (*src != ch)
    {
        *des++ = *src++;
        offset++;
    }

    offset++; //跳过 ch

    return offset;
}
/**
 * @desc  : 从src中获取ch之前的数字字符串并转换成数字
 * @param : src 原数据首地址（一定是数字开头）
 *          ch 原数据中的指定字符
 *          des 转换后的数字
 * @return: offset 数字字符长度+1
 */
uint8_t get_given_number(uint8_t *src, uint8_t ch, uint16_t *des)
{
    uint8_t offset = 0;
    uint16_t value = 0;

    while (*src != ch)
    {
        value *= 10;
        value += (*src - '0');
        src++;
        offset++;
    }

    offset++; //跳过 ch
    *des = value;
    return offset;
}
/**
 * @desc  : c库中的strstr,屏蔽strstr不能检索\0的
 * @param : s1 源字符串  s2 查找的字符串
 * @return: 出现的位置
 * @date  : 2018-7-1 21:02:04
 */
char *my_strstr(char *s1, char *s2, int max_len)
{
    int i = 0, k = 0, l2;

    l2 = strlen(s2);
    for (i = 0; i < max_len; i++)
    {
        if (s1[i] == s2[k])
        {
            if ((k++) == l2)
                return &s1[i - l2];
        }
    }
    return NULL;
}
/**
 * @desc  : 8位异或校验
 * @param : *buf 校验的地址 len 校验的数据长度
 * @return: 校验结果
 * @date  : 2018-4-21 11:16:24
 */
uint8_t xor_8bit_verify(uint8_t *buf, uint32_t len)
{
    uint8_t value = 0;
    uint32_t i = 0;

    for (i = 0; i < len; i++)
    {
        value = value ^ (*(buf + i));
    }

    return value;
}
/**
 * @desc  : 16位累加校验和
 * @param : *buf 校验的地址 len 校验的数据长度
 * @return: 校验结果
 * @date  : 2018-4-21 11:19:04
 */
uint16_t sum_16bit_verify(uint8_t *buf, uint32_t len)
{
    uint32_t i = 0;
    uint32_t sum = 0;
    uint16_t value = 0;

    for (i = 0; i < len; i++)
    {
        sum += *buf++;
    }
    value = (sum >> 16) + (sum & 0xffff);
    return value;
}
/**
 * @desc  : 生产一个 x~y 的随机数(包含x,不包含y)
 * @param : x 最小值;  y 最大值
 * @return: 随机值
 * @Date  : 2018-6-6 18:27:36
 */
uint16_t my_rand(uint16_t x, uint16_t y)
{
    uint16_t random = 0;
    static uint32_t srand_fre = 0;


    /* 每500个数据更新一次随机数种子*/
    if (((srand_fre++) % 500) == 0)
    {
        srand_fre = 1;
        srand((unsigned int)srand_fre);
    }

    random = (rand() % (y - x)) + x; /* 包含x，不包含y*/
    //random = (rand() % (y-x))+ x + 1/* 不包含x，包含y*/
    //random = (rand() % (y-x+1))+ x;/* 包含x，也包含y*/

    if ((random >= y) && (random < x))
        random = x;
    return random;
}
/**
 * @desc  : 内存打印
 * @param : *data:数据指针,len:数据长度 mode: 打印模式,
 * @return: None
 * @Date  : 2018-6-5 09:50:16
 */
void mem_printf(uint8_t *data, uint16_t len,print_mode mode)
{
    uint16_t i = 0;

    printf("\r\n");
    for (i = 0; i < len; i++)
    {
        switch (mode)
        {
        case PRINT_HEX:
            printf("%02X ", data[i]);
            break;
        case PRINT_DEC:
            printf("%d ", data[i]);
            break;
        case PRINT_CHAR:
            printf("%c", data[i]);
            break;
        default:
            break;
        }
    }
    printf("\r\n");
}
