#include "dev_gpio.h"

void gpio_set_mode(GPIO_TypeDef *port, uint16_t pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = pin;
    GPIO_InitStructure.GPIO_Mode = mode;
    GPIO_InitStructure.GPIO_Speed = speed;
    GPIO_Init(port, &GPIO_InitStructure);
}
/**
 * @desc  : gpio外部中断初始化
 * @param : line 中断线号; mode 中断线模式 中断/事件; trigger 触发条件 上升沿/下降沿等
 * @return: none
 */
void gpio_exti_init(uint32_t line, EXTIMode_TypeDef mode, EXTITrigger_TypeDef trigger, FunctionalState flag)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    EXTI_InitStructure.EXTI_Line = line;
    EXTI_InitStructure.EXTI_Mode = mode;
    EXTI_InitStructure.EXTI_Trigger = trigger;
    EXTI_InitStructure.EXTI_LineCmd = flag;
    EXTI_Init(&EXTI_InitStructure);
}

void hw_gpio_init()
{
    /*Start GPIO.(A-F) Prot*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                               RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO,
                           ENABLE);

//--gol
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
    gpio_set_mode(PORT_PWRON, PIN_PWRON, GPIO_Mode_Out_PP, GPIO_Speed_2MHz);
    pwron_on();

    gpio_set_mode(PORT_RESET, PIN_RESET, GPIO_Mode_IPU, GPIO_Speed_2MHz);

    gpio_set_mode(PORT_BEEP_EN1, PIN_BEEP_EN1, GPIO_Mode_Out_PP, GPIO_Speed_2MHz);
    beep1_off();
    gpio_set_mode(PORT_BEEP_EN2, PIN_BEEP_EN2, GPIO_Mode_Out_PP, GPIO_Speed_2MHz);
    beep2_off();

    gpio_set_mode(PORT_LED, PIN_LED, GPIO_Mode_Out_PP, GPIO_Speed_2MHz);
    led_off();


    gpio_set_mode(PORT_RES_CH1_L, PIN_RES_CH1_L, GPIO_Mode_Out_PP, GPIO_Speed_10MHz);
    gpio_set_mode(PORT_RES_CH1_H, PIN_RES_CH1_H, GPIO_Mode_Out_PP, GPIO_Speed_10MHz);
    open_ch1_high_res_mesure(); /* 默认使用高匹配电阻测量*/

    gpio_set_mode(PORT_RES_CH2_L, PIN_RES_CH2_L, GPIO_Mode_Out_PP, GPIO_Speed_10MHz);
    gpio_set_mode(PORT_RES_CH2_H, PIN_RES_CH2_H, GPIO_Mode_Out_PP, GPIO_Speed_10MHz);
    open_ch2_high_res_mesure(); /* 默认使用高匹配电阻测量*/
    /*外部检测配置选择，默认电阻*/
    gpio_set_mode(PORT_EXT_RX, PIN_EXT_RX, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_EXT_VX, PIN_EXT_VX, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_EXT_IX, PIN_EXT_IX, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    exte_no_mesure();
    /* X路开关量采集gpio初始化*/
    gpio_set_mode(PORT_SW_VALUE_1, PIN_SW_VALUE_1, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_2, PIN_SW_VALUE_2, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_3, PIN_SW_VALUE_3, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_4, PIN_SW_VALUE_4, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_5, PIN_SW_VALUE_5, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_6, PIN_SW_VALUE_6, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_7, PIN_SW_VALUE_7, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_8, PIN_SW_VALUE_8, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_9, PIN_SW_VALUE_9, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_10, PIN_SW_VALUE_10, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_11, PIN_SW_VALUE_11, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_12, PIN_SW_VALUE_12, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_13, PIN_SW_VALUE_13, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_14, PIN_SW_VALUE_14, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_15, PIN_SW_VALUE_15, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_16, PIN_SW_VALUE_16, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_17, PIN_SW_VALUE_17, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_18, PIN_SW_VALUE_18, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_VALUE_19, PIN_SW_VALUE_19, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);

    /* 6路开关量控制模式gpio初始化 上拉*/
    gpio_set_mode(PORT_SW_CTRL_1, PIN_SW_CTRL_1, GPIO_Mode_Out_PP, GPIO_Speed_2MHz);
    gpio_set_mode(PORT_SW_CTRL_2, PIN_SW_CTRL_2, GPIO_Mode_Out_PP, GPIO_Speed_2MHz);
    gpio_set_mode(PORT_SW_CTRL_3, PIN_SW_CTRL_3, GPIO_Mode_Out_PP, GPIO_Speed_2MHz);
    gpio_set_mode(PORT_SW_CTRL_4, PIN_SW_CTRL_4, GPIO_Mode_Out_PP, GPIO_Speed_2MHz);
    gpio_set_mode(PORT_SW_CTRL_5, PIN_SW_CTRL_5, GPIO_Mode_Out_PP, GPIO_Speed_2MHz);
    gpio_set_mode(PORT_SW_CTRL_6, PIN_SW_CTRL_6, GPIO_Mode_Out_PP, GPIO_Speed_2MHz);
    sw1_ctrl_up(PULL_DOWN); //
    sw6_ctrl_up(PULL_DOWN);
    /* 4路开关量控制模式gpio初始化 下拉*/
    gpio_set_mode(PORT_SW_CTRL_DOWN_1, PIN_SW_CTRL_DOWN_1, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_CTRL_DOWN_2, PIN_SW_CTRL_DOWN_2, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_CTRL_DOWN_3, PIN_SW_CTRL_DOWN_3, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_SW_CTRL_DOWN_4, PIN_SW_CTRL_DOWN_4, GPIO_Mode_Out_PP, GPIO_Speed_50MHz); 
        /* X路普通IO输出初始化 PWM*/
    gpio_set_mode(PORT_IO_OUTPUT_1, PIN_IO_OUTPUT_1, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_IO_OUTPUT_2, PIN_IO_OUTPUT_2, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_IO_OUTPUT_3, PIN_IO_OUTPUT_3, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_IO_OUTPUT_4, PIN_IO_OUTPUT_4, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
//    初始化完成后输出0；   要考虑到前两个引脚和DL1\DL2J接插件上是复用的
    io1_output(IO_LOW);
    io2_output(IO_LOW);
    io3_output(IO_LOW);
    io4_output(IO_LOW);
    gpio_set_mode(PORT_IO_OUTPUT_5, PIN_IO_OUTPUT_5, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_IO_OUTPUT_6, PIN_IO_OUTPUT_6, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_IO_OUTPUT_7, PIN_IO_OUTPUT_7, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_IO_OUTPUT_8, PIN_IO_OUTPUT_8, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
   
    gpio_set_mode(PORT_IO_OUTPUT_9, PIN_IO_OUTPUT_9, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_IO_OUTPUT_10, PIN_IO_OUTPUT_10, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_IO_OUTPUT_11, PIN_IO_OUTPUT_11, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_IO_OUTPUT_12, PIN_IO_OUTPUT_12, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);  
    /*  end GPIO输出*/
    //STKP_IUT
    gpio_set_mode(PORT_STKP_IUT_0, PIN_STKP_IUT_0, GPIO_Mode_IPU, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_STKP_IUT_1, PIN_STKP_IUT_1, GPIO_Mode_IPU, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_STKP_IUT_2, PIN_STKP_IUT_2, GPIO_Mode_IPU, GPIO_Speed_50MHz);
    gpio_set_mode(PORT_STKP_IUT_3, PIN_STKP_IUT_3, GPIO_Mode_IPU, GPIO_Speed_50MHz);
//--goli--------------------
    /* 4路开关量下拉控制*/
    //gpio_set_mode(PORT_SW_CTRL_DOWN_1, PIN_SW_CTRL_DOWN_1, GPIO_Mode_Out_PP, GPIO_Speed_2MHz);
    //gpio_set_mode(PORT_SW_CTRL_DOWN_2, PIN_SW_CTRL_DOWN_2, GPIO_Mode_Out_PP, GPIO_Speed_2MHz);
   // gpio_set_mode(PORT_SW_CTRL_DOWN_3, PIN_SW_CTRL_DOWN_3, GPIO_Mode_Out_PP, GPIO_Speed_2MHz);
   // gpio_set_mode(PORT_SW_CTRL_DOWN_4, PIN_SW_CTRL_DOWN_4, GPIO_Mode_Out_PP, GPIO_Speed_2MHz);
    /* 空闲gpio 设置成高阻态*/
    gpio_set_mode(GPIOA, GPIO_Pin_15, GPIO_Mode_AIN, GPIO_Speed_2MHz);
  //  gpio_set_mode(GPIOB, GPIO_Pin_3 | GPIO_Pin_4, GPIO_Mode_AIN, GPIO_Speed_2MHz);
    gpio_set_mode(GPIOC, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, GPIO_Mode_AIN, GPIO_Speed_2MHz);
    gpio_set_mode(GPIOE, GPIO_Pin_7| GPIO_Pin_0, GPIO_Mode_AIN, GPIO_Speed_2MHz);
}
