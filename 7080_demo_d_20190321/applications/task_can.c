/**
 * @author: emlsyx
 * @email:  yangx_1118@163.com
 * @create: date 2018-12-07 08:41:53
 * @modify: date 2018-12-07 08:41:53
 * @desc:   [description]
*/
#include "task_can.h"
#include "task_monitor.h"
#include "product.h"
#include "can_iap.h"
#include "dev_beep.h"
#include "ops_soft_time.h"
#include "dev_gpio.h"
OS_TID tid_task_can;
uint8_t task_can_heap[CAN_HEAP_SIZE] = {0};
 timer_t beep_en_start = NULL;
 timer_t beep_en_on = NULL;
struct excitation_info excitation   ={0};


struct can_info can = {0};

/******************************************************************************
*                                 can receive
******************************************************************************/
#define FILTER_NUM 7
static struct my_can_recv can_rx_msg[FILTER_NUM * 2];
static struct my_can_recv can_rx_copy[2]={0};
struct can_filter_item filter1item[FILTER_NUM] =
{
    /* library can id*/
    __CAN_Filter(0, ID_LIST, SCASE_32, CAN_PRO_REV1_ID, CAN_PRO_REV2_ID),
    __CAN_Filter(1, ID_LIST, SCASE_32, CAN_PRO_REV3_ID, CAN_PRO_REV4_ID),

    /* applications can id*/
    __CAN_Filter(2, ID_LIST, SCASE_32, CAN1_RX_ID0, CAN1_RX_ID1),
    __CAN_Filter(3, ID_LIST, SCASE_32, CAN1_RX_ID2, CAN1_RX_ID3),
    __CAN_Filter(4, ID_LIST, SCASE_32, CAN1_RX_ID4, CAN1_RX_ID5),
    __CAN_Filter(5, ID_LIST, SCASE_32, CAN1_RX_ID6, CAN1_RX_ID7),
    __CAN_Filter(6, ID_LIST, SCASE_32, CAN1_RX_ID8, CAN1_RX_ID9),

};
struct can_config can_cfg =
{
    250000,      /* CAN 波特率*/
    FILTER_NUM,  /* 使用过滤的个数*/
    filter1item, /* 过滤器配置表*/
};
/**
 * @desc  : can 报文解析
 * @param : data 报文地址; msg_num报文条数
 * @return: none
 * @Date  : 2018-12-7 08:41:37
 */
