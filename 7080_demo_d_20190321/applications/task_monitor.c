#include "task_can.h"
#include "task_monitor.h"
#include "ops_pwm_output.h"

OS_TID tid_task_monitor;
uint8_t task_monitor_heap[MONITOR_HEAP_SIZE] = {0};
static uint8_t app_storage_mirror[STORAGE_MIRROR_LEN] = {0};
struct applications_s *const app_s = (struct applications_s *)app_storage_mirror; /* 应用变量存储镜像地址*/
#define get_app_save_offset(app_s_add) ((uint32_t)app_s_add >= (uint32_t)&app_storage_mirror[0])           \
                                           ? (int)((uint32_t)app_s_add - (uint32_t)&app_storage_mirror[0]) \
                                           : -1
struct adc_result adc_value = {0};
/*debug Para*/
/*DI*/    
struct switch_status  DebugSwConfiInfo[10]=	{0};
struct switch_status  DebugSwInfo[19]=	{0};
/*PWM*/
uint8_t DebugDuty[5]={0};
/*PI*/    
struct freq_status DebugFreq[4] ={0};
/*VI*/
struct volt_current DebugVolInfo[3] = {0};      
/*RI*/    
struct res_status DebugResInfo[2] = {0};
void ResetA20()
{
//		static uint32_t A20ResetTick=0;
//		static uint8_t GetTickFlag=0;
		pwron_off();
		// if(GetTickFlag==0)
		// {
		// 		A20ResetTick=get_hw_time_tick();
		// 		GetTickFlag=1;
		// }
		// if(get_hw_time_tick()-A20ResetTick >500)
		// {
		// 		pwron_on();	
		// 		GetTickFlag=0;
		// }
        delay_ms(500);
        pwron_on();	
		
}
/**
* @desc  : led处理
* @param : tick单位(10ms)
* @return: none
* @Date  : 2019-1-28 11:11:35
*/
void led_handle(uint32_t tick)
{
    extern struct can_info can;
    if (can.connect_state == true)
    {
        if ((tick % 5) == 0)
            led_flip();
    }
    else
    {
        if ((tick % 100) == 0)
        {
            led_flip();
            //printf("tick = %d\r\n", tick);
        }
    }
}
/******************************************************************************
*                                 数据存储读写
******************************************************************************/
/**
* @desc  : 读取存储镜像区变量
* @date  : 2019-1-28 11:37:37
*/
void read_application_info(void)
{
    int status = 0;

    if (app_s == NULL)
        return;

    if (sizeof(struct applications_s) > STORAGE_MIRROR_LEN)
    {
        printf("- App storage mirror area too small,lack %d Byte\r\n",
               (int)(sizeof(struct applications_s) - STORAGE_MIRROR_LEN));
        return;
    }
    status = storage_read(0, (uint8_t *)app_s, STORAGE_MIRROR_LEN);
    if (status != STORAGE_MIRROR_LEN)
    {
        printf("- Read storage failed. read %d byte\r\n", status);
        return;
    }

    app_s->dev_work_time = (app_s->dev_work_time >= 0xfffffffe) ? 0 : app_s->dev_work_time;

    printf("- Cumulative work %dh %dm %ds\r\n\r\n", (int)(app_s->dev_work_time / 3600),
           (int)((app_s->dev_work_time % 3600) / 60), (int)(app_s->dev_work_time % 60));
}
void write_application_info(void)
{
    int status = 0;
    status = storage_write(0, (uint8_t *)app_s, STORAGE_MIRROR_LEN);
    if (status != 0)
    {
        printf("- Write storage failed.\r\n");
    }
}
/**
* @desc  : 修改指定的存储的变量
* @param : data 结构体成员变量地址 len 变量长度
* @return: None
* @date  : 2019-1-28 11:37:50
*/
void modify_app_save_handle(uint8_t *const data, uint32_t const len)
{
    int status = 0;
    int offset = 0;
    if ((uint32_t)data < (uint32_t)&app_storage_mirror[STORAGE_MIRROR_LEN])
    {
        offset = get_app_save_offset((uint32_t)data);
        if (offset >= 0)
        {
            status = storage_write(offset, (uint8_t *)data, len);
            if (status != len)
            {
                printf("- Write storage failed.\r\n");
            }
        }
    }
    else
        printf("- Modify addr must be member of struct applications_s.\r\n");
}

