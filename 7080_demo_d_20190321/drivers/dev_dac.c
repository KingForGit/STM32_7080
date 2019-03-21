#include "dev_dac.h"

#define DAC_RESOLUTION (float)4096 / 3.3
extern void gpio_set_mode(GPIO_TypeDef *port, uint16_t pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);
void dac1_hw_init(void)
{
    DAC_InitTypeDef DAC_InitType;

    gpio_set_mode(GPIOA, GPIO_Pin_4, GPIO_Mode_AIN, GPIO_Speed_50MHz);
    GPIO_SetBits(GPIOA, GPIO_Pin_4); /* PA.4 输出高*/

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);                  /* 使能DAC通道时钟*/
    DAC_InitType.DAC_Trigger = DAC_Trigger_None;                         /* 不使用触发功能 TEN1=0*/
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;           /* 不使用波形发生*/
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0; /* 屏蔽、幅值设置*/
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable;            /* DAC1输出缓存关闭 BOFF1=1*/
    DAC_Init(DAC_Channel_1, &DAC_InitType);                              /* 初始化DAC通道1*/
    DAC_Cmd(DAC_Channel_1, ENABLE);                                      /* 使能DAC1*/
    DAC_SetChannel1Data(DAC_Align_12b_R, 0);                             /* 12位右对齐数据格式设置DAC值*/
}

void dac2_hw_init(void)
{
    DAC_InitTypeDef DAC_InitType;

    gpio_set_mode(GPIOA, GPIO_Pin_5, GPIO_Mode_AIN, GPIO_Speed_50MHz);
    GPIO_SetBits(GPIOA, GPIO_Pin_5); /* PA.5 输出高*/

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);                  /* 使能DAC通道时钟*/
    DAC_InitType.DAC_Trigger = DAC_Trigger_None;                         /* 不使用触发功能 TEN1=0*/
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;           /* 不使用波形发生*/
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0; /* 屏蔽、幅值设置*/
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable;            /* DAC2输出缓存关闭 BOFF1=1*/
    DAC_Init(DAC_Channel_2, &DAC_InitType);                              /* 初始化DAC通道1*/
    DAC_Cmd(DAC_Channel_2, ENABLE);                                      /* 使能DAC2*/
    DAC_SetChannel1Data(DAC_Align_12b_R, 0);                             /* 12位右对齐数据格式设置DAC值*/
}

void dac_set_output(uint32_t DAC_Channel, uint16_t value)
{
    float temp = 0;
    value = (value > 3299) ? 3299 : value;

    temp = (float)value / 1000 * DAC_RESOLUTION;

    if (DAC_Channel == DAC_Channel_1)
    {
        DAC_SetChannel1Data(DAC_Align_12b_R, temp); /* 12位右对齐数据格式设置DAC值*/
    }
    else if (DAC_Channel == DAC_Channel_2)
    {
        DAC_SetChannel2Data(DAC_Align_12b_R, temp); /* 12位右对齐数据格式设置DAC值*/
    }
    else
    {
        /* code */
    }
}

uint16_t dac_get_value(uint32_t DAC_Channel)
{
    return DAC_GetDataOutputValue(DAC_Channel);
}

void hw_dac_init()
{
    dac1_hw_init();
    dac2_hw_init();
}
