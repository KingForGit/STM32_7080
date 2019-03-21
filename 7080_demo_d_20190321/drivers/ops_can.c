/**
 * @author: emlsyx
 * @email:  yangx_1118@163.com
 * @create: date 2018-12-07 08:41:43
 * @modify: date 2018-12-07 08:41:43
 * @desc:   [description]
*/
#include "ops_can.h"

#define CAN_OK 0

/* 计算波特率*/
#define BS1SHIFT 16
#define BS2SHIFT 20
#define RRESCLSHIFT 0
#define SJWSHIFT 24
#define BS1MASK ((0x0F) << BS1SHIFT)
#define BS2MASK ((0x07) << BS2SHIFT)
#define RRESCLMASK (0x3FF << RRESCLSHIFT)
#define SJWMASK (0x3 << SJWSHIFT)

#define MK_BKCAN_BAUD(SJW, BS1, BS2, PRES) \
    ((SJW << SJWSHIFT) | (BS1 << BS1SHIFT) | (BS2 << BS2SHIFT) | (PRES << RRESCLSHIFT))

struct _status_can
{
    uint32_t errcode; /* 错误代码*/

    uint32_t sndpkg;                /* 发送成功包计数*/
    uint32_t dropedsndpkg;          /* 发送失败的包计数*/
    volatile uint32_t rcvpkg;       /* 接受成功包计数*/
    volatile uint32_t dropedrcvpkg; /* 接收丢失包计数*/
};
#define CAN_ITEM_SIZE sizeof(CanRxMsg)
struct can_rx_fifo
{
    volatile CanRxMsg *buff;
    volatile uint32_t write_index;
    volatile uint32_t read_index;
    volatile uint16_t msg_cnt;
};
static struct _status_can can_state = {0};
static volatile struct can_rx_fifo can_rx_manage = {0};

extern void gpio_set_mode(GPIO_TypeDef *port, uint16_t pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);
/******************************************************************************
*                                 hw can ops
******************************************************************************/
struct stm_baud_rate_tab
{
    uint32_t baud_rate;
    uint32_t confdata;
};
static const struct stm_baud_rate_tab bxcan_baud_rate_tab[] =
{
    // CAN波特率=APB总线频率/BRP分频器/(1+tBS1+tBS2) APB1最大36M
    {1000UL * 1000, MK_BKCAN_BAUD(CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_2tq, 6)},
    {1000UL * 800, MK_BKCAN_BAUD(CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq, 5)},
    {1000UL * 500, MK_BKCAN_BAUD(CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_2tq, 12)},
    {1000UL * 250, MK_BKCAN_BAUD(CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_2tq, 24)},
    {1000UL * 125, MK_BKCAN_BAUD(CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_2tq, 48)},
    {1000UL * 100, MK_BKCAN_BAUD(CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_2tq, 60)},
    {1000UL * 50, MK_BKCAN_BAUD(CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_2tq, 120)},
    {1000UL * 20, MK_BKCAN_BAUD(CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_2tq, 300)},
    {1000UL * 10, MK_BKCAN_BAUD(CAN_SJW_1tq, CAN_BS1_8tq, CAN_BS2_3tq, 300)}
};
#define BAUD_DATA(TYPE, NO) ((bxcan_baud_rate_tab[NO].confdata & TYPE##MASK) >> TYPE##SHIFT)
static uint32_t bxcan_get_baud_index(uint32_t baud)
{
    uint32_t len, index, default_index;

    len = sizeof(bxcan_baud_rate_tab) / sizeof(bxcan_baud_rate_tab[0]);
    default_index = len;

    for (index = 0; index < len; index++)
    {
        if (bxcan_baud_rate_tab[index].baud_rate == baud)
            return index;

        /* 默认CAN 波特率大小为250kbps*/
        if (bxcan_baud_rate_tab[index].baud_rate == 1000UL * 250)
            default_index = index;
    }

    if (default_index != len)
        return default_index;

    return 0;
}
static void set_filter(struct can_config *pconfig)
{
    uint8_t i = 0;
    struct can_filter_item *pitem = NULL;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;

    pitem = pconfig->items;
    for (i = 0; i < pconfig->count; i++)
    {
        CAN_FilterInitStructure.CAN_FilterNumber = pitem->number;

        if (pitem->mode)
        {
            CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
        }
        else
        {
            CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
        }

        if (pitem->scale)
        {
            CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
        }
        else
        {
            CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
        }

        CAN_FilterInitStructure.CAN_FilterIdHigh = pitem->id_high;
        CAN_FilterInitStructure.CAN_FilterIdLow = pitem->id_low;
        CAN_FilterInitStructure.CAN_FilterMaskIdHigh = pitem->mask_id_high;
        CAN_FilterInitStructure.CAN_FilterMaskIdLow = pitem->mask_id_low;
        CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
        CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
        CAN_FilterInit(&CAN_FilterInitStructure);
        pitem++;
    }
}
static void isr_can_updata()
{
    CanRxMsg CANRxMessage;
    CAN_Receive(CAN1, CAN_FIFO0, &CANRxMessage);

    memcpy((uint8_t *)((uint8_t *)&can_rx_manage.buff[0] + can_rx_manage.write_index), &CANRxMessage, CAN_ITEM_SIZE);
    can_rx_manage.write_index += CAN_ITEM_SIZE;
    if (can_rx_manage.write_index >= CAN_BUFF_MAX_INDEX)
        can_rx_manage.write_index = 0;

    can_rx_manage.msg_cnt++;
    can_state.rcvpkg++;
    if (can_rx_manage.msg_cnt > CAN_FIFO_MAX_ITEM)
    {
        can_state.dropedrcvpkg++;
        can_rx_manage.msg_cnt = CAN_FIFO_MAX_ITEM;
    }

    /* 覆盖掉先入队列的数据,确保读指针读最到的数据是队列最底部的数据*/
    if (can_rx_manage.write_index == can_rx_manage.read_index)
    {
        can_rx_manage.read_index += CAN_ITEM_SIZE;
        if (can_rx_manage.read_index >= CAN_BUFF_MAX_INDEX)
            can_rx_manage.read_index = 0;
    }
}
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    __set_FAULTMASK(1);

    /* 当FIFO 0中有3个报文时，硬件对该位置’1’*/
    if (CAN1->RF0R & 0x03)
    {
        /* 当FIFO 0已满，又收到新的报文且报文符合过滤条件，硬件对该位置’1’*/
        if ((CAN1->RF0R & CAN_RF0R_FOVR0) != 0)
        {
            /* 清空溢出标志*/
            CAN1->RF0R &= ~CAN_RF0R_FOVR0;
            can_state.dropedrcvpkg++;
        }
        else
        {
            isr_can_updata();
        }
        /* 释放接收FIFO0输出邮箱*/
        CAN1->RF0R |= CAN_RF0R_RFOM0;
    }
    __set_FAULTMASK(0);
}
/******************************************************************************
*                                 can user ops
******************************************************************************/
/**
 * @desc  : CAN设备读数据
 * @param : *dev 设备地址;  pos: 预留; *buffer 接收数据的指针; size 本次读的条数
 * @return: 0:读取完成; 非0：本次读取剩余多少条数据为未读出
 * @date  : 2018-5-13 20:41:42
 */
