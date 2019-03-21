/**
 * @author emlsyx
 * @email yangx_1118@163.com
 * @create date 2018-11-22 07:23:27
 * @modify date 2018-11-22 17:35:46
 * @desc This is a file collected by ADC.
*/

#include "dev_adc.h"
//#define DEBUG_ADC
#ifdef DEBUG_ADC
    #define adc_dbg(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define adc_dbg(fmt, ...)
#endif

#define CHANNEL_NUM 7 /* 转换通道个数*/
#define ADC1_DR_Address ((u32)0x40012400 + 0x4c)

static volatile char adc_sampl_cnt = 0; /* 采样次数 最大SAMPLE_CNT次*/

static struct ADC_VALUE adc_value;   /* adc转换结果*/
static struct ADC_SAMPLE adc_sample; /* 采样adc转换结果*/

struct adc_channel adc1_info[CHANNEL_NUM] =
{
    //--gol
    {0, NULL, NULL, ADC_Channel_16, ADC_SampleTime_239Cycles5},         /* temp*/
    {1, GPIOC, GPIO_Pin_0, ADC_Channel_10, ADC_SampleTime_239Cycles5},   /* 电阻采集1*/
    {2, GPIOC, GPIO_Pin_1, ADC_Channel_11, ADC_SampleTime_239Cycles5},   /* 电阻采集2*/
    {3, GPIOC, GPIO_Pin_2, ADC_Channel_12, ADC_SampleTime_239Cycles5},  /*  电压采集1*/
    {4, GPIOC, GPIO_Pin_3, ADC_Channel_13, ADC_SampleTime_239Cycles5},  /* 24V电源电压电压采集2*/
    {5, GPIOA, GPIO_Pin_6, ADC_Channel_6, ADC_SampleTime_239Cycles5},   /* 12V电压采集3*/
    {6, GPIOA, GPIO_Pin_7, ADC_Channel_7, ADC_SampleTime_239Cycles5},   /* 电流采集1*/
    //--gol1
 //   {0, NULL, NULL, ADC_Channel_16, ADC_SampleTime_239Cycles5},         /* temp*/
//    {1, GPIOA, GPIO_Pin_0, ADC_Channel_0, ADC_SampleTime_239Cycles5},   /* 电阻采集1*/
//    {2, GPIOA, GPIO_Pin_1, ADC_Channel_1, ADC_SampleTime_239Cycles5},   /* 电阻采集2*/
//    {3, GPIOA, GPIO_Pin_2, ADC_Channel_2, ADC_SampleTime_239Cycles5},   /* 电阻采集3*/
//    {4, GPIOA, GPIO_Pin_3, ADC_Channel_3, ADC_SampleTime_239Cycles5},   /* 电阻采集4*/
//    {5, GPIOC, GPIO_Pin_5, ADC_Channel_15, ADC_SampleTime_239Cycles5},  /* 电压采集1*/
//    {6, GPIOC, GPIO_Pin_4, ADC_Channel_14, ADC_SampleTime_239Cycles5},  /* 电压采集2*/
//    {7, GPIOA, GPIO_Pin_6, ADC_Channel_6, ADC_SampleTime_239Cycles5},   /* 电压采集3*/
//    {8, GPIOA, GPIO_Pin_7, ADC_Channel_7, ADC_SampleTime_239Cycles5},   /* 电压采集4*/
//    {9, GPIOC, GPIO_Pin_3, ADC_Channel_13, ADC_SampleTime_239Cycles5},  /* 电流采集1*/
//    {10, GPIOC, GPIO_Pin_2, ADC_Channel_12, ADC_SampleTime_239Cycles5}, /* 电流采集2*/
//    {11, GPIOC, GPIO_Pin_1, ADC_Channel_11, ADC_SampleTime_239Cycles5}  /* 电源电压*/
};

/**
 * @desc  : adc1 引脚配置
 * @param : None
 * @return: None
 * @date  : 2018-4-21 13:59:12
 */
static void adc1_gpio_config(void)
{
    extern void gpio_set_mode(GPIO_TypeDef * port, uint16_t pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);
    uint8_t i = 0;

    for (i = 1; i < CHANNEL_NUM; i++)
    {
        gpio_set_mode(adc1_info[i].port, adc1_info[i].pin, GPIO_Mode_AIN, GPIO_Speed_2MHz);
    }
}
/**
 * @desc  : adc1 dma 模式配置
 * @param : None
 * @return: None
 * @date  : 2018-4-21 14:25:08
 */