void can_msg_handler(CanRxMsg *data, uint16_t msg_num)
{
    uint16_t i = 0;
    extern struct can_iap_info can_iap;

    for (i = 0; i < msg_num; i++)
    {
        //printf("CAN1 ExtId[%d] = 0x%x\r\n", i, data[i].ExtId);
        switch (data[i].ExtId)
        {
        case CAN_PRO_REV1_ID:
        case CAN_PRO_REV2_ID:
        case CAN_PRO_REV3_ID:
        case CAN_PRO_REV4_ID:
            // pro_verify_can_recv(data[i].ExtId, &data[i].Data[0]);
            // if ((data[i].ExtId == CAN_PRO_REV1_ID) && (data[i].Data[7] == EC2612_CAN_IAP_TYPE_CODE))
            // {
            //     can_iap_info_init(data[i].Data); /* 初始化can iap*/
            // }
            break;
        case CAN_IAP_REV_STOP:
            break;
        case CAN_IAP_REV_DATA:
            // if (can_iap.parent.mode == true)
            // {
            //     can_iap_msg_handle(data[i].ExtId, data[i].Data, data[i].DLC); /* 处于升级模式*/
            // }
            break;
        case CAN1_RX_ID0:
            memcpy(&can_rx_msg[0].id, &data[i].ExtId, 4);
            memcpy(&can_rx_msg[0].data, data[i].Data, data[i].DLC);
            break;
        case CAN1_RX_ID1:
            memcpy(&can_rx_msg[1].id, &data[i].ExtId, 4);
            memcpy(&can_rx_msg[1].data, data[i].Data, data[i].DLC);
            break;
        case CAN1_RX_ID2:
            memcpy(&can_rx_msg[2].id, &data[i].ExtId, 4);
            memcpy(&can_rx_msg[2].data, data[i].Data, data[i].DLC);
            break;
        case CAN1_RX_ID3:
            memcpy(&can_rx_msg[3].id, &data[i].ExtId, 4);
            memcpy(&can_rx_msg[3].data, data[i].Data, data[i].DLC);
            
            break;
        case CAN1_RX_ID4:
            memcpy(&can_rx_msg[4].id, &data[i].ExtId, 4);
            memcpy(&can_rx_msg[4].data, data[i].Data, data[i].DLC);
            break;
        case CAN1_RX_ID5:
            memcpy(&can_rx_msg[5].id, &data[i].ExtId, 4);
            memcpy(&can_rx_msg[5].data, data[i].Data, data[i].DLC);
            break;
        case CAN1_RX_ID6:
            memcpy(&can_rx_msg[6].id, &data[i].ExtId, 4);
            memcpy(&can_rx_msg[6].data, data[i].Data, data[i].DLC);
            break;
        case CAN1_RX_ID7:
            memcpy(&can_rx_msg[7].id, &data[i].ExtId, 4);
            memcpy(&can_rx_msg[7].data, data[i].Data, data[i].DLC);
            break;
        case CAN1_RX_ID8:
            memcpy(&can_rx_msg[8].id, &data[i].ExtId, 4);
            memcpy(&can_rx_msg[8].data, data[i].Data, data[i].DLC);
            break;
        case CAN1_RX_ID9:
            memcpy(&can_rx_msg[9].id, &data[i].ExtId, 4);
            memcpy(&can_rx_msg[9].data, data[i].Data, data[i].DLC);
            break;
        default:
            break;
        }
    }
}
/******************************************************************************
*                                 can transmit
******************************************************************************/
void can_send_data(uint32_t id, uint8_t *data)
{
    int status = 0;
    static uint32_t tick = 0;
    CanTxMsg can_msg = {0};

    os_sem_wait(&can.send_semaphore, 0xffff);

#ifdef CAN_TYPE_EXTID
    can_msg.ExtId = (id & 0x1fffffff);
    can_msg.IDE = CAN_ID_EXT;
#else
    can_msg.StdId = (id & 0x7ff);
    can_msg.IDE = CAN_ID_STD;
#endif
    can_msg.RTR = CAN_RTR_DATA;
    can_msg.DLC = 8;

    memcpy(&can_msg.Data[0], &data[0], can_msg.DLC);
    while (1)
    {
        if ((get_hw_time_tick() - tick) < 1)
        {
            __NOP();
        }
        else
        {
            tick = get_hw_time_tick();
            break;
        }
    }
    status = can_write(&can_msg);
    os_sem_send(&can.send_semaphore);

    if (status != 1)
    {
      //  printf("- Can data send error.\r\n");
        can_control(CAN_CMD_RESET, &can_cfg);
    }
}


