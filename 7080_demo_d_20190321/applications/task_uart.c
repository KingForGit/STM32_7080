#include "task_uart.h"
#include "task_can.h"
#include "task_monitor.h"
#include "dev_dac.h"

OS_TID tid_task_uart;
uint8_t task_uart_heap[UART_CMD_HEAP_SIZE] = {0};

static void uart_cmd_handler(struct _str_uart *uart)
{
    char *p = NULL;
    int dac1_v = 0;
    float out_freq = 0;
    float duty_cycle = 0;
    char buff[30] = {0};

    extern struct adc_result adc_value;
    struct volt_current power_volt = {0};
    struct volt_current volt_value[4] = {0};
    struct volt_current curr_value[2] = {0};
    struct res_status res_value[4] = {0};
    struct switch_status sw_value[19] = {0};
    struct freq_status freq_value[4] = {0};

    p = strstr((char *)(uart->data), "Reset");
    if (p != NULL)
    {
        printf("- Reset be from: Usart cmd...\r\n");
        NVIC_SystemReset();
    }
    p = strstr((char *)(uart->data), "IIAP_STM32");
    if (p != NULL)
    {
        uart_iap_handle(p);
    }
    p = strstr((char *)(uart->data), "can_status");
    if (p != NULL)
    {
        can_status();
    }

    p = strstr((char *)(uart->data), "get_");
    if (p != NULL)
    {
        if (strstr(p, "power") != NULL)
        {
            debug_volt_current_collect();
            get_volt_current_measure_result(VOLT_CH_BAT, &adc_value, &power_volt);
        }
        if (strstr(p, "volt") != NULL)//gol 3lu
        {
            debug_volt_current_collect(); /* get volt value*/
            get_volt_current_measure_result(VOLT_CH_ALL, &adc_value, &volt_value[0]);
        }
        if (strstr(p, "curr") != NULL)//gol 1lu
        {
            debug_volt_current_collect(); /* get current value*/
            get_volt_current_measure_result(CURR_CH_ALL, &adc_value, &curr_value[0]);
        }
        if (strstr(p, "res") != NULL)//GOL 2lu
        {
            debug_res_collect(); /* get resistance value*/
            get_res_measure_result(RES_CH_ALL, &adc_value, &res_value[0]);
        }
        if (strstr(p, "switch") != NULL)
        {
            debug_sw_collect(); /* get switching status*/
            get_switch_status(SW_CH_ALL, &sw_value[0]);
        }
        if (strstr(p, "freq") != NULL)
        {
            debug_freq_collect(); /* get freq status*/
            get_freq_measure_result(FREQ_CH_ALL, &freq_value[0]);
        }
        if (strstr(p, "dac:") != NULL)
        {
            sscanf(p, "get_dac:%d", &dac1_v);

            if (dac1_v == 1)
            {
                printf("- Dac1 volt %dmv\r\n", dac_get_value(DAC_Channel_1));
            }
            else if (dac1_v == 2)
            {
                printf("- Dac2 volt %dmv\r\n", dac_get_value(DAC_Channel_2));
            }
            else
            {
                printf("- Please enter get_dac:1 or get_dac:2\r\n");
            }
        }
    }

    p = strstr((char *)(uart->data), "set_pwm:");
    if (p != NULL)
    {
        sscanf(p, "set pwm:%f,%f", &out_freq, &duty_cycle);
        printf("- pwn output succress. %.2fHz,%.1f%%\r\n", out_freq, duty_cycle);
        open_pwm_outup(PWM_OUT_1, out_freq, duty_cycle);
    }

    p = strstr((char *)(uart->data), "open_freq:");
    if (p != NULL)
    {
        sscanf(p, "open_freq:%d", &dac1_v);
        open_freq_collect(FREQ_CH_ALL, LEVITATE, (uint16_t)dac1_v);
        printf("- open freq succress\r\n");
    }

    p = strstr((char *)(uart->data), "set_switch");
    if (p != NULL)
    {
        set_switch_output(OUT_CH_ALL, IO_HIGH);
        printf("- switch output high succress\r\n");
    }

    p = strstr((char *)(uart->data), "get_mcu_id");
    if (p != NULL)
    {
        GET_STM32_MCU_ID(buff);
        printf("- Stm32 mcu id: %s\r\n", buff);
    }

    p = strstr((char *)(uart->data), "storage_format");
    if (p != NULL)
    {
        printf("- Format storage\r\n");
        storage_format();
    }
    p = strstr((char *)(uart->data), "test");
    if (p != NULL)
    {
        printf("- uart test cmd, cmd len: %s...\r\n", p);
    }

    uart->flag = false;
    uart->index = 0;
    uart->this_cnt = 0;
    memset(uart->data, 0, uart->this_cnt);
}

__task void task_uart(void)
{
    OS_RESULT res;
    extern struct _str_uart uart1_recv;

    uart_open(USART1,uart1_recv);
    while (1)
    {
        res = os_evt_wait_or(USART_EVENT, 0xffff);
        if (res == OS_R_EVT)
        {
            if (os_evt_get() & USART_EVENT)
            {
                uart_cmd_handler(&uart1_recv);
            }
        }
    }
}
