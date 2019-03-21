#include "ops_freq_input.h"
#include "dev_dac.h"
#define DEBUG_FREQ_COLLECT
#ifdef DEBUG_FREQ_COLLECT
    #define ferq_dbg(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define ferq_dbg(fmt, ...)
#endif
extern void gpio_exti_init(uint32_t line, EXTIMode_TypeDef mode, EXTITrigger_TypeDef trigger, FunctionalState flag);

/* 输入捕获模式采集,脉宽不可小于15us*/
/* 外部中断模式采集,脉宽不可小于1ms*/
#define FREQ_TIME_CLOCK 1000000          /* 高频计数频率*/
#define FREQ_EXTI_CLOCK TIME6_CLOCK / 10 /* 低频计数频率*/
#define EDGE_FIRST 0                     /* 第一边沿标志*/
#define EDGE_SECOND 1                    /* 第二边沿标志*/

enum FREQ_MODE
{
    EXTI_MODE = 0, /* 低频模式,定时读取gpio状态*/
    TIMER_MODE = 1 /* 高频模式,定时器捕获*/
};

struct freq_s
{
    volatile const uint32_t ch_num; /* 通道号标记*/
    volatile uint32_t mark;         /* 边沿标记: EDGE_FIRST: 第一个捕获值; EDGE_SECOND: 第二个捕获值*/
    volatile uint32_t first;        /* 第一个边沿 crr值*/
    volatile uint32_t second;       /* 第二个边沿 crr值*/
    volatile uint32_t tick_cnt;     /* 当前tick*/
    volatile uint32_t pulse_total;  /* 脉冲总个数*/
    volatile uint32_t pulse_sec;    /* 秒脉冲个数*/
    volatile uint32_t pulse_width;  /* 脉冲宽度 (上升沿到下降沿差)*/
    volatile uint32_t diff_value;   /* 两个相同类型边沿差值*/
    volatile uint32_t time_flag;    /* 时间标志*/
    volatile uint32_t pulse_flag;   /* 脉冲标志*/
};

static bool time_init_status = false; /* 定时器初始化状态: false 未初始化定时器; true 已初始化 */
static volatile struct freq_s t_freq[4] =
{
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0},
}; /* 定时器捕获方式获取的脉冲信息*/
static volatile struct freq_s e_freq[4] =
{
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0},
}; /* 外部中断方式获取的脉冲信息*/

static void freq_status_default_init(volatile const uint8_t ch)
{
    if (ch >= 4)
        return;

    t_freq[ch].mark = EDGE_FIRST;
    t_freq[ch].first = 0;
    t_freq[ch].second = 0;
    t_freq[ch].tick_cnt = 0;
    t_freq[ch].pulse_total = 0;
    t_freq[ch].pulse_sec = 0;
    t_freq[ch].pulse_width = 0;
    t_freq[ch].diff_value = 0;
    t_freq[ch].time_flag = get_hw_time_tick();
    t_freq[ch].pulse_flag = 0;

    e_freq[ch].mark = EDGE_FIRST;
    e_freq[ch].first = 0;
    e_freq[ch].second = 0;
    e_freq[ch].tick_cnt = 0;
    e_freq[ch].pulse_total = 0;
    e_freq[ch].pulse_sec = 0;
    e_freq[ch].pulse_width = 0;
    e_freq[ch].diff_value = 0;
    e_freq[ch].time_flag = t_freq[ch].time_flag;
    e_freq[ch].pulse_flag = 0;
}
static void freq_collect_timer_init()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    extern void dev_nvic_init(IRQn_Type irq, uint8_t PreemptionPriority, uint8_t SubPriority);
    extern void gpio_set_mode(GPIO_TypeDef * port, uint16_t pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);

    if (time_init_status == false)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