void API_CanRec()
{
//这里后期得更改，跟协议不一样，这里填的不在是电流值而是需要自己设定的占空比，由占空比来计算电流
		extern uint8_t DebugDuty[5];  
//		extern void beep_play(uint16_t time);
//		static uint32_t A20Heart_Tick=0;
//		static uint16_t CurValue=0;

	 	if(can_rx_msg[0].id==0x18FF8035)  //这个判断意义不大
     {
			 if((can_rx_msg[3].data[0]==0xAA)&&(beep_data.can_beep_en == false))
			 {
                beep_data.can_beep_en = true;
                beep_data.BeepOnTime = (uint16_t)(can_rx_msg[3].data[1] << 8) + (uint16_t)(can_rx_msg[3].data[2]);
                beep_data.BeepOnTime = (beep_data.BeepOnTime>10000) ? 10000:beep_data.BeepOnTime;
                beep_data.BeepOnTime = (beep_data.BeepOnTime<100) ? 100:beep_data.BeepOnTime;
                beep_data.BeepOnTime /= 10;

                GPIO_SetBits(GPIOE,GPIO_Pin_4);
                GPIO_ResetBits(GPIOE,GPIO_Pin_5);
               if(beep_en_start == NULL)
                    beep_en_start = soft_time_create("beep_start", beep_start_handle, &beep_data, 50, SOFT_TIMER_TYPE_PERIODIC);
               soft_time_start(beep_en_start);	
                /*SOFT_TIMER_TYPE_ONE_SHOT,SOFT_TIMER_TYPE_PERIODIC*/

          }
			 else if((can_rx_msg[3].data[0]!=0xAA)&&(beep_data.can_beep_en == true))
			 {  
                    soft_time_stop(beep_en_start);	
					beep_data.can_beep_en = false;
                    beep_data.BeepOnTime=0;
                     beep_data.freq = 0;
                    beep1_off();
                    beep2_off();
       
			 }
     }

    //--
	// 	if(can_rx_msg[3].id==0x1FFFFFA3)  //这个判断意义不大
    //      {
	// 		 if((can_rx_msg[3].data[0]==0xAA)&&(beep_data.can_beep_en == false))
	// 		 {
    //             beep_data.can_beep_en = true;
    //             beep_data.BeepOnTime = (uint16_t)(can_rx_msg[3].data[1] << 8) + (uint16_t)(can_rx_msg[3].data[2]);
    //             beep_data.BeepOnTime = (beep_data.BeepOnTime>10000) ? 10000:beep_data.BeepOnTime;
    //             beep_data.BeepOnTime = (beep_data.BeepOnTime<100) ? 100:beep_data.BeepOnTime;
    //             beep_data.BeepOnTime /= 10;

    //             GPIO_SetBits(GPIOE,GPIO_Pin_4);
    //             GPIO_ResetBits(GPIOE,GPIO_Pin_5);
    //            if(beep_en_start == NULL)
    //                 beep_en_start = soft_time_create("beep_start", beep_start_handle, &beep_data, 50, SOFT_TIMER_TYPE_PERIODIC);
    //            soft_time_start(beep_en_start);	
    //             /*SOFT_TIMER_TYPE_ONE_SHOT,SOFT_TIMER_TYPE_PERIODIC*/

    //       }
	// 		 else if((can_rx_msg[3].data[0]!=0xAA)&&(beep_data.can_beep_en == true))
	// 		 {  
    //                 soft_time_stop(beep_en_start);	
	// 				beep_data.can_beep_en = false;
    //                 beep_data.BeepOnTime=0;
    //                  beep_data.freq = 0;
    //                 beep1_off();
    //                 beep2_off();
       
	// 		 }
    // }
//--

		//  if(can_rx_msg[1].id==0x18FF8036)
		//  {
        //     if(0 !=  memcmp(&can_rx_msg[4].data, &can_rx_copy[0].data, 8))
        //     {
        //         memcpy(&can_rx_copy[0].data, &can_rx_msg[4].data, 8);
        //         excitation.exc_can = true;
        //         excitation.exc_system =can_rx_msg[4].data[7];
        //         excitation.exc_cycle =  can_rx_msg[4].data[2];
        //         if(can_rx_msg[4].data[0]==0xBB && excitation.exc_en==false)
        //         {
        //             excitation.exc_paragraph = can_rx_msg[4].data[1];
        //             excitation.exc_en=true;
        //         }
        //         else 
        //         {
        //             excitation.exc_en=false;
        //         }
        //     }
        //     else
        //     {
        //          excitation.exc_can = false;
        //     }
            	 
		//  }
 		if(can_rx_msg[4].id==0x1FFFFFA4)
		{
            if(0 !=  memcmp(&can_rx_msg[4].data, &can_rx_copy[0].data, 8))
            {
                memcpy(&can_rx_copy[0].data, &can_rx_msg[4].data, 8);
                excitation.exc_can = true;
                excitation.exc_system =can_rx_msg[4].data[7];
                excitation.exc_cycle =  can_rx_msg[4].data[2];
                if(can_rx_msg[4].data[0]==0xBB && excitation.exc_en==false)
                {
                    excitation.exc_paragraph = can_rx_msg[4].data[1];
                    excitation.exc_en=true;
                }
                else 
                {
                    excitation.exc_en=false;
                }
            }
            else
            {
                 excitation.exc_can = false;
            }
            	 
		}      
//---        
		//复位A20
		// if(can_rx_msg[2].id==0x18EE0011)
		// {
		// 	 if(can_rx_msg[1].data[0]==0x00)
		// 	 {
		// 			A20Heart_Tick=get_hw_time_tick();				
		// 	 }
		// }	 
		// if(get_hw_time_tick()-A20Heart_Tick >3000)
		// {
		// 	//	ResetA20();
		// 		A20Heart_Tick=get_hw_time_tick();
		// }
}

