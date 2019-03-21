/**
 * @author: emlsyx
 * @email:  yangx_1118@163.com
 * @create: date 2018-12-03 08:48:06
 * @modify: date 2018-12-03 08:48:06
 * @desc:   [description]
*/
#include "ops_soft_time.h"

static volatile uint32_t time_cnt_sec = 0; /* 时间计数*/

void hw_time6_init()
{
    extern void dev_nvic_init(IRQn_Type irq, uint8_t PreemptionPriority, uint8_t SubPriority);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    TIM_DeInit(TIM6);
    TIM_TimeBaseStructure.TIM_Period = (TIME6_MILLISECOND * 10 - 1);
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / 10000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
    TIM_ARRPreloadConfig(TIM6, ENABLE);

    dev_nvic_init(TIM6_IRQn, NVIC_TIM6_PreemptionPriority, NVIC_SubPriority);
    TIM_ClearFlag(TIM6, TIM_FLAG_Update); /* 清除溢出中断标志*/
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM6, ENABLE);
}

uint32_t get_hw_time_tick()
{
    return (uint32_t)time_cnt_sec;
}
static void (*irq_timer6_hook1)(void *pram);
void irq_timer6_sethook(void (*hook)(void *pram))
{
    if (irq_timer6_hook1 == NULL)
        irq_timer6_hook1 = hook;
    else
    {
        printf("rt_irq_timer6_sethook failed\r\n");
    }
}

void TIM6_IRQHandler()
{
    if (((TIM6->SR & TIM_IT_Update) != (uint16_t)RESET) &&
        ((TIM6->DIER & TIM_IT_Update) != (uint16_t)RESET))
    {
        time_cnt_sec++;
        if (time_cnt_sec >= 0xffffffff)
            time_cnt_sec = 0;
#ifdef EN_OPS_SOFT_TIME
        if ((time_cnt_sec % SOFT_TIME_INTERVAL) == 0)
            isr_evt_set(SOFT_TIME_EVENT, tid_task_soft_time);
#endif
    }
    TIM6->SR = (uint16_t)~TIM_IT_Update;
}

/******************************************************************************
*                                 延时函数
******************************************************************************/
void tick_delay_ms(uint32_t delayms)
{
    uint32_t tick = get_hw_time_tick();

    delayms = (delayms > 2500) ? 2500 : delayms;
    while (1)
    {
        if ((get_hw_time_tick() - tick) < delayms)
        {
            __NOP();
        }
        else
        {
            break;
        }
    }
}

/**
 * @desc  : 根据_nop 指令进行延时,一个_nop指令为1/7200000s 占10个机器指令周期
 * @param : us 值
 * @return: None
 * @Date  : 2018-5-22 10:31:15
 */
void delay_us(uint32_t us)
{
    uint32_t i;
    us /= 10;
    for (i = 0; i < (72 * us); i++)
    {
        __NOP();
    }
}
/**
 * @desc  : 根据_nop 指令进行延时,一个_nop指令为1/72000000s
 * @param : ms 值
 * @return: None
 * @Date  : 2018-5-22 10:31:15
 */
void delay_ms(uint32_t ms)
{
    uint32_t i;
    for (i = 0; i < ms; i++)
    {
        delay_us(1000);
    }
}
/******************************************************************************
*                                 Soft time
******************************************************************************/
OS_TID tid_task_soft_time;
#ifdef EN_OPS_SOFT_TIME
uint8_t task_soft_time_heap[SOFT_TIME_HEAP_SIZE] = {0};
static struct tcb_soft_time soft_time_list[SOFT_TIME_NUM_MAX];
#else
uint8_t task_soft_time_heap[1] = {0};
#endif

static int _soft_time_default_init(timer_t t)
{
    if (t != NULL)
    {
        memcpy(t->name, "\0", SOFT_TIME_NAME_MAX);
        t->flag = SOFT_TIMER_FLAG_DEACTIVATED;
        t->type = SOFT_TIMER_TYPE_ONE_SHOT;
        t->fun = NULL;
        t->parameter = NULL;
        t->init_tick = 0;
        t->free_tick = t->init_tick;
        return 0;
    }
    return -1;
}
void soft_time_view(timer_t t)
{
#ifdef EN_OPS_SOFT_TIME
    if (t != NULL)
    {
        printf("- ********************* No.%d *********************\r\n", t->num);
        printf("- soft time name \"%s\"\r\n", t->name);
        printf("- soft time flag %d\r\n", t->flag);
        printf("- soft time type %d\r\n", t->type);
        printf("- soft time init tick %d\r\n", t->init_tick);
        printf("- soft time free tick %d\r\n\r\n", t->free_tick);
    }
    else
    {
        printf("- soft timer null!\r\n");
    }
#endif
}
void soft_time_printf_list(void)
{
#ifdef EN_OPS_SOFT_TIME
    uint8_t i = 0;

    for (i = 0; i < SOFT_TIME_NUM_MAX; i++)
    {
        soft_time_view(&soft_time_list[i]);
    }
#endif
}
static void soft_time_list_init(void)
{
#ifdef EN_OPS_SOFT_TIME
    uint8_t i = 0;
    for (i = 0; i < SOFT_TIME_NUM_MAX; i++)
    {
        soft_time_list[i].num = i;
        _soft_time_default_init(&soft_time_list[i]);
    }
#endif
}
/**
 * @desc  : 软件定时器创建
 * @param : name 定时器名称; fun() 超时处理任务; par 超时处理参数; time 超时时间; type 定时器类型
 * @return: 成功:定时器指针;失败:NULL
 * @Date  : 2018-12-2 15:11:18
 */
