/**
 * @author: emlsyx
 * @email:  yangx_1118@163.com
 * @create: date 2018-11-23 18:51:50
 * @modify: date 2018-11-23 18:51:50
 * @desc:   [description]
*/
#include "ops_resistan.h"

#define DEBUG_RES_COLLECT
#ifdef DEBUG_RES_COLLECT
    #define res_collect_dbg(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define res_collect_dbg(fmt, ...)
#endif

/**
 * @desc  : 根据电压计算并联电阻值
 * @param : vref_volt 参考电压(单位mv);collect_volt 采集电压
 * @return: 成功:并联电阻值(单位Ω)
 * @Date  : 2018-11-26 16:36:34
 */
__inline float count_general_res(float vref_volt, float collect_volt, float matched_r)
{
    //float value = 0;
    //value = vref_volt - collect_volt; /* 已知 支路的电压*/
		if(vref_volt - collect_volt!=0)
		{
				return (float)(matched_r * collect_volt) / (vref_volt - collect_volt);
		}
//		else
//		{
//				return  RES_GRAE_CRITICAL_R;
//		}
}
/**
 * @desc  : 根据总路电阻值计算出并联电路中支路电阻值
 * @param : total_res 总路电阻; known_res 已知的电阻值
 * @return: 并联支路中未知的电阻值单位k
 * @Date  : 2018-11-24 14:12:31
 */
__inline float count_branch_res(float total_res, float branch_res)
{
    /* 并联电路中总路电阻小于支路电阻值*/
    //assert_param(total_res < branch_res);

    /* 1/R = 1/R1 + 1/R2*/
    return (branch_res * total_res) / (branch_res - total_res);
}
static void printf_res_collect_status(struct res_status *data)
{
    res_collect_dbg("-  NO.%02d   ", data->ch_num + 1);
    if (data->status == false)
        res_collect_dbg("Error     ");
    else
        res_collect_dbg("  Ok      ");

    if (data->grae == false)
        res_collect_dbg("High    ");
    else
        res_collect_dbg("Low     ");
    res_collect_dbg("%010.4f      ", data->branch_v);
    res_collect_dbg("%010.4f     ", data->branch_r);
    res_collect_dbg("%010.4f     ", data->geae_min_r);
    res_collect_dbg("%010.4f     ", data->geae_max_r);
    res_collect_dbg("%010.4f\r\n", data->res_value);
}