//这里要考虑只用一个频率，另外用开关量的时候，下面的四行代码不合适了就
        gpio_set_mode(PORT_SW_VALUE_10, PIN_SW_VALUE_10, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
        gpio_set_mode(PORT_SW_VALUE_11, PIN_SW_VALUE_11, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
        gpio_set_mode(PORT_SW_VALUE_12, PIN_SW_VALUE_12, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
        gpio_set_mode(PORT_SW_VALUE_13, PIN_SW_VALUE_13, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);

        /* 低频采集*/
        GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource6);
        GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource7);
        GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource8);
        GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9);
        dev_nvic_init(EXTI9_5_IRQn, NVIC_EXTI_9_5_PreemptionPriority, NVIC_SubPriority);

        /* 高频采集*/
        TIM_DeInit(TIM8);
        TIM_TimeBaseStructure.TIM_Period = 0xffff;
        TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / FREQ_TIME_CLOCK - 1;
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
        TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
        TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

        dev_nvic_init(TIM8_CC_IRQn, NVIC_TIM8_TIM8_CC_PreemptionPriority, NVIC_SubPriority);
        TIM_ClearFlag(TIM8, TIM_FLAG_Update);
        TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE); /* 开溢出中断*/
    }
    time_init_status = true;
}
/**
 * @desc  : 中断处理函数(根据计算tick信息,更新freq结构信息)
 * @param : data freq结构信息;
 *          tick: timer 对应CCR值;
 *          type 翻转类型(定时器翻转或者外部中断翻转)
 * @return: none
 * @Date  : 2018-12-4 16:52:43
 */
