
/**
 * @author: emlsyx
 * @email:  yangx_1118@163.com
 * @create: date 2018-11-30 17:28:56
 * @modify: date 2018-11-30 17:28:56
 * @desc:   [description]
*/
#include "dev_gpio.h"
#include "ops_io_output.h"

/**
 * @desc  : 普通io输出配置
 * @param : ch 通道号; mode 输出模式(IO_HIGH/IO_LOW)
 * @return: none
 * @Date  : 2018-11-30 17:28:38
 */
void set_switch_output(uint32_t ch, enum OUT_MODE mode)
{
    extern void gpio_set_mode(GPIO_TypeDef * port, uint16_t pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);
    if (ch <= 0)
        return;

    ch &= OUT_CH_ALL;
    if (ch & 0x01)
    {
//        gpio_set_mode(PORT_IO_OUTPUT_1, PIN_IO_OUTPUT_1, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
        io1_output(mode);
    }
    if ((ch & 0x02) >> 1)
    {
 //       gpio_set_mode(PORT_IO_OUTPUT_2, PIN_IO_OUTPUT_2, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
        io2_output(mode);
    }
    if ((ch & 0x04) >> 2)
    {
        io3_output(mode);
    }
    if ((ch & 0x08) >> 3)
    {
        io4_output(mode);
    }
/*按键直接寄存器控制输出了
    if ((ch & 0x10) >> 4)
    {
        io5_output(mode);
    }
    if ((ch & 0x20) >> 5)
    {
        io6_output(mode);
    }
    if ((ch & 0x40) >> 6)
    {
        io7_output(mode);
    }
    if ((ch & 0x80) >> 7)
    {
        io8_output(mode);
    }
*/
/*PWMDAC是定时器输出，不用这个
    if ((ch & 0x100) >> 8)
    {
        io9_output(mode);
    }
    if ((ch & 0x200) >> 9)
    {
        io10_output(mode);
    }
    if ((ch & 0x400) >> 9)
    {
        io11_output(mode);
    }
    if ((ch & 0x800) >> 9)
    {
        io12_output(mode);
    }
		*/
}
/*uint16_t time  时间ms*/
void beep_play(uint16_t time)
{
    static uint32_t Beeptick=0;
		if(time<50) return;
    if((get_hw_time_tick()-Beeptick) > time )
    {
       beep_flip();
			 Beeptick=get_hw_time_tick();
    }
    else
    {
       ;
    }
}
