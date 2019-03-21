#include "board.h"
#include "dev_dac.h"
#include "ops_pwm_output.h"
/******************************************************************************
*                                 other
******************************************************************************/
void assert_failed(uint8_t *file, uint32_t line)
{
    printf("- Wrong parameters value: file %s on line %d\r\n", file, line);
    while (1)
        ;
}
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    printf("- Reset from : Hard Fault...");

    __set_FAULTMASK(1);
    NVIC_SystemReset();
    for (;;)
        ;
}
/******************************************************************************
*                                 get stm32 chip id
******************************************************************************/
uint32_t stm32_mcu_id[3] = {0};
static uint32_t mcu_id_address[] =
{
    0x1FFFF7AC, /* stm32f0唯一id起始地址*/
    0x1FFFF7E8, /* stm32f1唯一id起始地址*/
    0x1FFF7A10, /* stm32f2唯一id起始地址*/
    0x1FFFF7AC, /* stm32f3唯一id起始地址*/
    0x1FFF7A10, /* stm32f4唯一id起始地址*/
    0x1FF0F420, /* stm32f7唯一id起始地址*/
    0x1FF80050, /* stm32l0唯一id起始地址*/
    0x1FF80050, /* stm32l1唯一id起始地址*/
    0x1FFF7590, /* stm32l4唯一id起始地址*/
    0x1FF0F420  /* stm32h7唯一id起始地址*/
};

void get_stm32_mcu_id(uint32_t *id, st_mcu_type type)
{
    if (id != NULL)
    {
        id[0] = *(uint32_t *)(mcu_id_address[type]);
        id[1] = *(uint32_t *)(mcu_id_address[type] + 4);
        id[2] = *(uint32_t *)(mcu_id_address[type] + 8);
    }
    printf("- Mcu uid: %08x%08x%08x\r\n", id[0], id[1], id[2]);
}

/******************************************************************************
*                                 nvic config
******************************************************************************/
/**  @code
     The table below gives the allowed values of the pre-emption priority and subpriority according
     to the Priority Grouping configuration performed by NVIC_PriorityGroupConfig function
      ============================================================================================================================
        NVIC_PriorityGroup   | NVIC_IRQChannelPreemptionPriority | NVIC_IRQChannelSubPriority  | Description
      ============================================================================================================================
       NVIC_PriorityGroup_0  |                0                  |            0-15             |   0 bits for pre-emption priority
                             |                                   |                             |   4 bits for subpriority
      ----------------------------------------------------------------------------------------------------------------------------
       NVIC_PriorityGroup_1  |                0-1                |            0-7              |   1 bits for pre-emption priority
                             |                                   |                             |   3 bits for subpriority
      ----------------------------------------------------------------------------------------------------------------------------
       NVIC_PriorityGroup_2  |                0-3                |            0-3              |   2 bits for pre-emption priority
                             |                                   |                             |   2 bits for subpriority
      ----------------------------------------------------------------------------------------------------------------------------
       NVIC_PriorityGroup_3  |                0-7                |            0-1              |   3 bits for pre-emption priority
                             |                                   |                             |   1 bits for subpriority
      ----------------------------------------------------------------------------------------------------------------------------
       NVIC_PriorityGroup_4  |                0-15               |            0                |   4 bits for pre-emption priority
                             |                                   |                             |   0 bits for subpriority
      ============================================================================================================================
    @endcode
*/

void NVIC_Configuration(void)
{
#ifdef VECT_TAB_RAM
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08003000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);
#endif
}
/**
 * @desc  : NVIC初始化
 * @param : irq 中断向量; PreemptionPriority 抢占优先级[定义见 board.h]; SubPriority 子优先级
 * @return: none
 */
