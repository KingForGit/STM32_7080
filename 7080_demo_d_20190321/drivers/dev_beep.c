/**
 * @author: 
 * @email:  
 * @create: 
 * @modify: 
 * @desc:   
*/
#include "dev_beep.h"
#include "task_can.h"
//static timer_t beep_en_on = NULL;
struct beep_info beep_data ={0};

void beep_start_handle( void *parameter)
{
    if(beep_data.freq)
        beep_data.freq = 0;
    else
       beep_data.freq = 1;
    
    
}
void hw_time3_init()
{
  
    extern void dev_nvic_init(IRQn_Type irq, uint8_t PreemptionPriority, uint8_t SubPriority);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_DeInit(TIM3);
    TIM_TimeBaseStructure.TIM_Period = (TIME3_MILLISECOND- 1);
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / TIME3_CLOCK - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_ARRPreloadConfig(TIM3, ENABLE);

    dev_nvic_init(TIM3_IRQn, NVIC_TIM3_PreemptionPriority, NVIC_SubPriority);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update); /* 清除溢出中断标志*/
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM3, ENABLE);

}


void TIM3_IRQHandler()
{
    if (((TIM3->SR & TIM_IT_Update) != (uint16_t)RESET) &&
        ((TIM3->DIER & TIM_IT_Update) != (uint16_t)RESET))
    {

      //  beep_flip();
      	 if( beep_data.freq)
        {
          if(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_4))
            GPIO_ResetBits(GPIOE,GPIO_Pin_4);
          else
            GPIO_SetBits(GPIOE,GPIO_Pin_4);
          
          if(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_5))
            GPIO_ResetBits(GPIOE,GPIO_Pin_5);
          else
            GPIO_SetBits(GPIOE,GPIO_Pin_5);	
        }
    }
    TIM3->SR = (uint16_t)~TIM_IT_Update;
}



//beep_en_start = soft_time_create("beep_start", beep_en_handle, NULL, 1, SOFT_TIMER_TYPE_PERIODIC);
//soft_time_start(beep_en_start);

__task void task_beep(void)
{
    OS_RESULT res;

    //soft_time_list_init();
    while (1)
    {
        res = os_evt_wait_and(BEEP_EN_EVENT, 0xffff);
        if (res == OS_R_EVT)
        {
            ;
          //  soft_time_truns();
        }
    }
}