static void adc1_dma_mode_config(void)
{
    extern void dev_nvic_init(IRQn_Type irq, uint8_t PreemptionPriority, uint8_t SubPriority);
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); /* Enable DMA clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    DMA_DeInit(DMA1_Channel1); /* DMA channel1 configuration */

    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;                 /* ADC地址*/
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&adc_value;                     /* 内存地址*/
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                          /* 外设作为数据传输的来源*/
    DMA_InitStructure.DMA_BufferSize = CHANNEL_NUM;                             /* 每次传输的数据量*/
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            /* 外设地址固定*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     /* 内存地址自增*/
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; /* 外设数据位宽16bit*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         /* 内存数据位宽16bit*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             /* 循环采集*/
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         /* 高优先级*/
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                /* 禁止内存到内存*/
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE); /* 转换完成中断*/
    dev_nvic_init(DMA1_Channel1_IRQn, NVIC_DMA1_PreemptionPriority, NVIC_SubPriority);
    DMA_Cmd(DMA1_Channel1, ENABLE);
}
/**
 * @desc  : adc1 配置
 * @param : None
 * @return: None
 * @date  : 2018-4-21 14:36:02
 */
static void adc1_config()
{
    uint8_t i;
    ADC_InitTypeDef ADC_InitStructure;

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  /* 独立ADC模式*/
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;                        /* 扫描模式，扫描模式用于多通道采集*/
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                  /* 连续转换模式，即不停地进行ADC转换*/
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; /* 外部触发转换*/
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              /* 采集数据右对齐*/
    ADC_InitStructure.ADC_NbrOfChannel = CHANNEL_NUM;                   /* 要转换的通道数目*/
    ADC_Init(ADC1, &ADC_InitStructure);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8); /* 配置ADC时钟不能超过14MHZ，为PCLK2的8分频，即9MHz*/

    /* 配置ADC1的通道16为55.5个采样周期，序列为1*/
    for (i = 0; i < CHANNEL_NUM; i++)
    {
        ADC_RegularChannelConfig(ADC1, adc1_info[i].channel, adc1_info[i].number + 1, adc1_info[i].sample_time);
    }

    ADC_Cmd(ADC1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE); /* Enable ADC1 DMA*/

    ADC_ResetCalibration(ADC1); /* 复位校准寄存器*/
    while (ADC_GetResetCalibrationStatus(ADC1))
        ;                       /* 等待校准寄存器复位完成*/
    ADC_StartCalibration(ADC1); /* ADC校准*/
    while (ADC_GetCalibrationStatus(ADC1))
        ;                                   /* 等待校准完成*/
    ADC_TempSensorVrefintCmd(ENABLE);       /* 开启温度转换*/
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); /* 开启ADC1的软件转换*/
}
/**
 * @desc  : dma 中断处理
 * @param : None
 * @return: None
 * @date  : 2018-4-21 15:01:09
 */
void DMA1_Channel1_IRQHandler(void)
{
    static uint8_t index = 0; /* 采样值存储索引*/
    if (DMA_GetITStatus(DMA1_IT_TC1) == SET)
    {
        DMA_ClearITPendingBit(DMA1_IT_TC1);
        adc_sample.temperature[index] = adc_value.temperature;
        adc_sample.resistance_1[index] = adc_value.resistance_1;
        adc_sample.resistance_2[index] = adc_value.resistance_2;
       // adc_sample.resistance_3[index] = adc_value.resistance_3;
       // adc_sample.resistance_4[index] = adc_value.resistance_4;

        adc_sample.voltage_1[index] = adc_value.voltage_1;
        adc_sample.voltage_2[index] = adc_value.voltage_2;
        adc_sample.voltage_3[index] = adc_value.voltage_3;
      // adc_sample.voltage_4[index] = adc_value.voltage_4;
      // adc_sample.car_bat_v[index] = adc_value.car_bat_v;
			
        adc_sample.current_1[index] = adc_value.current_1;
      //  adc_sample.current_2[index] = adc_value.current_2;

        index = (index < (SAMPLE_CNT - 1)) ? index += 1 : 0;
        adc_sampl_cnt = (adc_sampl_cnt < (SAMPLE_CNT - 1)) ? adc_sampl_cnt += 1 : SAMPLE_CNT - 1;
    }
}
/**
 * @desc  : 校验adc转换值(全0 认为adc转换失败)
 * @param : adc通道数据
 * @return: true: 转换成功; false 转换失败
 * @Date  : 2018-11-26 16:06:36
 */
