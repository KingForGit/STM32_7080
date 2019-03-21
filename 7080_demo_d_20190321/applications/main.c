/**
 * @author: emlsyx
 * @email:  yangx_1118@163.com
 * @create: date 2018-11-22 14:32:22
 * @modify: date 2018-11-22 14:32:22
 * @desc:   [description]
*/
#include "lib_config.h"
#include "product.h"
#include "task_can.h"
#include "task_uart.h"
#include "task_monitor.h"
#include "task_keypad.h"

extern __task void task_can(void);
extern __task void task_monitor(void);
extern __task void task_uart(void);
extern __task void task_keypad(void);
struct product_s *lib_info = NULL; /* 生产环境变量*/
/**
 * @desc  : 初始化任务(值越大优先级越高)
 * @param : none
 * @return: none
 * @Date  : 2018-12-5 13:21:07
 */
__task void rtx_task_init(void)
{
#ifdef EN_OPS_SOFT_TIME
    tid_task_soft_time = os_tsk_create_user(task_soft_time, 6, &task_soft_time_heap, SOFT_TIME_HEAP_SIZE);
#endif
    tid_task_keypad = os_tsk_create_user(task_keypad, 4, &task_keypad_heap, KEYPAD_CMD_HEAP_SIZE);
    tid_task_uart = os_tsk_create_user(task_uart, 3, &task_uart_heap, UART_CMD_HEAP_SIZE);
    tid_task_can = os_tsk_create_user(task_can, 2, &task_can_heap, CAN_HEAP_SIZE);
    tid_task_monitor = os_tsk_create_user(task_monitor, 1, &task_monitor_heap, MONITOR_HEAP_SIZE);     
    /* get product info*/
    lib_info = get_soft_library_info();
    view_product_info(lib_info);
    /* delete app task init thread*/
    os_tsk_delete_self();
}
int main()
{
    extern void hw_board_init(void);            /* 板级初始化*/
    extern void switch_mode_default_init(void); /* 开关量采集初始化配置*/

    /* close interruption*/
    __set_FAULTMASK(1);

    /* board init*/
    hw_board_init();

    /* open interruption*/
    __set_FAULTMASK(0);

    /* 开关量采集初始化设置*/
    switch_mode_default_init();

    /* 读取应用层存储区域变量*/
    read_application_info();

    /* Initialize RTX and start init    */
    os_sys_init(rtx_task_init);
    while (1)
        ;
}
