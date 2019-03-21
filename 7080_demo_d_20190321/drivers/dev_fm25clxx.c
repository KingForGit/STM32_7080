#include "dev_fm25clxx.h"

//#define DEBUG_FM25
#ifdef DEBUG_FM25
    #define fm25_dbg(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define fm25_dbg(fmt, ...)
#endif
extern void gpio_set_mode(GPIO_TypeDef *port, uint16_t pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);
//#define FM25_LOCK
#ifdef FM25_LOCK
OS_SEM fm25_semaphore;
static void fm25_lock_create()
{
    os_sem_init(&fm25_semaphore, 1);
}
static void fm25_unlock()
{
    os_sem_send(&fm25_semaphore);
}
static bool fm25_lock()
{
    OS_RESULT res;
    res = os_sem_wait(&fm25_semaphore, 0xffff);
    return (res == OS_R_OK) ? false : true;
}
#else
#define fm25_unlock() (void)0
#define fm25_lock_create() (void)0
static bool fm25_lock()
{
    return false;
}
#endif

/**
 * @desc  : spi 发送/接收 一个字节
 * @param : data 发送的数据; *status 操作状态(false:读取成功; true:读取超时)
 * @return: 接收的字节
 * @Date  : 2018-11-25 21:03:49
 */
static uint8_t spi_readwrite(uint8_t data, bool *status)
{
    uint8_t value;
    uint32_t time_cnt = FM25_TIMEOUT;

    while (SPI_I2S_GetFlagStatus(SPI_DEV, SPI_I2S_FLAG_TXE) == RESET && --time_cnt > 0)
        ;

    if (time_cnt <= 0)
    {
        *status = true;
        return 0;
    }
    SPI_I2S_SendData(SPI_DEV, data);

    time_cnt = FM25_TIMEOUT;
    while (SPI_I2S_GetFlagStatus(SPI_DEV, SPI_I2S_FLAG_RXNE) == RESET && --time_cnt > 0)
        ;
    if (time_cnt <= 0)
    {
        *status = true;
        return 0;
    }
    value = SPI_I2S_ReceiveData(SPI_DEV);
    return value;
}
/**
 * @desc  : 读状态寄存器
 * @param : none
 * @return: none
 * @Date  : 2018-11-29 14:29:46
 */
static uint8_t fm25_read_status(void)
{
    uint8_t tmp;
    bool status = false;

 //   fm25cl_cs_low();
    spi_readwrite(FM25_RDSR, &status); /* 读状态寄存器*/
    tmp = spi_readwrite(0xff, &status);
//    fm25cl_cs_high();
    return tmp;
}
/**
 * @desc  : 写状态寄存器
 * @param : none
 * @return: flase 正常; true 超时
 * @Date  : 2018-11-29 12:52:53
 */
static bool fm25_write_status(uint8_t value)
{
    uint8_t res = 0;
    bool status = false;

   // fm25cl_cs_low();
    spi_readwrite(FM25_WREN, &status); /* 写使能*/
 //   fm25cl_cs_high();

 //   fm25cl_cs_low();
    spi_readwrite(FM25_WRSR, &status); /* 写寄存器指令*/
    spi_readwrite(value, &status);
 //   fm25cl_cs_high();

    res = fm25_read_status();
    fm25_dbg("- fm25 status :0x%02x\r\n", res);

    return (res == value) ? false : true;
}

static bool open_wp()
{
    return fm25_write_status(0x0c);
}
static bool close_wp()
{
    return fm25_write_status(0x00);
}

int32_t fm25_read(uint8_t *buff, uint32_t size, uint32_t address)
{
    uint32_t index = 0;
    bool status = false;

    /* 地址溢出校验*/
    size = ((size + address) > FM25_MAX_ADD) ? (FM25_MAX_ADD - address) : size;

    fm25_lock();
  //  fm25cl_cs_low();
    spi_readwrite(FM25_READ, &status);
    spi_readwrite(((address >> 8) & 0xff), &status);
    spi_readwrite((address & 0xff), &status);
    for (index = 0; index < size; index++)
    {
        *buff++ = spi_readwrite(0xff, &status);
        if (status == true)
        {
            fm25_unlock();
            printf("- Read fm25clxx time out\r\n");
            return -1;
        }
        address++;
    }
//    fm25cl_cs_high();
    fm25_unlock();
    return index;
}

