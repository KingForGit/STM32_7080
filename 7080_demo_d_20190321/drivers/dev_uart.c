#include "dev_uart.h"
#include "system_iap.h"

struct _str_uart uart1_recv = {0, {0}};
struct _str_uart uart2_recv = {0, {0}};
struct _str_uart uart4_recv = {0, {0}};
int fputc(int ch, FILE *f)
{
    USART1->SR;
    USART_SendData(USART1, (uint8_t)ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;
    return (ch);
}
void uart_send_data_package(USART_TypeDef *USARTx, uint8_t *data, uint16_t len)
{
    if(USARTx == USART1)
    {
        while (len--)
        {
            USARTx->SR;
            USART_SendData(USARTx, *data++);
            while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
                ;
        }
    }
    else if(USARTx == UART4)
    {
    {
        while (len--)
        {
            UART4->SR;
            USART_SendData(USARTx, *data++);
            while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
                ;
        }
    }
    }
  
}

void hw_uart_init(uint32_t baudrate)
{
    extern void gpio_set_mode(GPIO_TypeDef * port, uint16_t pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);
    USART_InitTypeDef USART_InitStructure;
#if defined(RT_USING_UART1)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    gpio_set_mode(GPIOA, GPIO_Pin_9, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
    gpio_set_mode(GPIOA, GPIO_Pin_10, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);

    USART_DeInit(USART1);
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
#endif    
#if defined(RT_USING_UART4)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); //UART4


    gpio_set_mode(GPIOC, GPIO_Pin_10, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
    gpio_set_mode(GPIOC, GPIO_Pin_11, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);

    USART_DeInit(UART4);
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART4, &USART_InitStructure);
    USART_Cmd(UART4, ENABLE);
#endif    
}

void uart_open(USART_TypeDef *USARTx,struct _str_uart uart_recv)
{
    extern void dev_nvic_init(IRQn_Type irq, uint8_t PreemptionPriority, uint8_t SubPriority);
    if(USARTx==USART1)
    {
        uart1_recv.flag = false;
        uart1_recv.this_cnt = 0;
        uart1_recv.index = 0;
        uart1_recv.data = dev_malloc(sizeof(char) * USART_MAX_LEN);
        if (uart1_recv.data != NULL)
        {
            memset(uart1_recv.data, 0, USART_MAX_LEN);
            dev_nvic_init(USART1_IRQn, NVIC_UART1_PreemptionPriority, NVIC_SubPriority);
            USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
        }
        else
        {
            printf("uart open failed! memory len: %dbyte\tmemory add:0x%p\r\n", USART_MAX_LEN, uart1_recv.data);
        }
    }
    else if(USARTx==UART4)
    {
        uart4_recv.flag = false;
        uart4_recv.this_cnt = 0;
        uart4_recv.index = 0;
        uart4_recv.data = dev_malloc(sizeof(char) * USART4_MAX_LEN);
        if (uart4_recv.data != NULL)
        {
            memset(uart4_recv.data, 0, USART4_MAX_LEN);
            dev_nvic_init(UART4_IRQn, NVIC_UART1_PreemptionPriority, NVIC_SubPriority);
            USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
        }
        else
        {
            printf("uart open failed! memory len: %dbyte\tmemory add:0x%p\r\n", USART4_MAX_LEN, uart1_recv.data);
        }
    }
}
void uart_close(USART_TypeDef *USARTx,struct _str_uart uart_recv)
{
    extern void gpio_set_mode(GPIO_TypeDef * port, uint16_t pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);

    uart1_recv.flag = false;
    uart1_recv.this_cnt = 0;
    uart1_recv.index = 0;

    dev_free((void *)uart1_recv.data);
    uart1_recv.data = NULL;
}

void USART1_IRQHandler(void)
{
    uint8_t clear = clear;
    extern OS_TID tid_task_uart;

    /* 接收数据*/
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);

        if (uart1_recv.flag == true)
        {
            printf("- uart overflow\r\n");
            return;
        }

        uart1_recv.this_cnt++;
        uart1_recv.data[uart1_recv.index] = USART_ReceiveData(USART1);
        if ((uart1_recv.data[uart1_recv.index] == '\n') && (uart1_recv.index > 1))
        {
            if (uart1_recv.data[uart1_recv.index - 1] == '\r')
            {
                uart1_recv.flag = true;
                isr_evt_set(USART_EVENT, tid_task_uart);
            }
        }
        uart1_recv.index = (uart1_recv.index < USART_MAX_LEN) ? (uart1_recv.index += 1) : 0;
    }
}

/******************************************************************************
*                                 串口升级处理
******************************************************************************/

#ifndef UART_IAP_OFF

struct _u_iap
{
    struct iap_info parent;
    timer_t uart_time;
    uint16_t pack_num; /* 请求的包号*/
} uart_iap = {0};

