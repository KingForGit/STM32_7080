#ifndef __DEV_UART_H__
#define __DEV_UART_H__

#include "board.h"
#define RT_USING_UART1
#define RT_USING_UART4
#define UART_BAUD (uint32_t)115200 /* 串口波特率*/

#define USART_EVENT 0x0001 /* 串口接收完成事件*/
#define USART_MAX_LEN 2048
#define USART4_EVENT 0x0001 /* 串口4接收完成事件*/
#define USART4_MAX_LEN 512
struct _str_uart
{
    volatile bool flag;
    volatile uint16_t this_cnt;
    volatile uint16_t index;
    char *data;
};
extern struct _str_uart uart1_recv;
extern void uart_send_data_package(USART_TypeDef *USARTx, uint8_t *data, uint16_t len);
extern void uart_iap_handle(char *data);
extern void uart_close(USART_TypeDef *USARTx,struct _str_uart uart_recv);
extern void uart_open(USART_TypeDef *USARTx,struct _str_uart uart_recv);
#endif /* end of include guard: __DEV_UART_H__ */