int32_t fm25_write(uint8_t *buff, uint32_t size, uint32_t address)
{
    uint32_t index = 0;
    bool res = true;
    bool status = false;

    /* 地址溢出校验*/
    index = ((size + address) > FM25_MAX_ADD) ? (FM25_MAX_ADD - address) : size;

    fm25_lock();

    res = close_wp();
    if (res == true)
        return -1;

    /* 发送写请求指令*/
//    fm25cl_cs_low();
    spi_readwrite(FM25_WREN, &status);
 //   fm25cl_cs_high();

//    fm25cl_cs_low();
    spi_readwrite(FM25_WRITE, &status);
    spi_readwrite(((address >> 8) & 0xff), &status);
    spi_readwrite((address & 0xff), &status);
    while (index > 0)
    {
        spi_readwrite((*buff++), &status);

        if (status == true)
        {
            fm25_unlock();
            printf("- Write fm25clxx time out.\r\n");
            return -1;
        }
        index--;
        address++;
    }
 //   fm25cl_cs_high();
    fm25_unlock();

    open_wp();

    return size;
}
int32_t fm_erasure(uint16_t size, uint32_t address)
{
    uint32_t index = 0;
    bool res = false;
    bool status = false;

    fm25_lock();

    res = close_wp();
    if (res == true)
        return -1;

    /* 发送写请求指令*/
//    fm25cl_cs_low();
    spi_readwrite(FM25_WREN, &status);
//    fm25cl_cs_high();

 //   fm25cl_cs_low();
    spi_readwrite(FM25_WRITE, &status);
    spi_readwrite(((address >> 8) & 0xff), &status);
    spi_readwrite((address & 0xff), &status);
    while (index < size)
    {
        spi_readwrite(0xff, &status);
        if (status == true)
        {
            fm25_unlock();
            printf("- Format fm25clxx time out.\r\n");
            return -1;
        }
        index++;
    }
//    fm25cl_cs_high();
    fm25_unlock();

    open_wp();
    return 0;
}

void fm25_spi_init()
{
    SPI_InitTypeDef SPI_InitStructure;

    if (SPI_DEV == SPI1)
    {
    }
    else if (SPI_DEV == SPI2)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    }
    else
    {
        printf("- Spi error fm25clxx_spi.c\r\n");
    }

  //  gpio_set_mode(SPI_PRO, SPI_SCK_PIN, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);        /* Configure SCK*/
 //   gpio_set_mode(SPI_PRO, SPI_MISO_PIN, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Configure MISO*/
  //  gpio_set_mode(SPI_PRO, SPI_MOSI_PIN, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);       /* Configure MOSI*/
  //  gpio_set_mode(SPI_PRO, SPI_CS_PIN, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);        /* Configure CS*/
  //  fm25cl_cs_high();                                                              /* Chip Select high */

    SPI_I2S_DeInit(SPI_DEV);
    SPI_Cmd(SPI_DEV, DISABLE);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; /* 设置SPI全双工*/
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                      /* 设置SPI工作模式为主工作模式*/
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  /* 8位帧结构*/
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                         /* 时钟悬空低*/
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                       /* 数据捕获在第一个时钟沿*/
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                          /* NSS信号由软件管理*/
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; /* 时钟预分频为4*/
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                 /* 数据传输从最高为开始*/
    SPI_InitStructure.SPI_CRCPolynomial = 7;                           /* CRC值计算的多项式*/

    SPI_Init(SPI_DEV, &SPI_InitStructure);
    SPI_Cmd(SPI_DEV, ENABLE);

    fm25_lock_create();
    open_wp();
}
 void fm25_spi_close()
 {}
// {
//     gpio_set_mode(SPI_PRO, SPI_SCK_PIN, GPIO_Mode_AIN, GPIO_Speed_50MHz);  /* Configure SCK*/
//     gpio_set_mode(SPI_PRO, SPI_MISO_PIN, GPIO_Mode_AIN, GPIO_Speed_50MHz); /* Configure MISO*/
//     gpio_set_mode(SPI_PRO, SPI_MOSI_PIN, GPIO_Mode_AIN, GPIO_Speed_50MHz); /* Configure MOSI*/
//     gpio_set_mode(SPI_PRO, SPI_CS_PIN, GPIO_Mode_AIN, GPIO_Speed_50MHz);   /* Configure CS*/
//     SPI_Cmd(SPI_DEV, DISABLE);
// }
