#include "ops_io_input.h"
#include "dev_dac.h"

#ifdef DEBUG_SW_COLLECT
    #define sw_collect_dbg(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define sw_collect_dbg(fmt, ...)
#endif
/**
 * @desc  : 配置开关量比较电压
 * @param : ch 配置通道号; volt 比较的电压(单位 volt:0~3300  表示0-3.3v)
 * @return: none
 * @Date  : 2018-11-28 14:50:45
 */
void conf_switch_compare_volt(enum SW_CH ch, uint16_t volt)
{
    switch (ch)
    {
    case SW_CH_10:
    case SW_CH_11:
			  dac_set_output(DAC_Channel_1, volt);
        break;
    case SW_CH_12:
    case SW_CH_13:
        dac_set_output(DAC_Channel_2, volt);
        break;
	case SW_CH_ALL:
				dac_set_output(DAC_Channel_1, volt);
				dac_set_output(DAC_Channel_2, volt);
        break;		
    default:
		break;
    }
}
/**
 * @desc  : 配置开关量采集模式
 * @param : ch 通道号;
 *          mode 配置模式(LEVITATE: 浮空; PULL_UP：上拉; PULL_DOWN: 下拉);ch1~ch4支持上/下拉；ch5~ch8支持上拉 其余通道不支持
 *          开关量采集结构指针(更新采集模式)
 * @return: none
 * @Date  : 2018-11-28 14:51:43
 */
void conf_switch_mode(enum SW_CH ch, enum sw_mode mode, struct switch_status *data)
{
    data->sw_mode = mode;
    switch (ch)
    {
			//DL1-DL6
    case SW_CH_14:
//        close_freq_collect(FREQ_CH_1);
        sw1_ctrl_up(data->sw_mode);
        break;
    case SW_CH_15:
//        close_freq_collect(FREQ_CH_2);
         sw2_ctrl_up(data->sw_mode);
        break;
    case SW_CH_16:
//       close_freq_collect(FREQ_CH_3);
         sw3_ctrl_up(data->sw_mode);
        break;
    case SW_CH_17:
//        close_freq_collect(FREQ_CH_4);
        sw4_ctrl_up(data->sw_mode);
        break;
    case SW_CH_18:
        sw5_ctrl_up(data->sw_mode);
        break;
    case SW_CH_19:
        sw6_ctrl_up(data->sw_mode);
        break;
/******4路DH_PI，配置SW_CH_7----SW_CH_10*******/		
    case SW_CH_10:
			  close_freq_collect(FREQ_CH_1);
			//	sw1_ctrl_down(data->sw_mode);
        break;		
    case SW_CH_11:
			  close_freq_collect(FREQ_CH_1);
			//	sw2_ctrl_down(data->sw_mode);
        break;
		case SW_CH_12:
			  close_freq_collect(FREQ_CH_1);
			//	sw3_ctrl_down(data->sw_mode);
        break;
		case SW_CH_13:
			  close_freq_collect(FREQ_CH_1);
			//	sw4_ctrl_down(data->sw_mode);
				break;
    case SW_CH_ALL:
			  conf_switch_mode(SW_CH_10, mode, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) ));
        conf_switch_mode(SW_CH_11, mode, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 1));
        conf_switch_mode(SW_CH_12, mode, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 2));
        conf_switch_mode(SW_CH_13, mode, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 3));
        conf_switch_mode(SW_CH_13, mode, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 4));
        conf_switch_mode(SW_CH_15, mode, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 5));
        conf_switch_mode(SW_CH_16, mode, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 6));
        conf_switch_mode(SW_CH_17, mode, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 7));
        conf_switch_mode(SW_CH_18, mode, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 8));
        conf_switch_mode(SW_CH_19, mode, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 9));
		
        return;
    default:
        break;
    }
}

static void printf_sw_collect_status(struct switch_status *data)
{
    sw_collect_dbg("-    No %02d    ", data->ch_num);

    if (data->sw_mode == PULL_UP)
    {
        sw_collect_dbg("Pull Up       ");
    }
    else if (data->sw_mode == PULL_DOWN)
    {
        sw_collect_dbg("Pull Down     ");
    }
    else
    {
        sw_collect_dbg("Floating      ");
    }
    sw_collect_dbg("%04.2fv         ", (float)data->ref_volt * ADC_RESOLUTION);

    if (data->sw_status == 1)
    {
        sw_collect_dbg("High\r\n");
    }
    else
    {
        sw_collect_dbg("Low\r\n");
    }
}
/**
 * @desc  : 获取开关量状态
 * @param : ch 指定通道; data 当前状态
 * @return: none
 * @Date  : 2018-11-24 20:18:53
 */
