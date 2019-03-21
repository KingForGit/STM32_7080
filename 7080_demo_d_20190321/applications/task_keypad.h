#ifndef __TASK_KEYPAD_H__
#define __TASK_KEYPAD_H__

#include "lib_config.h"

extern OS_TID tid_task_keypad;
extern  uint8_t task_keypad_heap[KEYPAD_CMD_HEAP_SIZE];
typedef struct 
{
    uint16_t mark;
    uint8_t len;
    uint8_t data[16];
    uint16_t check;
} key_data_t;

extern key_data_t key_data_buffer;
#endif /* end of include guard: __USER_UART_H__ */