timer_t soft_time_create(const char *name,
                         void (*fun)(void *parameter),
                         void *parameter, uint32_t time,
                         uint8_t type)
{
#ifdef EN_OPS_SOFT_TIME
    uint8_t i = 0;

    for (i = 0; i < SOFT_TIME_NUM_MAX; i++)
    {
        if (strlen(soft_time_list[i].name) <= 0)
        {
            memcpy(soft_time_list[i].name, name, SOFT_TIME_NAME_MAX);
            break;
        }
    }
    if (i < SOFT_TIME_NUM_MAX)
    {
        soft_time_list[i].num = i;
        soft_time_list[i].type = type;
        soft_time_list[i].fun = fun;
        soft_time_list[i].parameter = parameter;
        soft_time_list[i].init_tick = time;
        soft_time_list[i].free_tick = soft_time_list[i].init_tick;
        return &soft_time_list[i];
    }
#endif
    return NULL;
}
int soft_time_delete(timer_t *t)
{
#ifdef EN_OPS_SOFT_TIME
    if ((*t) != NULL)
    {
        memcpy((*t)->name, "\0", SOFT_TIME_NAME_MAX);
        (*t)->flag = SOFT_TIMER_FLAG_DEACTIVATED;
        (*t)->type = SOFT_TIMER_TYPE_ONE_SHOT;
        (*t)->fun = NULL;
        (*t)->parameter = NULL;
        (*t)->init_tick = 0;
        (*t)->free_tick = (*t)->init_tick;
        (*t) = NULL;
        return 0;
    }
#endif
    return -1;
}
int soft_time_stop(timer_t t)
{
#ifdef EN_OPS_SOFT_TIME
    if (t != NULL)
    {
        if (strlen(t->name) > 0)
        {
            t->flag = SOFT_TIMER_FLAG_DEACTIVATED;
        }

        return 0;
    }
#endif
    return -1;
}
int soft_time_start(timer_t t)
{
#ifdef EN_OPS_SOFT_TIME
    if (t != NULL)
    {
        if (strlen(t->name) > 0)
        {
            if (t->flag == SOFT_TIMER_FLAG_DEACTIVATED)
            {
                t->flag = SOFT_TIMER_FLAG_ACTIVATED;
            }
        }
        return 0;
    }
#endif
    return -1;
}
int soft_time_control(timer_t t, int cmd, void *arg)
{
#ifdef EN_OPS_SOFT_TIME
    /* timer check */
    //RT_ASSERT(timer != RT_NULL);

    if (t == NULL)
        return -1;

    switch (cmd)
    {
    case SOFT_TIMER_CTRL_GET_TIME:
        *(int32_t *)arg = t->free_tick;
        break;

    case SOFT_TIMER_CTRL_SET_TIME:
        t->init_tick = *(int32_t *)arg;
        t->free_tick = *(int32_t *)arg;
        break;

    case SOFT_TIMER_CTRL_SET_ONESHOT:
        t->type = SOFT_TIMER_TYPE_ONE_SHOT;
        break;

    case SOFT_TIMER_CTRL_SET_PERIODIC:
        t->type = SOFT_TIMER_TYPE_PERIODIC;
        break;
    }
#endif
    return 0;
}
static void soft_time_truns(void)
{
#ifdef EN_OPS_SOFT_TIME
    uint8_t i = 0;
    for (i = 0; i < SOFT_TIME_NUM_MAX; i++)
    {
        /* soft time fun*/
        if ((strlen(soft_time_list[i].name) > 0) && (soft_time_list[i].flag == SOFT_TIMER_FLAG_ACTIVATED))
        {
            soft_time_list[i].free_tick--;
            if (soft_time_list[i].free_tick <= 0)
            {
                /* soft timer run function*/
                soft_time_list[i].fun(soft_time_list[i].parameter);

                /* soft timer mode handle*/
                if (soft_time_list[i].type == SOFT_TIMER_TYPE_PERIODIC)
                {
                    /* soft timer free tick reset*/
                    soft_time_list[i].free_tick = soft_time_list[i].init_tick;
                }
                else
                {
                    soft_time_list[i].flag = SOFT_TIMER_FLAG_DEACTIVATED;
                }
            }
        }
    }
#endif
}
__task void task_soft_time(void)
{
    OS_RESULT res;

    /* soft time list init*/
    soft_time_list_init();
    while (1)
    {
        res = os_evt_wait_and(SOFT_TIME_EVENT, 0xffff);
        if (res == OS_R_EVT)
        {
            soft_time_truns();
        }
    }
}
