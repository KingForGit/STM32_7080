#include "task_keypad.h"
#include "dev_uart.h"


OS_TID tid_task_keypad;
uint8_t task_keypad_heap[KEYPAD_CMD_HEAP_SIZE] = {0};
static timer_t keypad_send = NULL;

//struct key_data_t *key_data = NULL;
//uint8_t key_data_buffer[16]={0};
key_data_t key_data_buffer ={0};


/*******************************************************************************
*函数名称: unsigned int GetKeyIn()
*功    能: 获取按键按下返回值
*输    入: 无
*返    回: 管脚值
*******************************************************************************/
unsigned int GetKeyIn()
{
	unsigned int tmp = 0;


	//tmp = ((GPIOB->IDR)&0x08) | ((GPIOD->IDR>>5)&0x07);
	tmp = ((GPIOB->IDR)&0x08) | (((GPIOD->IDR)&0xE0)>>5);

	
	return tmp;
}
/*******************************************************************************
*函数名称: unsigned long long GetScanKeyRaw(void)
*功    能: 获取键盘扫描值
*输    入: 无
*返    回: 按键键值，得到键盘的扫描码,没有去抖动，如果没有键按下，返回-1
*******************************************************************************/
unsigned long long GetScanKeyRaw(void)
{
	unsigned int i;
	unsigned long long result = (unsigned long long)-1, key = (unsigned long long)-1;
//	if(keypad_mode == KEYPAD_BASIC)
	{
		if(GetKeyIn() == 0xf)		//no key pressed
		{
			memset( key_data_buffer.data, 0, 16);
			return (unsigned long long)-1;
		}
		//有按键按下
		for(i=1; i!=(1<<2); i<<=1)                   
		{
            io5_output(IO_HIGH);
            io6_output(IO_HIGH);
			if(i == 1)
				io5_output(IO_LOW);
			else if(i == 2)
				io6_output(IO_LOW);

			if(((GPIOD->IDR>>5)&0x01) == 0)//8,7
			{
				if(i == 1)
					result =  0xffffffffffffefef;
				else if(i == 2)
					result =  0xfffffffffffffefe;
			}
			else if(((GPIOD->IDR>>6)&0x01) == 0)//6,5
			{
				if(i == 1)
					result =  0xffffffffffffdfdf;
				else if(i == 2)
					result =  0xfffffffffffffdfd;  
			}
			else if(((GPIOD->IDR>>7)&0x01) == 0) //4,3
			{
				if(i == 1)
					result =  0xffffffffffffbfbf;
				else if(i == 2)
					result =  0xfffffffffffffbfb;  
			}
			else if(((GPIOB->IDR>>3)&0x01) == 0) //2,1                 
			{
				if(i == 1)
					result =  0xffffffffffff7f7f;
				else if(i == 2)
					result =  0xfffffffffffff7f7;
			}
				
            key = 0xffffffffffffffff & result;

		}
	}
	
	io5_output(IO_LOW);
	io6_output(IO_LOW);
	return key;
}



/*******************************************************************************
*函数名称: unsigned long long GetScanKeyRaw(void)
*功    能: 获取键盘扫描值
*输    入: 无
*返    回: 按键键值，得到键盘的扫描码,没有去抖动，如果没有键按下，返回-1
*******************************************************************************/
void Keypad_scan_Handler(void )
{
	static unsigned long long lastkeyraw=(unsigned long long)-1;//,  keyscancnt=0;
	static unsigned long long keyscan=(unsigned long long)-1;
	unsigned long long  keyraw;//,tk,tmp = 0;
	//static int i;
//	unsigned char key;

	keyraw = GetScanKeyRaw();
	//temp = keyraw;
	if(keyraw != lastkeyraw)
	{	//有键盘按键变化，直接return，去抖动
		lastkeyraw = keyraw;
//			return;
	}

	//键盘稳定
	//if(keyraw != keyscan)
	{	//有扫描码变化
       // memset( key_data_buffer.data, 0, 16);
        switch (keyraw)
        {
            case 0xffffffffffffefef/* 8 */:
                 key_data_buffer.data[7] = 1;
                break;
            case 0xfffffffffffffefe/* 7 */:
                 key_data_buffer.data[6] = 1;
                break;   
            case 0xffffffffffffdfdf/* 6 */:
                 key_data_buffer.data[5] = 1;
                break;
            case 0xfffffffffffffdfd/* 5 */:
                 key_data_buffer.data[4] = 1;
                break;  
            case 0xffffffffffffbfbf/* 4 */:
                 key_data_buffer.data[3] = 1;
                break;
            case 0xfffffffffffffbfb/* 3 */:
                key_data_buffer.data[2] = 1;
                break;   
            case 0xffffffffffff7f7f/* 2 */:
                key_data_buffer.data[1] = 1;
                break;
            case 0xfffffffffffff7f7/* 1 */:
                 key_data_buffer.data[0] = 1;
				//  printf("- uart test cmd, key_data_buffer: %d...\r\n", key_data_buffer.data[0]);
                break;                

            default:
                break;
        }

		keyscan = keyraw;
	}

}


static void keypad_send_handle(void *parameter)
{
    Keypad_scan_Handler();			//按键扫描
    key_data_buffer.mark = 0x0425;
    key_data_buffer.len = 16;
    key_data_buffer.check = sum_16bit_verify(key_data_buffer.data,  key_data_buffer.len);
  	uart_send_data_package(UART4,  (uint8_t*)&key_data_buffer.mark, key_data_buffer.len+ 5);
  	//uart_send_data_package(USART1,  (uint8_t*)&key_data_buffer.mark, key_data_buffer.len+ 5);
    //	printf("- keypad_send_handle.\r\n");
}
static void uart_key_handler(struct _str_uart *uart)
{
;
}

__task void task_keypad(void)
{
    OS_RESULT res;
    extern struct _str_uart uart4_recv;
   // uart_open(UART4,uart4_recv);
    keypad_send = soft_time_create("keypad_start", keypad_send_handle, NULL, 20, SOFT_TIMER_TYPE_PERIODIC);
    soft_time_start(keypad_send);

    while (1)
    {
        res = os_evt_wait_or(USART4_EVENT, 0xffff);
        if (res == OS_R_EVT)
        {
            if (os_evt_get() & USART4_EVENT)
            {
                uart_key_handler(&uart4_recv);
            }
        }
    }
}

