#include "ops_pwm_output.h"
#include "dev_dac.h"
//#define DEBUG_PWM_OUT
#ifdef DEBUG_PWM_OUT
    #define pwm_out_dbg(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define pwm_out_dbg(fmt, ...)
#endif
static struct pwm_out pwm_info = {0};
static struct pwm_out pwm_dac_info = {0};
void get_pwm_conf(struct pwm_out *data)
{
    data->status = pwm_info.status;
    data->duty_cycle = pwm_info.duty_cycle;
    data->period = pwm_info.period;
    data->timer_clock = pwm_info.timer_clock;
    pwm_out_dbg("data->status %d\r\n", data.status);
    pwm_out_dbg("data->duty_cycle %d\r\n", data.duty_cycle);
    pwm_out_dbg("data->period %d\r\n", data.period);
    pwm_out_dbg("data->timer_clock %d\r\n", data.timer_clock);
}
void hw_pwm_out_init()
{
    extern void gpio_set_mode(GPIO_TypeDef * port, uint16_t pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    gpio_set_mode(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* time2 _ch1,ch2,ch3,ch4*/
		GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
    TIM_DeInit(TIM2);
}

void hw_pwm_dac_out_init()
{
    extern void gpio_set_mode(GPIO_TypeDef * port, uint16_t pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);
    gpio_set_mode(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* time2 _ch1,ch2,ch3,ch4*/
    GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
    TIM_DeInit(TIM4);
}
/**
 * @desc  : 配置和打开pwm输出
 * @param : ch 打开的通道号;
 *          out_freq 输出频率单位Hz(0.6hz ~ 1000000);
 *          duty_cycle(0~100%)
 * @return: none
 * @Date  : 2018-12-2 20:33:28
 */
void open_pwm_outup(enum PWM_OUT_CH ch, float out_freq, float duty_cycle)
{
    float temp = 0;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    /* pwm output init*/
	//  if(pwm_info.status != true)
		{
				hw_pwm_out_init();
		}
    /* 0.6hz ~ 1000hz 分频值7200*/
    if ((0.6 <= out_freq) && (out_freq <= 1000.0))
    {
        pwm_info.timer_clock = 10000;           /* 计数频率: 1ms 计数10次 (最慢1ms计数10次)*/
        pwm_info.period = 1000 / out_freq * 10; /* 装载值: 根据频率计算时间，根据1ms计数的次数计算总次数*/
    }
    else if ((out_freq > 1000.0) && (out_freq <= 1000000.0))
    {
        /* 1000hz ~ 1000000hz 分频值72*/
        pwm_info.timer_clock = 1000000;
        pwm_info.period = 1000000 / out_freq;
    }
    else
    {
        pwm_info.timer_clock = 10000;
        pwm_info.period = 10000;
        printf("- pwm out freq overflow. default 1Hz\r\n");
    }

    /* 占空比 = 比较值/计数器重载值*/
    duty_cycle = (duty_cycle > 100) ? 100 : duty_cycle;
    temp = (float)duty_cycle / 100;                           /* 百分比转化为实际数值*/
    pwm_info.duty_cycle = (uint16_t)(temp * pwm_info.period); /* 根据占空比计算输出比较值*/

    TIM_TimeBaseStructure.TIM_Period = pwm_info.period - 1;                           /* 计时器的装载值*/
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / pwm_info.timer_clock - 1; /* 设定预分频值为*/
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;                                      /* 指定时钟分频值*/
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = pwm_info.duty_cycle;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; /* 极性设置*/
   // TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

    if (ch == PWM_OUT_1)
    {
        TIM_OC1Init(TIM2, &TIM_OCInitStructure);
        TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    }
    else if (ch == PWM_OUT_2)
    {
        TIM_OC2Init(TIM2, &TIM_OCInitStructure);
        TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    }
    else if (ch == PWM_OUT_3)
    {
        TIM_OC3Init(TIM2, &TIM_OCInitStructure);
        TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    }
    else if (ch == PWM_OUT_4)
    {
        TIM_OC4Init(TIM2, &TIM_OCInitStructure);
        TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
    }

    TIM_Cmd(TIM2, ENABLE);

    pwm_info.status = true;
}

void close_pwm_output(enum PWM_OUT_CH ch)
{
    if (ch == PWM_OUT_2)
    {
        TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);
    }
    else if(ch == PWM_OUT_1)
    {
        TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
    }
    else if (ch == PWM_OUT_3)
    {
        TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);
    }
    else if (ch == PWM_OUT_4)
    {
        TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Disable);
    }
     TIM_Cmd(TIM2, DISABLE);
        gpio_set_mode(PORT_IO_OUTPUT_1, PIN_IO_OUTPUT_1, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
        gpio_set_mode(PORT_IO_OUTPUT_2, PIN_IO_OUTPUT_2, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
        gpio_set_mode(PORT_IO_OUTPUT_3, PIN_IO_OUTPUT_3, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
        gpio_set_mode(PORT_IO_OUTPUT_4, PIN_IO_OUTPUT_4, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);      
        io1_output(IO_LOW);
        io2_output(IO_LOW);
        io3_output(IO_LOW);
        io4_output(IO_LOW);
}

/**
 * @desc  : 设置pwm输出占空比
 * @param : duty_cycle 占空比 (1~100%)
 * @return: 设置成功 0; 设置失败 -1; 返回-1时，需要设置频率
 * @Date  : 2018-12-3 11:23:30
 */
int set_pwm_duty_cycle(enum PWM_OUT_CH ch, float duty_cycle)
{
    float temp = 0;
    /* 占空比 = 比较值/计数器重载值*/
    if (pwm_info.status == false)
        return -1;
    duty_cycle = (duty_cycle > 100) ? 100 : duty_cycle;
    temp = (float)duty_cycle / 100;                           /* 百分比转化为实际数值*/
    pwm_info.duty_cycle = (uint16_t)(temp * pwm_info.period); /* 根据占空比计算输出比较值*/

    if (ch == PWM_OUT_2)
    {
        TIM_SetCompare2(TIM2, pwm_info.duty_cycle);
    }
    else if (ch == PWM_OUT_1)
    {
        TIM_SetCompare1(TIM2, pwm_info.duty_cycle);
    }
    else if (ch == PWM_OUT_3)
    {
        TIM_SetCompare3(TIM2, pwm_info.duty_cycle);
    }
    else if (ch == PWM_OUT_4)
    {
        TIM_SetCompare4(TIM2, pwm_info.duty_cycle);
    }
    return 0;
}

/**
 * @desc  : 配置和打开pwm——DAC输出
 * @param : ch 打开的通道号;
 *          out_freq 输出频率单位Hz(0.6hz ~ 1000000);
 *          duty_cycle(0~100%)
 * @return: none
 * @Date  : 2018-12-2 20:33:28
 */
void open_pwm_dac_outup(enum PWM_DAC_CH ch, float out_freq, float duty_cycle)
{
    float temp = 0;

    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    /* pwm output init*/
	  if(pwm_dac_info.status != true)
		{
				hw_pwm_dac_out_init();
		}
    /* 0.6hz ~ 1000hz 分频值7200*/
    if ((0.6 <= out_freq) && (out_freq <= 1000.0))
    {
        pwm_dac_info.timer_clock = 10000;           /* 计数频率: 1ms 计数10次 (最慢1ms计数10次)*/
        pwm_dac_info.period = 1000 / out_freq * 10; /* 装载值: 根据频率计算时间，根据1ms计数的次数计算总次数*/
    }
    else if ((out_freq > 1000.0) && (out_freq <= 1000000.0))
    {
        /* 1000hz ~ 1000000hz 分频值72*/
        pwm_dac_info.timer_clock = 1000000;
        pwm_dac_info.period = 1000000 / out_freq;
    }
    else
    {
        pwm_dac_info.timer_clock = 10000;
        pwm_dac_info.period = 10000;
        printf("- pwm out freq overflow. default 1Hz\r\n");
    }

    /* 占空比 = 比较值/计数器重载值*/
    duty_cycle = (duty_cycle > 100) ? 100 : duty_cycle;
    temp = (float)duty_cycle / 100;                           /* 百分比转化为实际数值*/
    pwm_dac_info.duty_cycle = (uint16_t)(temp * pwm_dac_info.period); /* 根据占空比计算输出比较值*/

    TIM_TimeBaseStructure.TIM_Period = pwm_dac_info.period - 1;                           /* 计时器的装载值*/
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / pwm_dac_info.timer_clock - 1; /* 设定预分频值为*/
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;                                      /* 指定时钟分频值*/
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = pwm_dac_info.duty_cycle;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; /* 极性设置*/

    if (ch == PWM_DAC_1)
    {
        TIM_OC1Init(TIM4, &TIM_OCInitStructure);
        TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    }
    else if (ch == PWM_DAC_2)
    {
        TIM_OC2Init(TIM4, &TIM_OCInitStructure);
        TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
			
    }
    else if (ch == PWM_DAC_3)
    {
        TIM_OC3Init(TIM4, &TIM_OCInitStructure);
        TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
    }
    else if (ch == PWM_DAC_4)
    {
        TIM_OC4Init(TIM4, &TIM_OCInitStructure);
        TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
    }
    TIM_Cmd(TIM4, ENABLE);
    pwm_dac_info.status = true;
}
/* @desc  : 关闭pwm_dac输出*/
void close_pwm_dac_output(enum PWM_OUT_CH ch)
{
    if (ch == PWM_OUT_2)
    {
        TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Disable);
    }
    else if(ch == PWM_OUT_1)
    {
        TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Disable);
    }
    else if (ch == PWM_OUT_3)
    {
        TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Disable);
    }
    else if (ch == PWM_OUT_4)
    {
        TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Disable);
    }
}