static volatile void isr_updata_freq_s(volatile struct freq_s *const data, volatile uint32_t tick, enum FREQ_MODE volatile type)
{
    volatile uint16_t tmpccer = 0;
    volatile uint32_t time = 0;

    data->tick_cnt = tick;
    /* 上升沿触发*/
    if (data->mark == EDGE_FIRST)
    {
        /* 上升沿触发,first边沿值大于0,表示是第三个沿*/
        if ((data->first > 0) && (data->second > 0))
        {
            if (TIMER_MODE == type)
            {
                /* 获取脉宽*/
                data->pulse_width = (data->second > data->first) ? (data->second - data->first) : ((0xffff - data->first) + data->second);

                /* 获取频率*/
                data->diff_value = (data->tick_cnt > data->first) ? (data->tick_cnt - data->first) : ((0xffff - data->first) + data->tick_cnt);
            }
            else if (EXTI_MODE == type)
            {
                data->pulse_width = (data->second > data->first) ? (data->second - data->first) : ((0xffffffff - data->first) + data->second);
                data->diff_value = (data->tick_cnt > data->first) ? (data->tick_cnt - data->first) : ((0xffffffff - data->first) + data->tick_cnt);
            }

            /* 统计脉冲总个数*/
            data->pulse_total++;
            data->pulse_total = ((data->pulse_total) >= 0xffffffff) ? 0 : data->pulse_total;

            /* 统计每秒脉冲个数*/
            time = get_hw_time_tick();
            if ((time - data->time_flag) >= (1000 / TIME6_MILLISECOND))
            {
                data->pulse_sec = data->pulse_total - data->pulse_flag;
                data->time_flag = time;
                data->pulse_flag = data->pulse_total;
            }
            data->second = 0;
        }

        data->first = data->tick_cnt;
        data->mark = EDGE_SECOND;
        switch (data->ch_num)
        {
        case 0:
            if (TIMER_MODE == type)
            {
                tmpccer = TIM8->CCER; /* 获取当前time8 ccer寄存器值*/
                tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC1P));
                tmpccer |= TIM_ICPolarity_Falling; /* 修改为下降沿触发*/
                TIM8->CCER = tmpccer;
            }
            else
            {
                EXTI->RTSR &= ~EXTI_Line6; /* 关闭上升沿触发*/
                EXTI->FTSR |= EXTI_Line6;  /* 打开下降沿触发*/
            }
            break;
        case 1:
            if (TIMER_MODE == type)
            {
                tmpccer = TIM8->CCER;
                tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC2P));
                tmpccer |= (uint16_t)(TIM_ICPolarity_Falling << 4);
                TIM8->CCER = tmpccer;
            }
            else
            {
                EXTI->RTSR &= ~EXTI_Line7;
                EXTI->FTSR |= EXTI_Line7;
            }
            break;
        case 2:
            if (TIMER_MODE == type)
            {
                tmpccer = TIM8->CCER;
                tmpccer &= (uint16_t) ~((uint16_t)TIM_CCER_CC3P);
                tmpccer |= (uint16_t)(TIM_ICPolarity_Falling << 8);
                TIM8->CCER = tmpccer;
            }
            else
            {
                EXTI->RTSR &= ~EXTI_Line8;
                EXTI->FTSR |= EXTI_Line8;
            }
            break;
        case 3:
            if (TIMER_MODE == type)
            {
                tmpccer = TIM8->CCER;
                tmpccer &= (uint16_t) ~((uint16_t)TIM_CCER_CC4P);
                tmpccer |= (uint16_t)(TIM_ICPolarity_Falling << 12);
                TIM8->CCER = tmpccer;
            }
            else
            {
                EXTI->RTSR &= ~EXTI_Line9;
                EXTI->FTSR |= EXTI_Line9;
            }
            break;
        }
    }
    else if (data->mark == EDGE_SECOND)
    {
        data->second = data->tick_cnt;
        data->mark = EDGE_FIRST;
        switch (data->ch_num)
        {
        case 0:
            if (TIMER_MODE == type)
            {
                tmpccer = TIM8->CCER; /* 获取当前time8 ccer寄存器值*/
                tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC1P));
                tmpccer |= TIM_ICPolarity_Rising; /* 修改上升沿触发*/
                TIM8->CCER = tmpccer;
            }
            else
            {
                EXTI->FTSR &= ~EXTI_Line6; /* 关闭下降沿触发*/
                EXTI->RTSR |= EXTI_Line6;  /* 打开上升沿触发*/
            }
            break;
        case 1:
            if (TIMER_MODE == type)
            {
                tmpccer = TIM8->CCER;
                tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC2P));
                tmpccer |= (uint16_t)(TIM_ICPolarity_Rising << 4);
                TIM8->CCER = tmpccer;
            }
            else
            {
                EXTI->FTSR &= ~EXTI_Line7;
                EXTI->RTSR |= EXTI_Line7;
            }
            break;
        case 2:
            if (TIMER_MODE == type)
            {
                tmpccer = TIM8->CCER;
                tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC3P));
                tmpccer |= (uint16_t)(TIM_ICPolarity_Rising << 8);
                TIM8->CCER = tmpccer;
            }
            else
            {
                EXTI->FTSR &= ~EXTI_Line8;
                EXTI->RTSR |= EXTI_Line8;
            }
            break;
        case 3:
            if (TIMER_MODE == type)
            {
                tmpccer = TIM8->CCER;
                tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC4P));
                tmpccer |= (uint16_t)(TIM_ICPolarity_Rising << 12);
                TIM8->CCER = tmpccer;
            }
            else
            {
                EXTI->FTSR &= ~EXTI_Line9;
                EXTI->RTSR |= EXTI_Line9;
            }
            break;
        }
    }
    else
    {
        /* code*/
    }
}
void EXTI9_5_IRQHandler(void)
{
    volatile uint32_t tick = 0;
    if (((EXTI->PR & EXTI_Line6) != (uint32_t)RESET) &&
            ((EXTI->IMR & EXTI_Line6) != (uint32_t)RESET))
    {
        tick = get_hw_time_tick();
        isr_updata_freq_s(&e_freq[0], tick, EXTI_MODE);
        EXTI->PR = EXTI_Line6; /* 清除中断信号*/
    }
    else if (((EXTI->PR & EXTI_Line7) != (uint32_t)RESET) &&
             ((EXTI->IMR & EXTI_Line7) != (uint32_t)RESET))
    {
        tick = get_hw_time_tick();
        isr_updata_freq_s(&e_freq[1], tick, EXTI_MODE);
        EXTI->PR = EXTI_Line7;
    }
    else if (((EXTI->PR & EXTI_Line8) != (uint32_t)RESET) &&
             ((EXTI->IMR & EXTI_Line8) != (uint32_t)RESET))
    {
        tick = get_hw_time_tick();
        isr_updata_freq_s(&e_freq[2], tick, EXTI_MODE);
        EXTI->PR = EXTI_Line8;
    }
    else if (((EXTI->PR & EXTI_Line9) != (uint32_t)RESET) &&
             ((EXTI->IMR & EXTI_Line9) != (uint32_t)RESET))
    {
        tick = get_hw_time_tick();
        isr_updata_freq_s(&e_freq[3], tick, EXTI_MODE);
        EXTI->PR = EXTI_Line9;
    }
}
void TIM8_CC_IRQHandler(void)
{
    if (((TIM8->SR & TIM_IT_CC1) != (uint16_t)RESET) &&
            ((TIM8->DIER & TIM_IT_CC1) != (uint16_t)RESET))
    {
        isr_updata_freq_s(&t_freq[0], (uint32_t)TIM8->CCR1, TIMER_MODE);
        TIM8->SR = (uint16_t)~TIM_IT_CC1;
    }
    else if (((TIM8->SR & TIM_IT_CC2) != (uint16_t)RESET) &&
             ((TIM8->DIER & TIM_IT_CC2) != (uint16_t)RESET))
    {
        isr_updata_freq_s(&t_freq[1], (uint32_t)TIM8->CCR2, TIMER_MODE);
        TIM8->SR = (uint16_t)~TIM_IT_CC2;
    }
    else if (((TIM8->SR & TIM_IT_CC3) != (uint16_t)RESET) &&
             ((TIM8->DIER & TIM_IT_CC3) != (uint16_t)RESET))
    {
        isr_updata_freq_s(&t_freq[2], (uint32_t)TIM8->CCR3, TIMER_MODE);
        TIM8->SR = (uint16_t)~TIM_IT_CC3;
    }
    else if (((TIM8->SR & TIM_IT_CC4) != (uint16_t)RESET) &&
             ((TIM8->DIER & TIM_IT_CC4) != (uint16_t)RESET))
    {
        isr_updata_freq_s(&t_freq[3], (uint32_t)TIM8->CCR4, TIMER_MODE);
        TIM8->SR = (uint16_t)~TIM_IT_CC4;
    }
    else
    {
        /* code*/
    }
}