bool adc_value_verify(struct adc_result *adc_result)
{
    float temp = 0.0;
    temp += adc_result->temperature;
    temp += adc_result->resistance_1;
    temp += adc_result->resistance_2;
   // temp += adc_result->resistance_3;
  //  temp += adc_result->resistance_4;
    temp += adc_result->voltage_1;
    temp += adc_result->voltage_2;
    temp += adc_result->voltage_3;
  //  temp += adc_result->voltage_4;
  //  temp += adc_result->car_bat_v;
    temp += adc_result->current_1;
  //  temp += adc_result->current_2;
    return (temp > 0.0) ? true : false;
}
/**
 * @desc  : 读取ADC的值 (全0 认为转换失败)
 * @param : adc_result 获取结果的存储地址
 * @return: true: 转换成功; false 转换失败
 * @date  : 2018-4-21 15:41:13
 */
void adc_read_value(struct adc_result *adc_result)
{
    struct ADC_SUM adc_sum = {0};
    char temp = adc_sampl_cnt;
    adc_result->status = true;
    memset(adc_result, 0, sizeof(adc_result));
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, DISABLE);
    while (adc_sampl_cnt > 0)
    {
        /* 求和*/
        adc_sum.temperature += adc_sample.temperature[adc_sampl_cnt];
        adc_sum.resistance_1 += adc_sample.resistance_1[adc_sampl_cnt];
        adc_sum.resistance_2 += adc_sample.resistance_2[adc_sampl_cnt];
     //   adc_sum.resistance_3 += adc_sample.resistance_3[adc_sampl_cnt];
     //   adc_sum.resistance_4 += adc_sample.resistance_4[adc_sampl_cnt];
        adc_sum.voltage_1 += adc_sample.voltage_1[adc_sampl_cnt];
        adc_sum.voltage_2 += adc_sample.voltage_2[adc_sampl_cnt];
        adc_sum.voltage_3 += adc_sample.voltage_3[adc_sampl_cnt];
     //   adc_sum.voltage_4 += adc_sample.voltage_4[adc_sampl_cnt];
    //    adc_sum.car_bat_v += adc_sample.car_bat_v[adc_sampl_cnt];
        adc_sum.current_1 += adc_sample.current_1[adc_sampl_cnt];
//   adc_sum.current_2 += adc_sample.current_2[adc_sampl_cnt];
        adc_sampl_cnt--;
    }
    adc_result->temperature = (float)adc_sum.temperature / temp;
    adc_result->resistance_1 = (float)adc_sum.resistance_1 / temp;
    adc_result->resistance_2 = (float)adc_sum.resistance_2 / temp;
//  adc_result->resistance_3 = (float)adc_sum.resistance_3 / temp;
//   adc_result->resistance_4 = (float)adc_sum.resistance_4 / temp;
    adc_result->voltage_1 = (float)adc_sum.voltage_1 / temp;
    adc_result->voltage_2 = (float)adc_sum.voltage_2 / temp;
    adc_result->voltage_3 = (float)adc_sum.voltage_3 / temp;
//   adc_result->voltage_4 = (float)adc_sum.voltage_4 / temp;
//   adc_result->car_bat_v = (float)adc_sum.car_bat_v / temp;

    adc_result->current_1 = (float)adc_sum.current_1 / temp;
//   adc_result->current_2 = (float)adc_sum.current_2 / temp;
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

    adc_result->status = adc_value_verify(adc_result);

//     adc_dbg("\r\n***** adc1 original value *****\r\n");
//     adc_dbg("- temperature :%09.4f\r\n", adc_result->temperature);
//     adc_dbg("- resistance_1:%09.4f\r\n", adc_result->resistance_1);
//     adc_dbg("- resistance_2:%09.4f\r\n", adc_result->resistance_2);
//     adc_dbg("- resistance_3:%09.4f\r\n", adc_result->resistance_3);
//     adc_dbg("- resistance_4:%09.4f\r\n", adc_result->resistance_4);
//     adc_dbg("- voltage_1   :%09.4f\r\n", adc_result->voltage_1);
//     adc_dbg("- voltage_2   :%09.4f\r\n", adc_result->voltage_2);
//     adc_dbg("- voltage_3   :%09.4f\r\n", adc_result->voltage_3);
//     adc_dbg("- voltage_4   :%09.4f\r\n", adc_result->voltage_4);
//     adc_dbg("- car_bat_v   :%09.4f\r\n", adc_result->car_bat_v);
//     adc_dbg("- current_1   :%09.4f\r\n", adc_result->current_1);
//     adc_dbg("- current_2   :%09.4f\r\n", adc_result->current_2);
//     adc_dbg("******************************\r\n");
}
void hw_adc_init(void)
{
    adc1_gpio_config();
    adc1_dma_mode_config();
    adc1_config();
}
