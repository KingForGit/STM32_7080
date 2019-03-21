#ifndef __DEV_GPIO_H__
#define __DEV_GPIO_H__

#include "board.h"

//--gol
/* A20主板电源复位*/
#define PORT_PWRON GPIOE
#define PIN_PWRON GPIO_Pin_2
#define pwron_off() GPIO_ResetBits(PORT_PWRON, PIN_PWRON)
#define pwron_on() GPIO_SetBits(PORT_PWRON, PIN_PWRON)
#define pwron_flip() GPIO_ReadOutputDataBit(PORT_PWRON, PIN_PWRON) < 1 ? pwron_on() : pwron_off()

/* A20主板电源检测*/
#define PORT_RESET GPIOE
#define PIN_RESET GPIO_Pin_3
#define pwr_gpio_state() GPIO_ReadInputDataBit(PORT_RESET, PIN_RESET)

/* BEEP_EN1*/
#define PORT_BEEP_EN1 GPIOE
#define PIN_BEEP_EN1 GPIO_Pin_4
#define beep1_off() GPIO_ResetBits(PORT_BEEP_EN1, PIN_BEEP_EN1)
#define beep1_on() GPIO_SetBits(PORT_BEEP_EN1, PIN_BEEP_EN1)

/* BEEP_EN2*/
#define PORT_BEEP_EN2 GPIOE
#define PIN_BEEP_EN2 GPIO_Pin_5
#define beep2_off() GPIO_ResetBits(PORT_BEEP_EN2, PIN_BEEP_EN2)
#define beep2_on() GPIO_SetBits(PORT_BEEP_EN2, PIN_BEEP_EN2)

#define beep_flip()     GPIO_ReadOutputDataBit(PORT_BEEP_EN1, PIN_BEEP_EN1) <1 ?beep1_on(),beep2_off()\
                                                                               :beep1_off(),beep2_on()   



/* LED*/
#define PORT_LED GPIOE
#define PIN_LED GPIO_Pin_6
#define led_off() GPIO_ResetBits(PORT_LED, PIN_LED)
#define led_on() GPIO_SetBits(PORT_LED, PIN_LED)
#define led_flip() GPIO_ReadOutputDataBit(PORT_LED, PIN_LED) < 1 ? led_on() : led_off()

/* 电阻测量1配置选择*/
#define PORT_RES_CH1_L GPIOC
#define PIN_RES_CH1_L GPIO_Pin_4
#define PORT_RES_CH1_H GPIOC
#define PIN_RES_CH1_H GPIO_Pin_5
#define open_ch1_low_res_mesure()                      \
    {                                                  \
        GPIO_SetBits(PORT_RES_CH1_L, PIN_RES_CH1_L);   \
        GPIO_ResetBits(PORT_RES_CH1_H, PIN_RES_CH1_H); \
    }
#define open_ch1_high_res_mesure()                     \
    {                                                  \
        GPIO_ResetBits(PORT_RES_CH1_L, PIN_RES_CH1_L); \
        GPIO_SetBits(PORT_RES_CH1_H, PIN_RES_CH1_H);   \
    }
/* 电阻测量2配置选择*/
#define PORT_RES_CH2_L GPIOB
#define PIN_RES_CH2_L GPIO_Pin_0
#define PORT_RES_CH2_H GPIOB
#define PIN_RES_CH2_H GPIO_Pin_1
#define open_ch2_low_res_mesure()                      \
    {                                                  \
        GPIO_SetBits(PORT_RES_CH2_L, PIN_RES_CH2_L);   \
        GPIO_ResetBits(PORT_RES_CH2_H, PIN_RES_CH2_H); \
    }
#define open_ch2_high_res_mesure()                     \
    {                                                  \
        GPIO_ResetBits(PORT_RES_CH2_L, PIN_RES_CH2_L); \
        GPIO_SetBits(PORT_RES_CH2_H, PIN_RES_CH2_H);   \
    }

/*外部检查配置选择*/
#define PORT_EXT_RX GPIOE
#define PIN_EXT_RX GPIO_Pin_12

#define PORT_EXT_VX GPIOE
#define PIN_EXT_VX GPIO_Pin_13

#define PORT_EXT_IX GPIOE
#define PIN_EXT_IX GPIO_Pin_14

#define exte_no_mesure()                            \
    {                                                  \
        GPIO_ResetBits(PORT_EXT_RX, PIN_EXT_RX);       \
        GPIO_ResetBits(PORT_EXT_VX, PIN_EXT_VX);       \
        GPIO_ResetBits(PORT_EXT_IX, PIN_EXT_IX);         \
    }