int can_read(void *buffer, int item)
{
    int length;         /* 本次要读取的数据长度*/
    uint8_t *read_buff; /* 接收数据的指针*/
    uint8_t *msg_addr;

    if (item == 0)
        return 0;

    length = item;      /* 本次需要读的条数*/
    read_buff = buffer; /* 本次读数据的指针*/
    /* read from software FIFO */
    while (length)
    {
        if (can_rx_manage.read_index != can_rx_manage.write_index)
        {
            msg_addr = (uint8_t *)((uint8_t *)&can_rx_manage.buff[0] + can_rx_manage.read_index);
            can_rx_manage.read_index += CAN_ITEM_SIZE;
            if (can_rx_manage.read_index >= CAN_BUFF_MAX_INDEX)
                can_rx_manage.read_index = 0;

            if (can_rx_manage.msg_cnt != 0)
                can_rx_manage.msg_cnt--;
        }
        else
        {
            break;
        }
        memcpy(read_buff, (uint8_t *)msg_addr, CAN_ITEM_SIZE);
        read_buff += CAN_ITEM_SIZE;
        length--;
    }

    return item - length;
}
/**
 * @desc  : CAN设备的写数据
 * @param : *dev 设备地址;  pos 预留; *buffer 发送数据的指针; size 发送数据的大小(预留)
 * @return: 0 发送失败; 1 发送成功
 * @date  : 2018-5-13 16:14:04
 */
int can_write(CanTxMsg *buffer)
{
    if (buffer == NULL)
        return 0;
    if (CAN_Transmit(CAN1, buffer) != CAN_TxStatus_NoMailBox)
    {
        can_state.sndpkg++;
        return 1;
    }
    else
    {
        can_state.dropedsndpkg++;
        return 0;
    }
}