static void uart_iap_timeout(void *parameter)
{
    /* 超过1s 没有数据*/
    if ((get_hw_time_tick() - uart_iap.parent.tick) > 1000)
    {
        NVIC_SystemReset();
    }
    soft_time_start(uart_iap.uart_time);
}
static void get_bin_file(uint16_t id, uint16_t pack_num)
{
    struct
    {
        uint16_t mark;
        uint16_t len;
        uint16_t id;
        uint16_t n_pack;
        uint16_t check;
        uint16_t end_mark;
    } msg = {0x5f5f, 0x0006, 0, 0, 0, 0xaa55};

    msg.id = id;
    msg.n_pack = pack_num;
    msg.check = sum_16bit_verify((uint8_t *)&msg.id, msg.len - 2);

    uart_send_data_package(USART1, (uint8_t *)&msg.mark, 12);
}
void uart_iap_handle(char *data)
{
    static char *bin = NULL;
    char *p_start = NULL;
    char *p_end = NULL;
    uint16_t pack_len = 0; /* 数据长度*/
    uint16_t get_check = 0;
    uint16_t rev_check = 0;
    bool status = true;
    extern void can_close();
    extern OS_TID tid_task_can;
    extern OS_TID tid_task_monitor;

    p_end = strstr(data, "\r\n");
    p_start = strstr(data, "IAP_STM32");

    /* ascii类型数据,长度*2*/
    if (((uint32_t)p_end - (uint32_t)p_start - 9) >= (600 * 2))
    {
        printf("- Data_len error len: %x\r\n", (uint32_t)p_end - (uint32_t)p_start - 9);
        NVIC_SystemReset();
    }

    /* 申请内存*/
    if (bin == NULL)
    {
        can_close();
        bin = dev_malloc(sizeof(char) * 600);
        if (bin == NULL)
            NVIC_SystemReset();
    }

    ascii_to_hex(bin, p_start + 9, (uint32_t)p_end - (uint32_t)p_start - 9);
    if ((bin[0] == 0x5f) && (bin[1] == 0x5f))
    {
        uart_iap.parent.tick = get_hw_time_tick();
        pack_len = *(uint16_t *)(bin + 2);                 /* 取出本包长度*/
        get_check = *(uint16_t *)(bin + 4 + pack_len - 2); /* 取出本包校验值*/
        rev_check = sum_16bit_verify((uint8_t *)(bin + 4), pack_len - 2);
        if (rev_check != get_check)
        {
            delay_ms(10);
            get_bin_file(4, 0); /* 4 校验失败*/
            printf("- single package verify error\r\n");
            NVIC_SystemReset();
        }

        switch (*(uint16_t *)(bin + 4))
        {
        case 1:
            uart_iap.parent.get_total_size = *(uint32_t *)(bin + 6);    /* 升级文件总长度*/
            uart_iap.parent.get_total_verify = *(uint16_t *)(bin + 10); /* 升级总校验和*/
            if (uart_iap.parent.get_total_size < APP_DATA_SIZE)
            {
                status = flash_area_erase(ADDR_APP_BAK, uart_iap.parent.get_total_size); /* 检查bin文件存储目录*/
                if (false != status)
                {
                    get_bin_file(6, 0); /* 没有存储的空间*/
                    printf("- earse falsh error\r\n");
                    NVIC_SystemReset();
                }
                uart_iap.pack_num = 1;
                uart_iap.parent.mode = true;

                DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, DISABLE);
                TIM_ITConfig(TIM8, TIM_IT_Update, DISABLE);

                os_tsk_delete(tid_task_can);
                os_tsk_delete(tid_task_monitor);

                uart_iap.parent.save_wirite_index = ADDR_APP_BAK;
                uart_iap.parent.rev_total_size = 0; /* 实际接收数据总大小*/
                get_bin_file(2, uart_iap.pack_num); /* 请求数据包*/
                uart_iap.uart_time = soft_time_create("_u_timeout", uart_iap_timeout, &uart_iap, 300, SOFT_TIMER_TYPE_ONE_SHOT);
                soft_time_start(uart_iap.uart_time);
            }
            break;
        case 2:
            status = flash_area_write((uint32_t *)uart_iap.parent.save_wirite_index, (uint8_t *)(bin + 8), (pack_len - 6));
            if (true == status)
            {
                get_bin_file(3, 0); /* write error*/
                printf("- write falsh error\r\n");
                NVIC_SystemReset();
            }
            else
            {
                uart_iap.parent.rev_total_size += (pack_len - 6);
                uart_iap.parent.save_wirite_index += (pack_len - 6);
                if (uart_iap.parent.rev_total_size == uart_iap.parent.get_total_size) /* 升级完毕*/
                {
                    uart_iap.parent.rev_total_verify = sum_16bit_verify((uint8_t *)ADDR_APP_BAK, uart_iap.parent.rev_total_size);
                    if (uart_iap.parent.rev_total_verify == uart_iap.parent.get_total_verify) /* 校验正确*/
                    {
                        get_bin_file(5, 0); /* 回应升级成功*/
                        flash_update_mark('Y');
                        delay_ms(10);
                    }
                    NVIC_SystemReset();
                }
                else
                {
                    uart_iap.pack_num++;
                    get_bin_file(2, uart_iap.pack_num); /* 持续请求升级包*/
                }
            }
            break;
        default:
            NVIC_SystemReset();
            break;
        }
    }
    else
    {
        NVIC_SystemReset();
    }
}
#endif