#define exte_rx_en_mesure()                            \
    {                                                  \
        GPIO_SetBits(PORT_EXT_RX, PIN_EXT_RX);         \
        GPIO_ResetBits(PORT_EXT_VX, PIN_EXT_VX);       \
        GPIO_ResetBits(PORT_EXT_IX, PIN_EXT_IX);       \
    }
#define exte_vx_en_mesure()                            \
    {                                                  \
        GPIO_ResetBits(PORT_EXT_RX, PIN_EXT_RX);       \
        GPIO_SetBits(PORT_EXT_VX, PIN_EXT_VX);         \
        GPIO_ResetBits(PORT_EXT_IX, PIN_EXT_IX);       \
    }
#define exte_ix_en_mesure()                            \
    {                                                  \
        GPIO_ResetBits(PORT_EXT_RX, PIN_EXT_RX);       \
        GPIO_ResetBits(PORT_EXT_VX, PIN_EXT_VX);       \
        GPIO_SetBits(PORT_EXT_IX, PIN_EXT_IX);         \
    }
/* X路开关量采集io配置*/
//9路高有效采集io
#define PORT_SW_VALUE_1 GPIOD
#define PIN_SW_VALUE_1 GPIO_Pin_11
#define sw1_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_1, PIN_SW_VALUE_1)

#define PORT_SW_VALUE_2 GPIOD
#define PIN_SW_VALUE_2 GPIO_Pin_10
#define sw2_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_2, PIN_SW_VALUE_2)

#define PORT_SW_VALUE_3 GPIOD
#define PIN_SW_VALUE_3 GPIO_Pin_9
#define sw3_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_3, PIN_SW_VALUE_3)

#define PORT_SW_VALUE_4 GPIOD
#define PIN_SW_VALUE_4 GPIO_Pin_8
#define sw4_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_4, PIN_SW_VALUE_4)

#define PORT_SW_VALUE_5 GPIOB
#define PIN_SW_VALUE_5 GPIO_Pin_15
#define sw5_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_5, PIN_SW_VALUE_5)

#define PORT_SW_VALUE_6 GPIOB
#define PIN_SW_VALUE_6 GPIO_Pin_14
#define sw6_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_6, PIN_SW_VALUE_6)

#define PORT_SW_VALUE_7 GPIOB
#define PIN_SW_VALUE_7 GPIO_Pin_13
#define sw7_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_7, PIN_SW_VALUE_7)

#define PORT_SW_VALUE_8 GPIOB
#define PIN_SW_VALUE_8 GPIO_Pin_12
#define sw8_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_8, PIN_SW_VALUE_8)

#define PORT_SW_VALUE_9 GPIOA
#define PIN_SW_VALUE_9 GPIO_Pin_8
#define sw9_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_9, PIN_SW_VALUE_9)

//4路下拉（传感器？）采集io
#define PORT_SW_VALUE_10 GPIOC
#define PIN_SW_VALUE_10  GPIO_Pin_6
#define sw10_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_10, PIN_SW_VALUE_10)

#define PORT_SW_VALUE_11 GPIOC
#define PIN_SW_VALUE_11 GPIO_Pin_7
#define sw11_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_11, PIN_SW_VALUE_11)

#define PORT_SW_VALUE_12 GPIOC
#define PIN_SW_VALUE_12 GPIO_Pin_8
#define sw12_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_12, PIN_SW_VALUE_12)

#define PORT_SW_VALUE_13 GPIOC
#define PIN_SW_VALUE_13 GPIO_Pin_9
#define sw13_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_13, PIN_SW_VALUE_13)

//6路可配采集 io
#define PORT_SW_VALUE_14 GPIOC
#define PIN_SW_VALUE_14 GPIO_Pin_12
#define sw14_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_14, PIN_SW_VALUE_14)

#define PORT_SW_VALUE_15 GPIOD
#define PIN_SW_VALUE_15 GPIO_Pin_0
#define sw15_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_15, PIN_SW_VALUE_15)

#define PORT_SW_VALUE_16 GPIOD
#define PIN_SW_VALUE_16 GPIO_Pin_1
#define sw16_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_16, PIN_SW_VALUE_16)

#define PORT_SW_VALUE_17 GPIOD
#define PIN_SW_VALUE_17 GPIO_Pin_2
#define sw17_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_17, PIN_SW_VALUE_17)