void dev_nvic_init(IRQn_Type irq, uint8_t PreemptionPriority, uint8_t SubPriority)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    SubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannel = irq;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/******************************************************************************
*                                 iwdg ops
******************************************************************************/
int iwdg_init(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); /* 允许写IWDG*/
    IWDG_SetPrescaler(IWDG_Prescaler_256);        /* 设置256分频 中断1次6.4ms*/
    IWDG_SetReload(IWDG_TIME_OUT / 6.4);          /* 设置Reload 最大4095*/
    IWDG_ReloadCounter();                         /* 重载值*/
    IWDG_Enable();                                /* 使能IDWG*/
    return 0;
}
void iwdg_feed(void)
{
    IWDG_ReloadCounter(); //重载值
}
/******************************************************************************
*                                 pvd ops
******************************************************************************/
void hw_pvd_init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); /* Enable PWR and BKP clock */
    EXTI_ClearITPendingBit(EXTI_Line16);

    PWR_PVDLevelConfig(PWR_PVDLevel_2V9); /* 设置阈值电压*/
    PWR_PVDCmd(ENABLE);                   /* 开启pvd*/

    /* 配置中断线*/
    EXTI_InitStructure.EXTI_Line = EXTI_Line16;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* 配置中断向量*/
    NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
void PVD_IRQHandler()
{
    EXTI_ClearITPendingBit(EXTI_Line16);
    if (PWR_GetFlagStatus(PWR_FLAG_PVDO))
    {
        while (1)
        {
            iwdg_feed();
            delay_ms(500);
            if (1 == PWR_GetFlagStatus(PWR_FLAG_PVDO))
                printf("- Pvd int low\r\n");
            else
                break;
        }
    }
}

/******************************************************************************
*                                 show versions
******************************************************************************/
static void show_lib_version()
{
    printf("- Hardware ver : %s\r\n", HARD_VERSION);
    printf("- Softwere ver : V1.%d.%d build %s\r\n", BSP_VERSION, APP_VERSION, __DATE__);
}
/******************************************************************************
*                                 show reset type
******************************************************************************/
void sys_get_reset_type(void)
{
    uint32_t reg = 0;
    reg = RCC->CSR;
    // printf(LOG_ERROR,"RCC->CSR = 0x%x\r\n", reg);

    if ((reg >> 26) & 0x1)
    {
        printf("- NRST Pin Reset...\r\n");
    }
    if ((reg >> 27) & 0x1)
    {
        printf("- Por/Pdr Reset...\r\n");
    }
    if ((reg >> 28) & 0x1)
    {
        printf("- Soft Reset...\r\n");
    }
    if ((reg >> 29) & 0x1)
    {
        printf("- Iwdg Reset...\r\n");
    }
    if ((reg >> 30) & 0x1)
    {
        printf("- Wwdg Reset...\r\n");
    }
    if ((reg >> 31) & 0x1)
    {
        printf("- Low Power Reset...\r\n");
    }

    RCC->CSR |= (uint32_t)(1 << 24); /* 清除复位标志*/
}

void switch_mode_default_init()
{
    float temp = 0;
    struct adc_result adc = {0};
    struct volt_current volt = {0};

    /* adc read voltage*/
    while (adc.status == false)
    {
        adc_read_value(&adc);
        get_volt_current_measure_result(VOLT_CH_BAT, &adc, &volt);
        delay_ms(10);
    }
    temp = (float)((float)(volt.volt_value * 0.7) / 11);
    dac_set_output(DAC_Channel_1, temp * 1000);
    dac_set_output(DAC_Channel_2, temp * 1000);
    //printf("temp= %d  volt_bat %fV\r\n", temp*1000, volt.volt_value);
}

void hw_board_init(void)
{
    extern void hw_pwm_out_init(void);
    extern void hw_gpio_init(void);
    extern void hw_uart_init(uint32_t baudrate);
    extern void hw_adc_init(void);
    extern void hw_dac_init(void);
    extern void hw_time6_init(void);
    extern void hw_can_init(void);
    extern void  hw_time3_init(void);

    delay_ms(100);
    /* NVIC Configuration */
    NVIC_Configuration();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); /* NVIC_PriorityGroup_4 没有子优先级 全部为抢占优先级 */

    /* gpio init*/
    hw_gpio_init();

    /* uart init*/
    hw_uart_init(UART_BAUD);

    /* show version*/
    show_lib_version();

    /* get mcu id*/
    get_stm32_mcu_id(stm32_mcu_id, STM32F1);

    /* show reset type*/
    sys_get_reset_type();

    /* hw timer6 init*/
    hw_time6_init();

     hw_time3_init();
    /* adc init*/
    hw_adc_init();

    /* dac init*/
    hw_dac_init();
    pwm_dac_init();
    /* can init*/
    hw_can_init();

    /* storage init*/
    storage_open();
}