void API_CanTraRec(void *Parameter)
{
	static uint8_t Tradata[8]={0};
		extern struct switch_status  DebugSwInfo[19];
		extern struct volt_current   DebugVolInfo[3]; 
		extern struct res_status     DebugResInfo[2];
		extern struct freq_status    DebugFreq[4];
        uint16_t canvolFreq;

    Tradata[0] = (DebugSwInfo[0].sw_status      |  0xfe)  & (DebugSwInfo[1].sw_status<<1  |   0xfd)  & (DebugSwInfo[2].sw_status<<2  |  0xfb)  & 
								 (DebugSwInfo[3].sw_status<<3   |  0xf7)  & (DebugSwInfo[4].sw_status<<4  |   0xef)  & (DebugSwInfo[5].sw_status<<5  |  0xdf)  & 
								 (DebugSwInfo[6].sw_status<<6   |  0xbf)  & (DebugSwInfo[7].sw_status<<7  |   0x7f);
    // Tradata[1] = (DebugSwInfo[8].sw_status      |  0xfe)  & (DebugSwInfo[13].sw_status<<1 |   0xfd)  & (DebugSwInfo[14].sw_status<<2 |  0xfb)  & 
	// 							 (DebugSwInfo[15].sw_status<<3  |  0xf7)  & (DebugSwInfo[16].sw_status<<4 |   0xef)  & (DebugSwInfo[17].sw_status<<5 |  0xdf)  &
	// 							 (DebugSwInfo[18].sw_status<<6  |  0xbf)  & ((uint8_t)0<<7                |   0x7f);
    // can_send_data(0x18FF8031,Tradata);
    Tradata[1] = (DebugSwInfo[8].sw_status      |  0xfe) &( (uint8_t)0<<1  |   0xfd)  & (DebugSwInfo[13].sw_status<<2 |  0xfb)  & 
								 (DebugSwInfo[14].sw_status<<3  |  0xf7)  & (DebugSwInfo[15].sw_status<<4 |   0xef)  & (DebugSwInfo[16].sw_status<<5 |  0xdf)  &
								 (DebugSwInfo[17].sw_status<<6    |  0xbf)  & (DebugSwInfo[18].sw_status<<7                |   0x7f);
 
    can_send_data(0x18FF8031,Tradata);
    memset(Tradata,0,8*sizeof(Tradata[0]));
	//这里不知道协议里面有没有分辨率，建议加0.01分辨率
   // DebugVolInfo[1].volt_value =(float)DebugVolInfo[1].volt_value * 1000;
    Tradata[0] = ((uint16_t)DebugVolInfo[1].volt_value)  & 0xff;//24V
    Tradata[1] = ((uint16_t)DebugVolInfo[1].volt_value) >> 8;  
    Tradata[2] = ((uint16_t)DebugResInfo[0].res_value)   & 0xff; 
    Tradata[3] = ((uint16_t)DebugResInfo[0].res_value)   >> 8; 
    Tradata[4] = ((uint16_t)DebugResInfo[1].res_value)    & 0xff; 
    Tradata[5] = ((uint16_t)DebugResInfo[1].res_value)   >> 8; 
/*后期这里加CurIn数据*/
   // DebugVolInfo[2].volt_value *= 1000;
    Tradata[6] =((uint16_t)DebugVolInfo[2].volt_value)    & 0xff;//12V 
    Tradata[7] =((uint16_t)DebugVolInfo[2].volt_value)   >> 8;         
    can_send_data(0x18FF8032,Tradata);
    memset(Tradata,0,8*sizeof(Tradata[0]));
  //  DebugVolInfo[0].volt_value *= 1000;
    Tradata[0] = ((uint16_t)DebugVolInfo[0].volt_value)  & 0xff;//VI
    Tradata[1] = ((uint16_t)DebugVolInfo[0].volt_value)  >> 8;  
/*这里要加DO/PWM输出电压*/    
/*
    Tradata[2] = ((uint16)DebugResInfo[0].res_value)   & 0xff; 
    Tradata[3] = ((uint16)DebugResInfo[0].res_value)  >> 8; 
    Tradata[4] = ((uint16)DebugResInfo[1].res_value)   & 0xff; 
    Tradata[5] = ((uint16)DebugResInfo[1].res_value)  >> 8;   
*/  can_send_data(0x18FF8033,Tradata);
    memset(Tradata,0,8*sizeof(Tradata[0]));
    canvolFreq = DebugFreq[0].freq_value *100;
    Tradata[0] = canvolFreq    & 0xff; 
    Tradata[1] = canvolFreq   >> 8; 
    canvolFreq = DebugFreq[1].freq_value *100;
    Tradata[2] = canvolFreq    & 0xff; 
    Tradata[3] = canvolFreq   >> 8; 
    canvolFreq = DebugFreq[2].freq_value *100;
    Tradata[4] = canvolFreq    & 0xff; 
    Tradata[5] = canvolFreq   >> 8; 
    canvolFreq = DebugFreq[3].freq_value; //*100;
    Tradata[6] = canvolFreq    & 0xff; 
    Tradata[7] = canvolFreq   >> 8;                    
    can_send_data(0x18FF8034,Tradata);
    memset(Tradata,0,8*sizeof(Tradata[0]));
		API_CanRec();
}
//励磁控制
void API_excitation()
{
    extern struct switch_status  DebugSwInfo[19];
     extern void gpio_set_mode(GPIO_TypeDef * port, uint16_t pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);
   if( excitation.exc_can == false)
   return;
   if(excitation.exc_system ==1)
   {

         sw6_ctrl_up(PULL_DOWN);  
         conf_switch_mode(SW_CH_14,PULL_UP,&DebugSwInfo[13]);
        if(excitation.exc_en == true)    //27脚是12伏
        {
            open_pwm_outup(PWM_OUT_2,600,excitation.exc_cycle);
           // set_pwm_duty_cycle(PWM_OUT_2, 6);
        }
        else
        {
           close_pwm_output(PWM_OUT_2);
        }
        gpio_set_mode(PORT_IO_OUTPUT_1, PIN_IO_OUTPUT_1, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
        io1_output(IO_LOW);    
 
   }
   else if( excitation.exc_system==2)
   {    

       sw1_ctrl_up(PULL_DOWN);
       conf_switch_mode(SW_CH_19,PULL_UP,&DebugSwInfo[18]);
        if(excitation.exc_en == true)      //10脚24伏
        {
            open_pwm_outup(PWM_OUT_1,600,excitation.exc_cycle);
            //set_pwm_duty_cycle(PWM_OUT_1, 60);
        }
        else
        {
            close_pwm_output(PWM_OUT_1);
        }
        gpio_set_mode(PORT_IO_OUTPUT_2, PIN_IO_OUTPUT_2, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
        io2_output(IO_LOW);    
   }
    else//if( excitation.exc_system==3)
    {
        gpio_set_mode(PORT_IO_OUTPUT_1, PIN_IO_OUTPUT_1, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
        gpio_set_mode(PORT_IO_OUTPUT_2, PIN_IO_OUTPUT_2, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
        gpio_set_mode(PORT_IO_OUTPUT_3, PIN_IO_OUTPUT_3, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
        gpio_set_mode(PORT_IO_OUTPUT_4, PIN_IO_OUTPUT_4, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);      
        io1_output(IO_LOW);
        io2_output(IO_LOW);
        io3_output(IO_LOW);
        io4_output(IO_LOW);
        close_pwm_output(PWM_OUT_1);
        close_pwm_output(PWM_OUT_2);
     //   sw1_ctrl_up(PULL_DOWN);
     //   sw6_ctrl_up(PULL_DOWN);  
    }
}

static timer_t BoardDataCanTraRec = NULL;
/******************************************************************************
*                                 can thread
******************************************************************************/
__task void task_can(void)
{
    uint32_t msg_num = 0;
    uint32_t tick;
    CanRxMsg read_buff[CAN_FIFO_MAX_ITEM] = {0};

    os_sem_init(&can.send_semaphore, 1);
    can_open(&can_cfg);
    BoardDataCanTraRec = soft_time_create("BoardDataCanTraRec", API_CanTraRec, NULL, 6, SOFT_TIMER_TYPE_PERIODIC);
    soft_time_start(BoardDataCanTraRec);

    while (1)
    {
        os_dly_wait(1);
        memset(&read_buff[0], 0, sizeof(read_buff));
        can_control(CAN_CMD_GET_MSG_NUM, &msg_num);
        can_item_verify(msg_num);
        if (msg_num > 0)
        {
            can.cnt_disconnect = 0;
            can.connect_state = false;
            can_read(&read_buff[0], msg_num);
            can_msg_handler(&read_buff[0], msg_num);
        }
        else
        {
            /* 3秒收不到数据 认为物理总线异常*/
            if (can.cnt_disconnect++ > 300)
            {
                can.connect_state = true;
            }
        }
        API_excitation();
 
        tick++;
        led_handle(tick);
    }
}