/**
 * @desc  : 打开通道频率采集
 * @param : ch 打开的通道号; mode 上下拉模式(禁止使用上拉); dac_v 比较电压(单位mv)
 * @return: none
 * @Date  : 2018-12-3 16:34:39
 */
void open_freq_collect(enum FREQ_CH ch, enum sw_mode mode, uint16_t dac_v)
{
    TIM_ICInitTypeDef TIM_ICInitStructure;

    freq_collect_timer_init();
    dac_set_output(DAC_Channel_1, dac_v);
    dac_set_output(DAC_Channel_2, dac_v);
    switch (ch)
    {
    case FREQ_CH_1:
        freq_status_default_init(FREQ_CH_1);
        if (PULL_DOWN == mode)
        {
            sw1_ctrl_down(mode);
        }
        TIM_ICInitStructure.TIM_Channel = TIM_Channel_1; /* 选择TIM8的通道1*/
        TIM_ITConfig(TIM8, TIM_IT_CC1, ENABLE);          /* 使能TIM的TIM_IT_CC1中断*/
        gpio_exti_init(EXTI_Line6, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling, ENABLE);
        break;
    case FREQ_CH_2:
        freq_status_default_init(FREQ_CH_2);
        if (PULL_DOWN == mode)
        {
            sw2_ctrl_down(mode);
        }
        TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
        TIM_ITConfig(TIM8, TIM_IT_CC2, ENABLE);
        gpio_exti_init(EXTI_Line7, EXTI_Mode_Interrupt, EXTI_Trigger_Rising, ENABLE);
        break;
    case FREQ_CH_3:
        freq_status_default_init(FREQ_CH_3);
        if (PULL_DOWN == mode)
        {
            sw3_ctrl_down(mode);
        }
        TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
        TIM_ITConfig(TIM8, TIM_IT_CC3, ENABLE);
        gpio_exti_init(EXTI_Line8, EXTI_Mode_Interrupt, EXTI_Trigger_Rising, ENABLE);
        break;
    case FREQ_CH_4:
        freq_status_default_init(FREQ_CH_4);
        if (PULL_DOWN == mode)
        {
            sw4_ctrl_down(mode);
        }
        TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
        TIM_ITConfig(TIM8, TIM_IT_CC4, ENABLE);
        gpio_exti_init(EXTI_Line9, EXTI_Mode_Interrupt, EXTI_Trigger_Rising, ENABLE);
        break;
    case FREQ_CH_ALL:
        open_freq_collect(FREQ_CH_1, mode, dac_v);
        open_freq_collect(FREQ_CH_2, mode, dac_v);
        open_freq_collect(FREQ_CH_3, mode, dac_v);
        open_freq_collect(FREQ_CH_4, mode, dac_v);
        return;
    default:
        break;
    }
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;     /* 上升沿捕获*/
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; /* 映射到Ti1上*/
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;           /* 捕获分频系数为0*/

    /* 配置输入滤波器虑*/
    /* http://note.youdao.com/noteshare?id=754d946c2b9dce95c05054660ca087f2&sub=A4E241F233D5432896C2FC7B04944414*/
    TIM_ICInitStructure.TIM_ICFilter = 0x0f;
    TIM_ICInit(TIM8, &TIM_ICInitStructure);

    TIM_Cmd(TIM8, ENABLE);
}
void close_freq_collect(enum FREQ_CH ch)
{
    time_init_status = false;
    switch (ch)
    {
    case FREQ_CH_1:
        freq_status_default_init(FREQ_CH_1);
        TIM_ITConfig(TIM8, TIM_IT_CC1, DISABLE);
        gpio_exti_init(EXTI_Line6, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling, DISABLE);
        break;
    case FREQ_CH_2:
        freq_status_default_init(FREQ_CH_2);
        TIM_ITConfig(TIM8, TIM_IT_CC2, DISABLE);
        gpio_exti_init(EXTI_Line7, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling, DISABLE);
        break;
    case FREQ_CH_3:
        freq_status_default_init(FREQ_CH_3);
        TIM_ITConfig(TIM8, TIM_IT_CC3, DISABLE);
        gpio_exti_init(EXTI_Line8, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling, DISABLE);
        break;
    case FREQ_CH_4:
        freq_status_default_init(FREQ_CH_4);
        TIM_ITConfig(TIM8, TIM_IT_CC4, DISABLE);
        gpio_exti_init(EXTI_Line9, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling, DISABLE);
        break;
    case FREQ_CH_ALL:
        close_freq_collect(FREQ_CH_1);
        close_freq_collect(FREQ_CH_2);
        close_freq_collect(FREQ_CH_3);
        close_freq_collect(FREQ_CH_4);
        return;
    default:
        break;
    }
}
/**
 * @desc  : 计算频率
 * @param :
 * @return: false 采集失败; true 采集成功
 * @Date  : 2019-1-19 16:21:00
 */