void get_switch_status(enum SW_CH ch, struct switch_status *data)
{
    switch (ch)
    {
    case SW_CH_1:
        data->ch_num = SW_CH_1;
        data->reuse = 1;
        data->down_res = 1;
        data->sw_status = sw1_gpio_state();
        data->ref_volt = pmw_dac1_state();
        break;
    case SW_CH_2:
        data->ch_num = SW_CH_2;
        data->reuse = 1;
        data->down_res = 1;
        data->sw_status = sw2_gpio_state();
        data->ref_volt = pmw_dac1_state();
        break;
    case SW_CH_3:
        data->ch_num = SW_CH_3;
        data->reuse = 1;
        data->down_res = 1;
        data->sw_status = sw3_gpio_state();
        data->ref_volt = pmw_dac1_state();
        break;
    case SW_CH_4:
        data->ch_num = SW_CH_4;
        data->reuse = 1;
        data->down_res = 1;
        data->sw_status = sw4_gpio_state();
        data->ref_volt = pmw_dac1_state();
    //    data->ref_volt = dac_get_value(DAC_Channel_1);
        break;
    case SW_CH_5:
        data->ch_num = SW_CH_5;
        data->reuse = 1;
        data->sw_status = sw5_gpio_state();
        data->ref_volt = dac_get_value(DAC_Channel_2);
        break;
    case SW_CH_6:
        data->ch_num = SW_CH_6;
        data->reuse = 1;
        data->sw_status = sw6_gpio_state();
        data->ref_volt = dac_get_value(DAC_Channel_2);
        break;
    case SW_CH_7:
        data->ch_num = SW_CH_7;
        data->reuse = 1;
        data->sw_status = sw7_gpio_state();
        data->ref_volt = dac_get_value(DAC_Channel_2);
        break;
    case SW_CH_8:
        data->ch_num = SW_CH_8;
        data->reuse = 1;
        data->sw_status = sw8_gpio_state();
        data->ref_volt = dac_get_value(DAC_Channel_2);
        break;
    case SW_CH_9:
        data->ch_num = SW_CH_9;
        data->sw_status = sw9_gpio_state();
        data->ref_volt = dac_get_value(DAC_Channel_2);
        break;
    case SW_CH_10:
        data->ch_num = SW_CH_10;
        data->sw_status = sw10_gpio_state();
        data->ref_volt = dac_get_value(DAC_Channel_2);
        break;
    case SW_CH_11:
        data->ch_num = SW_CH_11;
        data->sw_status = sw11_gpio_state();
        data->ref_volt = dac_get_value(DAC_Channel_2);
        break;
    case SW_CH_12:
        data->ch_num = SW_CH_12;
        data->sw_status = sw12_gpio_state();
        data->ref_volt = dac_get_value(DAC_Channel_2);
        break;
    case SW_CH_13:
        data->ch_num = SW_CH_13;
        data->sw_status = sw13_gpio_state();
        data->ref_volt = dac_get_value(DAC_Channel_2);
        break;
    case SW_CH_14:
        data->ch_num = SW_CH_14;
        data->sw_status = sw14_gpio_state();
        data->ref_volt = dac_get_value(DAC_Channel_2);
        break;
    case SW_CH_15:
        data->ch_num = SW_CH_15;
        data->sw_status = sw15_gpio_state();
        data->ref_volt = dac_get_value(DAC_Channel_2);
        break;
    case SW_CH_16:
        data->ch_num = SW_CH_16;
        data->sw_status = sw16_gpio_state();
        data->ref_volt = dac_get_value(DAC_Channel_2);
        break;
     case SW_CH_17:
        data->ch_num = SW_CH_17;
        data->sw_status = sw17_gpio_state();
        data->ref_volt = dac_get_value(DAC_Channel_2);
        break;
    case SW_CH_18:
        data->ch_num = SW_CH_18;
        data->sw_status = sw18_gpio_state();
        data->ref_volt = dac_get_value(DAC_Channel_2);
        break;
    case SW_CH_19:
        data->ch_num = SW_CH_19;
        data->sw_status = sw19_gpio_state();
        data->ref_volt = dac_get_value(DAC_Channel_2);
        break;       
    case SW_CH_ALL:
        get_switch_status(SW_CH_1, data);
        get_switch_status(SW_CH_2, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status)));
        get_switch_status(SW_CH_3, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 2));
        get_switch_status(SW_CH_4, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 3));
        get_switch_status(SW_CH_5, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 4));
        get_switch_status(SW_CH_6, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 5));
        get_switch_status(SW_CH_7, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 6));
        get_switch_status(SW_CH_8, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 7));
        get_switch_status(SW_CH_9, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 8));
        get_switch_status(SW_CH_10, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 9));
        get_switch_status(SW_CH_11, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 10));
        get_switch_status(SW_CH_12, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 11));
        get_switch_status(SW_CH_13, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 12));
        get_switch_status(SW_CH_14, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 13));
        get_switch_status(SW_CH_15, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 14));
        get_switch_status(SW_CH_16, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 15));
        get_switch_status(SW_CH_17, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 16));
        get_switch_status(SW_CH_18, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 17));
        get_switch_status(SW_CH_19, (struct switch_status *)((uint8_t *)data + sizeof(struct switch_status) * 18));

        return;
    default:
        break;
    }
    #ifdef MONITOR_DUG
     printf_sw_collect_status(data);
   #endif
}
