/**
 * @author: emlsyx
 * @email:  yangx_1118@163.com
 * @create: date 2018-11-23 18:50:41
 * @modify: date 2018-11-23 18:50:41
 * @desc:   [description]
*/
#include "ops_volt_current.h"

//#define DEBUG_VOLT_COLLECT
#ifdef DEBUG_VOLT_COLLECT
    #define volt_current_collect_dbg(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define volt_current_collect_dbg(fmt, ...)
#endif

static void printf_volt_current_collect_status(struct volt_current *data)
{
    if (data->ch_num == VOLT_CH_BAT)
    {
        volt_current_collect_dbg("- Power volt      ");
    }
    else if (data->ch_num == CURR_CH_1)
    {
        volt_current_collect_dbg("- Currect NO.1    ");
    }
    else if (data->ch_num == CURR_CH_2)
    {
        volt_current_collect_dbg("- Currect NO.2    ");
    }
    else
    {
        volt_current_collect_dbg("- Voltage NO.%d    ", data->ch_num);
    }

    volt_current_collect_dbg("%07.2f       ", data->adc_value);
    volt_current_collect_dbg("%07.3f         ", (float)data->matching_r / 1000);
    volt_current_collect_dbg("%07.3f       ", (float)data->bleeder_r / 1000);
    volt_current_collect_dbg("%02d      ", data->proportion);
    volt_current_collect_dbg("%07.2f     ", data->volt_value);
    volt_current_collect_dbg("%07.3f\r\n", data->current_value);
}
void get_volt_current_measure_result(enum VOLT_CH ch, struct adc_result *adc, struct volt_current *data)
{
    float temp = 0.0L;
    if (false == adc->status)
    {
        return;
    }
    switch (ch)
    {
    case VOLT_CH_1:
        data->ch_num = VOLT_CH_1;
        data->adc_value = adc->voltage_1;
        data->matching_r = VOLT_MATCHING_R;
        data->bleeder_r = VOLT_BLEEDER_R;
        data->proportion = SAMPLI_VOLT;
        break;
    case VOLT_CH_2:
        data->ch_num = VOLT_CH_2;
        data->adc_value = adc->voltage_2;
        data->matching_r = BAT_MATCHING_R;
        data->bleeder_r = BAT_BLEEDER_R;
        data->proportion = SAMPLI_BAT;
        break;
    case VOLT_CH_3:
        data->ch_num = VOLT_CH_3;
        data->adc_value = adc->voltage_3;
        data->matching_r = VOLT_MATCHING_R;
        data->bleeder_r = VOLT_BLEEDER_R;
        data->proportion = SAMPLI_VOLT;
        break;
		/*
    case VOLT_CH_4:
        data->ch_num = VOLT_CH_4;
        data->adc_value = adc->voltage_4;
        data->matching_r = VOLT_MATCHING_R;
        data->bleeder_r = VOLT_BLEEDER_R;
        data->proportion = SAMPLI_VOLT;
        break;
		*/
    case CURR_CH_1:
        data->ch_num = CURR_CH_1 - VOLT_CH_4;
        data->adc_value = adc->current_1;
        data->matching_r = CU_MATCHING_R;
        data->bleeder_r = CU_BLEEDER_R;
        data->proportion = SAMPLI_CURR;
        break;
		/*
    case CURR_CH_2:
        data->ch_num = CURR_CH_2 - VOLT_CH_4;
        data->adc_value = adc->current_2;
        data->matching_r = CU_MATCHING_R;
        data->bleeder_r = CU_BLEEDER_R;
        data->proportion = SAMPLI_CURR;
        break;
    case VOLT_CH_BAT:
        data->ch_num = VOLT_CH_BAT;
        data->adc_value = adc->car_bat_v;
        data->matching_r = BAT_MATCHING_R;
        data->bleeder_r = BAT_BLEEDER_R;
        data->proportion = SAMPLI_BAT;
        break;
				*/
    case VOLT_CH_ALL:
        get_volt_current_measure_result(VOLT_CH_1, adc, data);
        get_volt_current_measure_result(VOLT_CH_2, adc, (struct volt_current *)((uint8_t *)data + sizeof(struct volt_current)));
        get_volt_current_measure_result(VOLT_CH_3, adc, (struct volt_current *)((uint8_t *)data + sizeof(struct volt_current) * 2));
 //       get_volt_current_measure_result(VOLT_CH_4, adc, (struct volt_current *)((uint8_t *)data + sizeof(struct volt_current) * 3));
        return;
    case CURR_CH_ALL:
        get_volt_current_measure_result(CURR_CH_1, adc, data);
 //      get_volt_current_measure_result(CURR_CH_2, adc, (struct volt_current *)((uint8_t *)data + sizeof(struct volt_current)));
        return;
    case VOLT_CH:
        get_volt_current_measure_result(VOLT_CH_1, adc, data);
        get_volt_current_measure_result(VOLT_CH_2, adc, (struct volt_current *)((uint8_t *)data + sizeof(struct volt_current)));
        get_volt_current_measure_result(VOLT_CH_3, adc, (struct volt_current *)((uint8_t *)data + sizeof(struct volt_current) * 1));
 //     get_volt_current_measure_result(VOLT_CH_4, adc, (struct volt_current *)((uint8_t *)data + sizeof(struct volt_current) * 2));
        get_volt_current_measure_result(CURR_CH_1, adc, (struct volt_current *)((uint8_t *)data + sizeof(struct volt_current) * 3));
 //     get_volt_current_measure_result(CURR_CH_2, adc, (struct volt_current *)((uint8_t *)data + sizeof(struct volt_current) * 4));
 //     get_volt_current_measure_result(VOLT_CH_BAT, adc, (struct volt_current *)((uint8_t *)data + sizeof(struct volt_current) * 5));
        return;
    default:
        printf("- Voltage current measure ch error ch:%d\r\n", ch);
        return;
    }
    //--
    // temp = (float)data->adc_value * ADC_VSOLUTION;
    // data->volt_value = temp * data->proportion;
    // data->current_value = temp / data->bleeder_r * 1000;

     temp = (float)data->adc_value * ADC_VSOLUTION;
     data->volt_per += (temp * data->proportion);
     data->vol_num++;
     if(data->vol_num >= 10)
     {
        data->volt_value =  data->volt_per / (float)10;
        data->vol_num =0;
        data->volt_per=0;
     }
     data->current_value = temp / data->bleeder_r * 1000;

    //---
    #ifdef MONITOR_DUG
    printf_volt_current_collect_status(data);
   #endif
}