bool freq_count(volatile struct freq_status *result, volatile struct freq_s *source, enum FREQ_MODE type)
{
    bool status = false;
    result->mode = type;

    if (result->mode == TIMER_MODE)
    {
        /* 脉宽小于15us(定时器捕获滤波值) 采集失败*/
        if ((source->pulse_width < 15) || (source->pulse_width <= 0))
        {
            result->duty_cycle = 0; /* 正脉冲占空比 单位%*/
            result->pulse_width = 0;
            goto ferq_error;
        }
        result->pulse_width = source->pulse_width;                 /* 1us时基 转换us*/
        result->freq_value =(float) (FREQ_TIME_CLOCK) /(float) (source->diff_value ) ; /* 频率值*/
     //     result->freq_value = FREQ_TIME_CLOCK / 17; /* 频率值*/
    }
    else
    {
        /* 脉宽值大于1s 采集失败*/
        if ((source->pulse_width > 1000) || (source->pulse_width <= 0))
        {
            result->duty_cycle = 100; /* 正脉冲占空比 单位%*/
            result->pulse_width = 1000000;
            goto ferq_error;
        }
        result->pulse_width = source->pulse_width * 1000;          /* 1ms时基 转换us*/
        result->freq_value = (float)FREQ_EXTI_CLOCK / (float)(source->diff_value)  ; /* 频率值*/
    }
    status = true;
    result->pulse_second = source->pulse_sec; /* 每秒的脉冲数*/
    result->pulse_total = source->pulse_total;
    result->duty_cycle = (float)source->pulse_width / source->diff_value * 100; /* 正脉冲占空比*/
    result->this_time = get_hw_time_tick() / (1000 / TIME6_MILLISECOND);        /* 秒计数*/

    /* updata time*/
    source->pulse_width = 0;
    source->diff_value = 0;
    source->tick_cnt = 0;
    return status;

ferq_error:
    result->freq_value = 0;   /* 频率值*/
    result->pulse_total = 0;  /* 脉冲总个数*/
    result->pulse_second = 0; /* 秒脉冲个数*/
    return status;
}
static void printf_freq_collect_status(volatile struct freq_status *data)
{
    ferq_dbg("-  NO.%02d   ", data->ch_num + 1);
    if (data->status == false)
        ferq_dbg("Error     ");
    else
        ferq_dbg("  Ok      ");

    if (data->mode == TIMER_MODE)
        ferq_dbg("Timer   ");
    else
        ferq_dbg("Exti    ");
//data->freq_value=8.9;
    ferq_dbg("%08d  ", data->pulse_total);
    ferq_dbg("%07d    ", data->pulse_second);
    //ferq_dbg("%06d    ", data->freq_value);
    ferq_dbg("%010.4f\r\n", data->freq_value);
    ferq_dbg("%07d   ", data->pulse_width);
    ferq_dbg("%02d%%    ", data->duty_cycle);
    ferq_dbg("%05d\r\n", data->this_time);
}
#ifdef DEBUG_FREQ_COLLECT
static void printf_freq_debug(enum FREQ_CH ch)
{
    volatile struct freq_status freq = {0};
    volatile struct freq_s *const t_data = &t_freq[ch];
    volatile struct freq_s *const e_data = &e_freq[ch];

    freq.status = true;
    freq.ch_num = ch;
    printf("timer mode\r\n");
    freq_count(&freq, t_data, TIMER_MODE);
    ferq_dbg("\r\n- Channel  Status  Mode   Pulse(T)  Pulse(S)  Freq(Hz)  Width(us)  Empty  Tick(s)\r\n");
    printf_freq_collect_status(&freq);
    ferq_dbg("******************************************************************************\r\n");
    ferq_dbg("t_freq->mark %d\r\n", t_data->mark);
    ferq_dbg("t_freq->first %d\r\n", t_data->first);
    ferq_dbg("t_freq->second %d\r\n", t_data->second);
    ferq_dbg("t_freq->pulse_total %d\r\n", t_data->pulse_total);
    ferq_dbg("t_freq->tick_cnt %d\r\n", t_data->tick_cnt);
    ferq_dbg("t_freq->pulse_sec %d\r\n", t_data->pulse_sec);
    ferq_dbg("t_freq->pulse_width %d\r\n", t_data->pulse_width);
    ferq_dbg("t_freq->diff_value %d\r\n", t_data->diff_value);
    ferq_dbg("t_freq->time_flag %d\r\n\r\n", t_data->time_flag);

    printf("exti mode\r\n");
    freq_count(&freq, e_freq, EXTI_MODE);
    ferq_dbg("\r\n- Channel  Status  Mode   Pulse(T)  Pulse(S)  Freq(Hz)  Width(us)  Empty  Tick(s)\r\n");
    printf_freq_collect_status(&freq);
    ferq_dbg("******************************************************************************\r\n");
    ferq_dbg("e_data->mark %d\r\n", e_data->mark);
    ferq_dbg("e_data->first %d\r\n", e_data->first);
    ferq_dbg("e_data->second %d\r\n", e_data->second);
    ferq_dbg("e_data->pulse_total %d\r\n", e_data->pulse_total);
    ferq_dbg("e_data->tick_cnt %d\r\n", e_data->tick_cnt);
    ferq_dbg("e_data->pulse_sec %d\r\n", e_data->pulse_sec);
    ferq_dbg("e_data->pulse_width %d\r\n", e_data->pulse_width);
    ferq_dbg("e_data->diff_value %d\r\n", e_data->diff_value);
    ferq_dbg("e_data->time_flag %d\r\n\r\n", e_data->time_flag);
}
#endif
/**
 * @desc  : 获取频率采集结果
 * @param : ch 通道号; dat：采集状态数据存储放地址
 * @return: none
 * @Date  : 2018-12-4 10:40:17
 */