#define PORT_SW_VALUE_18 GPIOD
#define PIN_SW_VALUE_18 GPIO_Pin_3
#define sw18_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_18, PIN_SW_VALUE_18)

#define PORT_SW_VALUE_19 GPIOD
#define PIN_SW_VALUE_19 GPIO_Pin_4
#define sw19_gpio_state() GPIO_ReadInputDataBit(PORT_SW_VALUE_19, PIN_SW_VALUE_19)





/* 开关量采集模式控制io配置 (6个上拉)*/
#define PORT_SW_CTRL_1 GPIOB
#define PIN_SW_CTRL_1 GPIO_Pin_4
#define sw1_ctrl_up(mode) ((mode == PULL_UP) ? GPIO_SetBits(PORT_SW_CTRL_1, PIN_SW_CTRL_1) : GPIO_ResetBits(PORT_SW_CTRL_1, PIN_SW_CTRL_1))

#define PORT_SW_CTRL_2 GPIOB
#define PIN_SW_CTRL_2 GPIO_Pin_5
#define sw2_ctrl_up(mode) ((mode == PULL_UP) ? GPIO_SetBits(PORT_SW_CTRL_2, PIN_SW_CTRL_2) : GPIO_ResetBits(PORT_SW_CTRL_2, PIN_SW_CTRL_2))

#define PORT_SW_CTRL_3 GPIOB
#define PIN_SW_CTRL_3 GPIO_Pin_6
#define sw3_ctrl_up(mode) ((mode == PULL_UP) ? GPIO_SetBits(PORT_SW_CTRL_3, PIN_SW_CTRL_3) : GPIO_ResetBits(PORT_SW_CTRL_3, PIN_SW_CTRL_3))

#define PORT_SW_CTRL_4 GPIOB
#define PIN_SW_CTRL_4 GPIO_Pin_7
#define sw4_ctrl_up(mode) ((mode == PULL_UP) ? GPIO_SetBits(PORT_SW_CTRL_4, PIN_SW_CTRL_4) : GPIO_ResetBits(PORT_SW_CTRL_4, PIN_SW_CTRL_4))

#define PORT_SW_CTRL_5 GPIOB
#define PIN_SW_CTRL_5 GPIO_Pin_8
#define sw5_ctrl_up(mode) ((mode == PULL_UP) ? GPIO_SetBits(PORT_SW_CTRL_5, PIN_SW_CTRL_5) : GPIO_ResetBits(PORT_SW_CTRL_5, PIN_SW_CTRL_5))

#define PORT_SW_CTRL_6 GPIOB
#define PIN_SW_CTRL_6 GPIO_Pin_9
#define sw6_ctrl_up(mode) ((mode == PULL_UP) ? GPIO_SetBits(PORT_SW_CTRL_6, PIN_SW_CTRL_6) : GPIO_ResetBits(PORT_SW_CTRL_6, PIN_SW_CTRL_6))

/* 6个开关量采集模式设置浮空*/
#define sw1_ctrl_levitate() GPIO_ResetBits(PORT_SW_CTRL_1, PIN_SW_CTRL_1)
#define sw2_ctrl_levitate() GPIO_ResetBits(PORT_SW_CTRL_2, PIN_SW_CTRL_2)
#define sw3_ctrl_levitate() GPIO_ResetBits(PORT_SW_CTRL_3, PIN_SW_CTRL_3)
#define sw4_ctrl_levitate() GPIO_ResetBits(PORT_SW_CTRL_4, PIN_SW_CTRL_4)
#define sw5_ctrl_levitate() GPIO_ResetBits(PORT_SW_CTRL_5, PIN_SW_CTRL_5)
#define sw6_ctrl_levitate() GPIO_ResetBits(PORT_SW_CTRL_6, PIN_SW_CTRL_6)

// #define sw7_ctrl_levitate() GPIO_ResetBits(PORT_SW_CTRL_6, PIN_SW_CTRL_6)
// #define sw8_ctrl_levitate() GPIO_ResetBits(PORT_SW_CTRL_6, PIN_SW_CTRL_6)
// #define sw9_ctrl_levitate() GPIO_ResetBits(PORT_SW_CTRL_6, PIN_SW_CTRL_6)
// #define sw10_ctrl_levitate() GPIO_ResetBits(PORT_SW_CTRL_6, PIN_SW_CTRL_6)