void can_open(struct can_config *conf)
{
    uint32_t baud_index = 0;
    CAN_InitTypeDef CAN_InitStructure;
    baud_index = bxcan_get_baud_index(conf->baud_rate); /* 获取CAN 波特率配置索引*/

    CAN_DeInit(CAN1);
    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;

    /* 波特率时序寄存器配置*/
    CAN_InitStructure.CAN_SJW = BAUD_DATA(SJW, baud_index); //获取相应时序数据
    CAN_InitStructure.CAN_BS1 = BAUD_DATA(BS1, baud_index);
    CAN_InitStructure.CAN_BS2 = BAUD_DATA(BS2, baud_index);
    CAN_InitStructure.CAN_Prescaler = BAUD_DATA(RRESCL, baud_index);
    CAN_Init(CAN1, &CAN_InitStructure);

    set_filter(conf);

    can_rx_manage.write_index = 0;
    can_rx_manage.read_index = 0;
    can_rx_manage.msg_cnt = 0;
    can_rx_manage.buff = (CanRxMsg *)dev_malloc(CAN_ITEM_SIZE * CAN_FIFO_MAX_ITEM);
    if (can_rx_manage.buff != NULL)
    {
        memset((char *)can_rx_manage.buff, 0, CAN_ITEM_SIZE * CAN_FIFO_MAX_ITEM);
        CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);  /* 邮箱满中断*/
        CAN_ITConfig(CAN1, CAN_IT_FF0, DISABLE);  /* 消息挂号中断*/
        CAN_ITConfig(CAN1, CAN_IT_FOV0, DISABLE); /* 溢出中断*/
    }
    else
    {
       // printf("can open failed! memory len: %dbyte\tmemory add:0x%p\r\n", CAN_ITEM_SIZE * CAN_FIFO_MAX_ITEM, can_rx_manage.buff);
    }
}
void can_close()
{
    /* 关闭can 接收中断*/
    CAN_ITConfig(CAN1, CAN_IT_FMP0, DISABLE);

    /* rx tx 引脚设置未高阻态*/
    gpio_set_mode(CAN1_GPIO_PORT, CAN1_GPIO_RX, GPIO_Mode_AIN, GPIO_Speed_2MHz);
    gpio_set_mode(CAN1_GPIO_PORT, CAN1_GPIO_TX, GPIO_Mode_AIN, GPIO_Speed_2MHz);

    can_rx_manage.write_index = 0;
    can_rx_manage.read_index = 0;
    can_rx_manage.msg_cnt = 0;
    dev_free((void *)can_rx_manage.buff);
    can_rx_manage.buff = NULL;
}
void can_control(int cmd, void *args)
{
    switch (cmd)
    {
    case CAN_CMD_RESET:
        can_open((struct can_config *)args);
        break;
    case CAN_CMD_GET_MSG_NUM:
        *(uint32_t *)args = can_rx_manage.msg_cnt;
        break;
    default:
        break;
    }
}
void can_status()
{
    uint32_t errtype;
    static const char *ErrCode[] =
    {
        "No Error!",
        "Warning !",
        "Passive !",
        "Bus Off !"
    };

    errtype = CAN1->ESR;                /* 获取ESR寄存器的值*/
    can_state.errcode = errtype & 0x07; /* ESR 错误代码*/
    switch (can_state.errcode)
    {
    case 0:
        printf("- ErrCode: %s.\r\n", ErrCode[0]);
        break;
    case 1:
        printf("- ErrCode: %s.\r\n", ErrCode[1]); /* 位填充错误*/
        break;
    case 2:
    case 3:
        printf("- ErrCode: %s.\r\n", ErrCode[2]);
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        printf("- ErrCode: %s.\r\n", ErrCode[3]);
        break;
    }
    printf("- Total receive.packages: %010ld. Droped receive packages: %010ld.\r\n", can_state.rcvpkg, can_state.dropedrcvpkg);
    printf("- Total sending packages: %010ld. Droped sendint packages: %010ld.\r\n", can_state.sndpkg + can_state.dropedsndpkg, can_state.dropedsndpkg);
}
void hw_can_init()
{
    extern void dev_nvic_init(IRQn_Type irq, uint8_t PreemptionPriority, uint8_t SubPriority);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    gpio_set_mode(CAN1_GPIO_PORT, CAN1_GPIO_RX, GPIO_Mode_IPU, GPIO_Speed_50MHz);
    gpio_set_mode(CAN1_GPIO_PORT, CAN1_GPIO_TX, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);

    dev_nvic_init(USB_LP_CAN1_RX0_IRQn, NVIC_CAN_PreemptionPriority, NVIC_SubPriority);
}