void get_freq_measure_result(enum FREQ_CH ch, struct freq_status *data)
{
    bool status = false;
    data->status = false;
    // printf_freq_debug(ch);
    // return;
    switch (ch)
    {
    case FREQ_CH_1:
    case FREQ_CH_2:
    case FREQ_CH_3:
    case FREQ_CH_4:
        data->ch_num = ch;
        status = freq_count(data, &t_freq[ch], TIMER_MODE);
        /* 定时器捕获采集失败,或者频率小于50Hz采用外部中断采集*/
        if ((data->freq_value < 50) || (status != true))
        {
            status = freq_count(data, &e_freq[ch], EXTI_MODE);
        }
        data->status = (status != true) ? false : true; /* 采集成功*/
        break;
    case FREQ_CH_ALL:
        get_freq_measure_result(FREQ_CH_1, data);
        get_freq_measure_result(FREQ_CH_2, (struct freq_status *)((uint8_t *)data + sizeof(struct freq_status)));
        get_freq_measure_result(FREQ_CH_3, (struct freq_status *)((uint8_t *)data + sizeof(struct freq_status) * 2));
        get_freq_measure_result(FREQ_CH_4, (struct freq_status *)((uint8_t *)data + sizeof(struct freq_status) * 3));
        return;
    default:
        break;
    }
    #ifdef MONITOR_DUG
    printf_freq_collect_status(data);
  #endif
}