/* 4个开关量采集下拉  频率*/
#define PORT_SW_CTRL_DOWN_1 GPIOE
#define PIN_SW_CTRL_DOWN_1 GPIO_Pin_1
#define sw1_ctrl_down(mode) ((mode == PULL_DOWN) ? GPIO_SetBits(PORT_SW_CTRL_DOWN_1, PIN_SW_CTRL_DOWN_1) : GPIO_ResetBits(PORT_SW_CTRL_DOWN_1, PIN_SW_CTRL_DOWN_1))

#define PORT_SW_CTRL_DOWN_2 GPIOE
#define PIN_SW_CTRL_DOWN_2 GPIO_Pin_15
#define sw2_ctrl_down(mode) ((mode == PULL_DOWN) ? GPIO_SetBits(PORT_SW_CTRL_DOWN_2, PIN_SW_CTRL_DOWN_2) : GPIO_ResetBits(PORT_SW_CTRL_DOWN_2, PIN_SW_CTRL_DOWN_2))

#define PORT_SW_CTRL_DOWN_3 GPIOB
#define PIN_SW_CTRL_DOWN_3 GPIO_Pin_10
#define sw3_ctrl_down(mode) ((mode == PULL_DOWN) ? GPIO_SetBits(PORT_SW_CTRL_DOWN_3, PIN_SW_CTRL_DOWN_3) : GPIO_ResetBits(PORT_SW_CTRL_DOWN_3, PIN_SW_CTRL_DOWN_3))

#define PORT_SW_CTRL_DOWN_4 GPIOB
#define PIN_SW_CTRL_DOWN_4 GPIO_Pin_11
#define sw4_ctrl_down(mode) ((mode == PULL_DOWN) ? GPIO_SetBits(PORT_SW_CTRL_DOWN_4, PIN_SW_CTRL_DOWN_4) : GPIO_ResetBits(PORT_SW_CTRL_DOWN_4, PIN_SW_CTRL_DOWN_4))
/* 4个开关量采集下拉浮空*/

/* x路普通 GPIO输出*/
//DO/PWM_EN1
#define PORT_IO_OUTPUT_1 GPIOA
#define PIN_IO_OUTPUT_1 GPIO_Pin_0
#define io1_output(mode) ((mode == IO_HIGH) ? GPIO_SetBits(PORT_IO_OUTPUT_1, PIN_IO_OUTPUT_1) : GPIO_ResetBits(PORT_IO_OUTPUT_1, PIN_IO_OUTPUT_1))

#define PORT_IO_OUTPUT_2 GPIOA
#define PIN_IO_OUTPUT_2 GPIO_Pin_1
#define io2_output(mode) ((mode == IO_HIGH) ? GPIO_SetBits(PORT_IO_OUTPUT_2, PIN_IO_OUTPUT_2) : GPIO_ResetBits(PORT_IO_OUTPUT_2, PIN_IO_OUTPUT_2))

#define PORT_IO_OUTPUT_3 GPIOA
#define PIN_IO_OUTPUT_3 GPIO_Pin_2
#define io3_output(mode) ((mode == IO_HIGH) ? GPIO_SetBits(PORT_IO_OUTPUT_3, PIN_IO_OUTPUT_3) : GPIO_ResetBits(PORT_IO_OUTPUT_3, PIN_IO_OUTPUT_3))

#define PORT_IO_OUTPUT_4 GPIOA
#define PIN_IO_OUTPUT_4 GPIO_Pin_3
#define io4_output(mode) ((mode == IO_HIGH) ? GPIO_SetBits(PORT_IO_OUTPUT_4, PIN_IO_OUTPUT_4) : GPIO_ResetBits(PORT_IO_OUTPUT_4, PIN_IO_OUTPUT_4))

//STKP_OUT0
#define PORT_IO_OUTPUT_5 GPIOE
#define PIN_IO_OUTPUT_5 GPIO_Pin_8
#define io5_output(mode) ((mode == IO_HIGH) ? GPIO_SetBits(PORT_IO_OUTPUT_5, PIN_IO_OUTPUT_5) : GPIO_ResetBits(PORT_IO_OUTPUT_5, PIN_IO_OUTPUT_5))

#define PORT_IO_OUTPUT_6 GPIOE
#define PIN_IO_OUTPUT_6 GPIO_Pin_9
#define io6_output(mode) ((mode == IO_HIGH) ? GPIO_SetBits(PORT_IO_OUTPUT_6, PIN_IO_OUTPUT_6) : GPIO_ResetBits(PORT_IO_OUTPUT_6, PIN_IO_OUTPUT_6))