__task void task_monitor(void)
{   
    static uint32_t led_tick = 0;
/*DL1-DL6 Config*/
	//	conf_switch_mode(SW_CH_14,PULL_UP,&DebugSwInfo[13]);//PWM /diL
		conf_switch_mode(SW_CH_15,PULL_UP,&DebugSwInfo[14]);
		conf_switch_mode(SW_CH_16,PULL_UP,&DebugSwInfo[15]);
		conf_switch_mode(SW_CH_17,PULL_UP,&DebugSwInfo[16]);
		conf_switch_mode(SW_CH_18,PULL_UP,&DebugSwInfo[17]);
	//	conf_switch_mode(SW_CH_19,PULL_UP,&DebugSwInfo[18]); //PWM /diL
/*DHP1-DHP4 Config*/
/*
		conf_switch_mode(SW_CH_10,PULL_DOWN,&DebugSwInfo[0]);
		conf_switch_mode(SW_CH_11,PULL_DOWN,&DebugSwInfo[1]);
		conf_switch_mode(SW_CH_12,PULL_DOWN,&DebugSwInfo[2]);
		conf_switch_mode(SW_CH_13,PULL_DOWN,&DebugSwInfo[3]);
*/
/*DAC config*/
    conf_switch_compare_volt(SW_CH_ALL,1000);
    pwm_dac_init();
/*PI Config*/  //LEVITATE
    open_freq_collect(FREQ_CH_1,PULL_DOWN,300);
    open_freq_collect(FREQ_CH_2,PULL_DOWN,300);
    open_freq_collect(FREQ_CH_3,PULL_DOWN,300);
    open_freq_collect(FREQ_CH_4,PULL_DOWN,300);
    // open_freq_collect(FREQ_CH_1,LEVITATE,300);
    // open_freq_collect(FREQ_CH_2,LEVITATE,300);
    // open_freq_collect(FREQ_CH_3,LEVITATE,300);
    // open_freq_collect(FREQ_CH_4,LEVITATE,300);
/*PWM输出Cnfig*/
		// open_pwm_outup(PWM_OUT_1,200,50);
		// open_pwm_outup(PWM_OUT_2,200,50);
		// open_pwm_outup(PWM_OUT_3,100,50);
		// open_pwm_outup(PWM_OUT_4,10,5);	
    sw1_ctrl_up(PULL_DOWN); //复用
    sw6_ctrl_up(PULL_DOWN);
    exte_vx_en_mesure();
    while (1)
    {
        led_tick++;
        app_s->dev_work_time++;
        if(pwr_gpio_state() == 0)
        {
            ResetA20();  
        }
        os_dly_wait(1);
        /* get adc basics value*/
        adc_read_value(&adc_value);
        /*LED.SYS_YUN*/
        led_handle(led_tick);
/*Get RI*/     
//					get_res_measure_result(RES_CH_1, &adc_value, &DebugResInfo[0]); //有问题，采集值不准，误差大，硬件电压不准确(=加偏移量6mv)
//        get_res_measure_result(RES_CH_2, &adc_value, &DebugResInfo[1]); //有问题，采集值不准，误差大，硬件电压不准确       
/*Get VI */         
//        get_volt_current_measure_result(VOLT_CH_1, &adc_value, &DebugVolInfo[0]);//VI1
        get_volt_current_measure_result(VOLT_CH_2, &adc_value, &DebugVolInfo[1]);//24
        get_volt_current_measure_result(VOLT_CH_3, &adc_value, &DebugVolInfo[2]);//12     
/*Get DI*/
/*
        get_switch_status(SW_CH_1,&DebugSwInfo[0]);//6U2比较器7为低，6位0.98V，1脚为1，硬件问题；
        get_switch_status(SW_CH_2,&DebugSwInfo[1]);//6U2比较器9位1.9V，8位0.98V，14脚为0；硬件问题
        get_switch_status(SW_CH_3,&DebugSwInfo[2]);//6U2比较器11为0，10位0.98V，13脚为2.7V，采集到1；硬件问题；
        get_switch_status(SW_CH_4,&DebugSwInfo[3]);
        get_switch_status(SW_CH_5,&DebugSwInfo[4]);
        get_switch_status(SW_CH_6,&DebugSwInfo[5]);
        get_switch_status(SW_CH_7,&DebugSwInfo[6]);
        get_switch_status(SW_CH_8,&DebugSwInfo[7]);
        get_switch_status(SW_CH_9,&DebugSwInfo[8]);
*/
/*				
        get_switch_status(SW_CH_10,&DebugSwInfo[9]);
        get_switch_status(SW_CH_11,&DebugSwInfo[10]);	
        get_switch_status(SW_CH_12,&DebugSwInfo[11]);
        get_switch_status(SW_CH_13,&DebugSwInfo[12]);
*/
/*
        get_switch_status(SW_CH_14,&DebugSwInfo[13]);
        get_switch_status(SW_CH_15,&DebugSwInfo[14]);
        get_switch_status(SW_CH_16,&DebugSwInfo[15]);
        get_switch_status(SW_CH_17,&DebugSwInfo[16]);
        get_switch_status(SW_CH_18,&DebugSwInfo[17]);
        get_switch_status(SW_CH_19,&DebugSwInfo[18]); 
*/				
/*PWMDOTEST   DO和PWM输出电压小于电源电压大概2V左右，硬件需要找原因*/
/*这里后期得更改，跟协议不一样，这里填的不在是电流值而是需要自己设定的占空比，
由占空比来计算电流,并且占空比越大，电流越大，can发送数据时0-1000(此处与CAN协议和相反！！！)*/
       // set_pwm_duty_cycle(PWM_OUT_1, 50);

      //  set_pwm_duty_cycle(PWM_OUT_1, 100-DebugDuty[0]);
        //set_pwm_duty_cycle(PWM_OUT_2, 100-DebugDuty[0]);
       // set_pwm_duty_cycle(PWM_OUT_3, 100-DebugDuty[0]);
     //   set_pwm_duty_cycle(PWM_OUT_4, 50);
      
/*DO*/
/*
        set_switch_output(OUT_CH_1,IO_HIGH);
        set_switch_output(OUT_CH_2,IO_HIGH);
        set_switch_output(OUT_CH_3,IO_HIGH);
        set_switch_output(OUT_CH_4,IO_HIGH);
*/     
/*beep  硬件没安装蜂鸣器
        beep_play();  
*/
/*PI相互干扰*/

//        get_freq_measure_result(FREQ_CH_1,&DebugFreq[0]);
//					get_freq_measure_result(FREQ_CH_2,&DebugFreq[1]);
//        get_freq_measure_result(FREQ_CH_3,&DebugFreq[2]);
//        get_freq_measure_result(FREQ_CH_4,&DebugFreq[3]);
        iwdg_feed();     
    }
}