/**
 * @desc  : 设置pwm_dac输出占空比
 * @param : duty_cycle 占空比 (1~100%)
 * @return: 设置成功 0; 设置失败 -1; 返回-1时，需要设置频率
 * @Date  : 2018-12-3 11:23:30
 */
int set_pwm_dac_duty_cycle(enum PWM_OUT_CH ch, float duty_cycle)
{
    float temp = 0;
    /* 占空比 = 比较值/计数器重载值*/
    if (pwm_dac_info.status == false)
        return -1;

    duty_cycle = (duty_cycle > 100) ? 100 : duty_cycle;
    temp = (float)duty_cycle / 100;                           /* 百分比转化为实际数值*/
    pwm_dac_info.duty_cycle = (uint16_t)(temp * pwm_dac_info.period); /* 根据占空比计算输出比较值*/

    if (ch == PWM_OUT_2)
    {
        TIM_SetCompare2(TIM4, pwm_dac_info.duty_cycle);
    }
    else if (ch == PWM_OUT_1)
    {
        TIM_SetCompare1(TIM4, pwm_dac_info.duty_cycle);
    }
    else if (ch == PWM_OUT_3)
    {
        TIM_SetCompare3(TIM4, pwm_dac_info.duty_cycle);
    }
    else if (ch == PWM_OUT_4)
    {
        TIM_SetCompare4(TIM4, pwm_dac_info.duty_cycle);
    }
    return 0;
}
/*******************************************************************************
*函数名称: void in_DI_ThresHold(uint8 group_u8, uint16 value_16)
*功    能: 设置开关量阈值，共6组阈值
			DAC1:DH_1~4						DAC3:DL_1~4
			DAC2:DH_5~9						DAC4:DL_5~6
			DAC5:DH_PI_1~2					    DAC6:DH_PI_3~4
*输    入: chnl_32设置通道号，value_u16输出值 mv值
*返    回: 无
*******************************************************************************/
void in_DI_ThresHold(uint8_t group_u8, uint16_t Set_mvVal_u16)
{
	uint16_t value_u16;
	value_u16 = 1000 - (Set_mvVal_u16 / 3.3);
	switch(group_u8)
	{	//DAC1
		case IN_GROUP_1_ThresHold:
				TIM_SetCompare1(TIM4, (uint16_t)(value_u16 * pwm_dac_info.period / 1000));
			break;
		//DAC2
		case IN_GROUP_2_ThresHold:
				TIM_SetCompare2(TIM4, (uint16_t)(value_u16 * pwm_dac_info.period  / 1000));
			break;
		//DAC3
		case IN_GROUP_3_ThresHold:
				TIM_SetCompare3(TIM4, (uint16_t)(value_u16 * pwm_dac_info.period  / 1000));
			break;
		//DAC4
		case IN_GROUP_4_ThresHold:
				TIM_SetCompare4(TIM4, (uint16_t)(value_u16 * pwm_dac_info.period / 1000));
			break;
		//DAC5
		case IN_GROUP_5_ThresHold:
			//	dac_run(DAC1, Set_mvVal_u16);
                dac_set_output(DAC_Channel_1, Set_mvVal_u16);
//				TIM_SetCompare3(TIM3, value_u16);
			break;
		//DAC6		
		case IN_GROUP_6_ThresHold:
			//	dac_run(DAC2, Set_mvVal_u16);
                 dac_set_output(DAC_Channel_2, Set_mvVal_u16);
//				TIM_SetCompare4(TIM3, value_u16);
			break;
	}
}
void pwm_dac_init(void)
{
	//0.98V的阈值
	open_pwm_dac_outup(PWM_DAC_1,2000,30);
	open_pwm_dac_outup(PWM_DAC_2,2000,30);	
	open_pwm_dac_outup(PWM_DAC_3,2000,20);
	open_pwm_dac_outup(PWM_DAC_4,2000,20);		
}