#define PORT_IO_OUTPUT_7 GPIOE
#define PIN_IO_OUTPUT_7 GPIO_Pin_10
#define io7_output(mode) ((mode == IO_HIGH) ? GPIO_SetBits(PORT_IO_OUTPUT_7, PIN_IO_OUTPUT_7) : GPIO_ResetBits(PORT_IO_OUTPUT_7, PIN_IO_OUTPUT_7))

#define PORT_IO_OUTPUT_8 GPIOE
#define PIN_IO_OUTPUT_8 GPIO_Pin_11
#define io8_output(mode) ((mode == IO_HIGH) ? GPIO_SetBits(PORT_IO_OUTPUT_8, PIN_IO_OUTPUT_8) : GPIO_ResetBits(PORT_IO_OUTPUT_8, PIN_IO_OUTPUT_8))
//
//

//PWM_DAC
#define PORT_IO_OUTPUT_9 GPIOD
#define PIN_IO_OUTPUT_9 GPIO_Pin_12
#define io9_output(mode) ((mode == IO_HIGH) ? GPIO_SetBits(PORT_IO_OUTPUT_9, PIN_IO_OUTPUT_9) : GPIO_ResetBits(PORT_IO_OUTPUT_9, PIN_IO_OUTPUT_9))
#define pmw_dac1_state() GPIO_ReadInputDataBit(PORT_IO_OUTPUT_9, PIN_IO_OUTPUT_9)

#define PORT_IO_OUTPUT_10 GPIOD
#define PIN_IO_OUTPUT_10 GPIO_Pin_13
#define io10_output(mode) ((mode == IO_HIGH) ? GPIO_SetBits(PORT_IO_OUTPUT_10, PIN_IO_OUTPUT_10) : GPIO_ResetBits(PORT_IO_OUTPUT_10, PIN_IO_OUTPUT_10))
#define pmw_dac2_state() GPIO_ReadInputDataBit(PORT_IO_OUTPUT_10, PIN_IO_OUTPUT_10)

#define PORT_IO_OUTPUT_11 GPIOD
#define PIN_IO_OUTPUT_11 GPIO_Pin_14
#define io11_output(mode) ((mode == IO_HIGH) ? GPIO_SetBits(PORT_IO_OUTPUT_11, PIN_IO_OUTPUT_11) : GPIO_ResetBits(PORT_IO_OUTPUT_11, PIN_IO_OUTPUT_11))
#define pmw_dac3_state() GPIO_ReadInputDataBit(PORT_IO_OUTPUT_11, PIN_IO_OUTPUT_11)

#define PORT_IO_OUTPUT_12 GPIOD
#define PIN_IO_OUTPUT_12 GPIO_Pin_15
#define io12_output(mode) ((mode == IO_HIGH) ? GPIO_SetBits(PORT_IO_OUTPUT_12, PIN_IO_OUTPUT_12) : GPIO_ResetBits(PORT_IO_OUTPUT_12, PIN_IO_OUTPUT_12))
#define pmw_dac4_state() GPIO_ReadInputDataBit(PORT_IO_OUTPUT_12, PIN_IO_OUTPUT_12)

/*  end GPIO输出*/
//STKP_IUT
#define PORT_STKP_IUT_0 GPIOD
#define PIN_STKP_IUT_0 GPIO_Pin_5
#define stkp0_gpio_state() GPIO_ReadInputDataBit(PORT_STKP_IUT_0, PIN_STKP_IUT_0)

#define PORT_STKP_IUT_1 GPIOD
#define PIN_STKP_IUT_1 GPIO_Pin_6
#define stkp1_gpio_state() GPIO_ReadInputDataBit(PORT_STKP_IUT_1, PIN_STKP_IUT_1)

#define PORT_STKP_IUT_2 GPIOD
#define PIN_STKP_IUT_2 GPIO_Pin_7
#define stkp2_gpio_state() GPIO_ReadInputDataBit(PORT_STKP_IUT_2, PIN_STKP_IUT_2)

#define PORT_STKP_IUT_3 GPIOB
#define PIN_STKP_IUT_3 GPIO_Pin_3
#define stkp3_gpio_state() GPIO_ReadInputDataBit(PORT_STKP_IUT_3, PIN_STKP_IUT_3)


//--goli------------------------------------------------

#endif /* end of include guard: __DEV_GPIO_H__ */