void set_res_gears(enum RES_CH ch, struct res_status *data, enum RES_GEARS gears)
{
    switch (ch)
    {
    case RES_CH_1:
        if (gears == HIGH_GEARS)
        {
            open_ch1_high_res_mesure();
        }
        else
        {
            open_ch1_low_res_mesure();
        }
        break;
    case RES_CH_2:
        if (gears == HIGH_GEARS)
        {
            open_ch2_high_res_mesure();
        }
        else
        {
            open_ch2_low_res_mesure();
        }
        break;
 /*
    case RES_CH_3:
        if (gears == HIGH_GEARS)
        {
           // open_ch3_high_res_mesure();
        }
        else
        {
           // open_ch3_low_res_mesure();
        }
        break;
    case RES_CH_4:
        if (gears == HIGH_GEARS)
        {
          //  open_ch4_high_res_mesure();
        }
        else
        {
          //  open_ch4_low_res_mesure();
        }
        break;
 */
    case RES_CH_ALL:
        set_res_gears(RES_CH_1, data, gears);
        set_res_gears(RES_CH_2, (struct res_status *)((uint8_t *)data + sizeof(struct res_status)), gears);
/*     
        set_res_gears(RES_CH_3, (struct res_status *)((uint8_t *)data + sizeof(struct res_status) * 2), gears);
        set_res_gears(RES_CH_4, (struct res_status *)((uint8_t *)data + sizeof(struct res_status) * 3), gears);
*/
        return;
    default:
        break;
    }

    if (gears == HIGH_GEARS)
    {
        data->grae = false;
        data->geae_max_r = count_branch_res(RES_COLLECT_MAX, RES_PARALLEL);
        data->geae_min_r = RES_MAX_GRAE_THRESHOLD;
    }
    else
    {
        data->grae = true;
        data->geae_min_r = count_branch_res(RES_COLLECT_MIN, RES_PARALLEL);
        data->geae_max_r = RES_MIN_GRAE_THRESHOLD;
    }
}
void get_res_measure_result(enum RES_CH ch, struct adc_result *adc, struct res_status *data)
{
	int AbsResult=0;
    if (false == adc->status)
    {
        return;
    }
    data->geae_min_r = (data->geae_min_r <= 0) ? RES_MIN_GRAE_THRESHOLD : data->geae_min_r;
    data->geae_max_r = (data->geae_max_r <= 0) ? count_branch_res(RES_COLLECT_MAX, RES_PARALLEL) : data->geae_max_r;
    switch (ch)
    {
    case RES_CH_1:
        data->ch_num = RES_CH_1;
        data->branch_v = (float)(adc->resistance_1 * ADC_RESOLUTION) / 11 * 1000; /* 原始电压*/
        if (data->grae == true)
        {
            data->branch_r = count_general_res(RES_VREF_V, data->branch_v, RES_LOW_SAMPLI);
					
            if (data->branch_r > RES_MAX_GRAE_THRESHOLD|| adc->resistance_1==4095 )
            {
                data->grae = false;
                data->geae_max_r = count_branch_res(RES_COLLECT_MAX, RES_PARALLEL);
                data->geae_min_r = RES_MAX_GRAE_THRESHOLD;
                open_ch1_high_res_mesure();
             //   break;
            }
        }
        else if (data->grae == false)
        {
            data->branch_r = count_general_res(RES_VREF_V, data->branch_v, RES_HIGH_SAMPLI);
            if (data->branch_r < RES_MIN_GRAE_THRESHOLD)
            {
                data->grae = true;
                data->geae_min_r = count_branch_res(RES_COLLECT_MIN, RES_PARALLEL);
                data->geae_max_r = RES_MIN_GRAE_THRESHOLD;
                open_ch1_low_res_mesure();
             //   break;
            }

				}
        break;
    case RES_CH_2:
        data->ch_num = RES_CH_2;
        data->branch_v = (float)(adc->resistance_2 * ADC_RESOLUTION) / 11 * 1000; /* 原始电压*/
        if (data->grae == true)
        {
            data->branch_r = count_general_res(RES_VREF_V, data->branch_v, RES_LOW_SAMPLI);
            if (data->branch_r > RES_MAX_GRAE_THRESHOLD)
            {
                data->grae = false;
                data->geae_max_r = count_branch_res(RES_COLLECT_MAX, RES_PARALLEL);
                data->geae_min_r = RES_MAX_GRAE_THRESHOLD;
                open_ch2_high_res_mesure();
                break;
            }
        }
        else if (data->grae == false)
        {
            data->branch_r = count_general_res(RES_VREF_V, data->branch_v, RES_HIGH_SAMPLI);
            if (data->branch_r < RES_MIN_GRAE_THRESHOLD)
            {
                data->grae = true;
                data->geae_min_r = count_branch_res(RES_COLLECT_MIN, RES_PARALLEL);
                data->geae_max_r = RES_MIN_GRAE_THRESHOLD;
                open_ch2_low_res_mesure();
                break;
            }
        }
        break;
				/*
    case RES_CH_3:
        data->ch_num = RES_CH_3;
        data->branch_v = (float)(adc->resistance_3 * ADC_RESOLUTION) / 11 * 1000; 
        if (data->grae == true)
        {
            data->branch_r = count_general_res(RES_VREF_V, data->branch_v, RES_LOW_SAMPLI);
            if (data->branch_r > RES_MAX_GRAE_THRESHOLD)
            {
                data->grae = false;
                data->geae_max_r = count_branch_res(RES_COLLECT_MAX, RES_PARALLEL);
                data->geae_min_r = RES_MAX_GRAE_THRESHOLD;
            //    open_ch3_high_res_mesure();
                break;
            }
        }
        else if (data->grae == false)
        {
            data->branch_r = count_general_res(RES_VREF_V, data->branch_v, RES_HIGH_SAMPLI);
            if (data->branch_r < RES_MIN_GRAE_THRESHOLD)
            {
                data->grae = true;
                data->geae_min_r = count_branch_res(RES_COLLECT_MIN, RES_PARALLEL);
                data->geae_max_r = RES_MIN_GRAE_THRESHOLD;
          //      open_ch3_low_res_mesure();
                break;
            }
        }
        break;
				
    case RES_CH_4:
        data->ch_num = RES_CH_4;
        data->branch_v = (float)(adc->resistance_4 * ADC_RESOLUTION) / 11 * 1000; 
        if (data->grae == true)
        {
            data->branch_r = count_general_res(RES_VREF_V, data->branch_v, RES_LOW_SAMPLI);
            if (data->branch_r > RES_MAX_GRAE_THRESHOLD)
            {
                data->grae = false;
                data->geae_max_r = count_branch_res(RES_COLLECT_MAX, RES_PARALLEL);
                data->geae_min_r = RES_MAX_GRAE_THRESHOLD;
          //      open_ch4_high_res_mesure();
                break;
            }
        }
        else if (data->grae == false)
        {
            data->branch_r = count_general_res(RES_VREF_V, data->branch_v, RES_HIGH_SAMPLI);
            if (data->branch_r < RES_MIN_GRAE_THRESHOLD)
            {
                data->grae = true;
                data->geae_min_r = count_branch_res(RES_COLLECT_MIN, RES_PARALLEL);
                data->geae_max_r = RES_MIN_GRAE_THRESHOLD;
          //      open_ch4_low_res_mesure();
                break;
            }
        }
        break;
				*/
    case RES_CH_ALL:
        get_res_measure_result(RES_CH_1, adc, data);
        get_res_measure_result(RES_CH_2, adc, (struct res_status *)((uint8_t *)data + sizeof(struct res_status)));
//        get_res_measure_result(RES_CH_3, adc, (struct res_status *)((uint8_t *)data + sizeof(struct res_status) * 2));
//        get_res_measure_result(RES_CH_4, adc, (struct res_status *)((uint8_t *)data + sizeof(struct res_status) * 3));
        return;
    default:
        break;
    }

        // if(adc->resistance_1> 4090)
        // {
        //     data->grae = false;
        //     data->res_value =  100000; 
        // }
        // else if(adc->resistance_1<15)
        // {
        //     data->grae = false;
        //     data->res_value =  2;            /* code */
        // }
        // else

              data->res_value = count_branch_res(data->branch_r, RES_PARALLEL);

    /* 量程溢出判断*/
    
  /*  if (((data->branch_r < RES_COLLECT_MIN)) || (data->branch_r > RES_COLLECT_MAX))
    {
        data->status = false;
        if (data->grae == true)
        {
            res_collect_dbg("- NO.%d Collect Low error. branch_r：%f, branch volt %f mv\r\n\r\n", data->ch_num, data->branch_r, data->branch_v); // 低于量程
        }
        else
        {
            res_collect_dbg("- NO.%d Collect High error. branch_r：%f, branch volt %f mv\r\n\r\n", data->ch_num, data->branch_r, data->branch_v); // 高于量程
        }

        data->branch_r = 0;
        data->res_value = 0;
    }*/
   // else
    {
        data->status = true;
#ifdef MONITOR_DUG
        printf_res_collect_status(data);
#endif
    }
}
