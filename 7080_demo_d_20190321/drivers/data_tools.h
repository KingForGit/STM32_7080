#ifndef __DATA_TOOLS_H__
#define __DATA_TOOLS_H__

#include "board.h"



typedef enum
{
    PRINT_HEX = 0,
    PRINT_DEC = 1,
    PRINT_CHAR = 2,
} print_mode;
struct check_range
{
    uint8_t min;
    uint8_t max;
};


extern void ascii_to_hex(char *des, char *src, int size);
extern void hex_to_ascii_print(char hex);
extern char *my_strstr(char *s1,char* s2,int max_len);
extern uint16_t my_htons(uint16_t data);
extern uint32_t my_htonl(uint32_t data);
extern uint8_t get_given_string(uint8_t *src, uint8_t ch, uint8_t *des);
extern uint8_t get_given_number(uint8_t *src, uint8_t ch, uint16_t *des);
extern uint8_t xor_8bit_verify(uint8_t *buf, uint32_t len);
extern uint16_t sum_16bit_verify(uint8_t *buf, uint32_t len);
extern void mem_printf(uint8_t *data, uint16_t len,print_mode mode);
extern uint16_t my_rand(uint16_t x, uint16_t y);
extern int16_t check_data_in_assign_range(struct check_range range[],uint8_t r_num, uint8_t *data, uint16_t len);
#endif /* end of include guard: __DATA_TOOLS_H__ */
