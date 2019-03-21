#ifndef __DEV_DAC_H__
#define __DEV_DAC_H__

#include "board.h"

extern void dac_set_output(uint32_t DAC_Channel, uint16_t value);
extern uint16_t dac_get_value(uint32_t DAC_Channel);
#endif /* end of include guard: __DEV_DAC_H__*/
