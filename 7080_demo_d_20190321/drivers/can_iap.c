/**
 * @author: emlsyx
 * @email:  yangx_1118@163.com
 * @create: date 2018-11-09 19:21:17
 * @modify: date 2018-11-09 19:21:17
 * @desc:   [description]
*/
#include "task_can.h"
#include "can_iap.h"
#include "lib_config.h"
#include "product.h"
#include "dev_uart.h"
extern struct product_s *lib_info;
struct can_iap_info can_iap = {0};

struct can_filter_item iap_can_filter[2] =
{
    __CAN_Filter(0, ID_LIST, SCASE_32, CAN_IAP_REV_STOP, CAN_IAP_REV_DATA),
};
struct can_config can_iap_cfg =
{
    250000,         /* CAN 波特率*/
    1,              /* 使用过滤的个数*/
    iap_can_filter, /* 过滤器配置表*/
};

static timer_t can_iap_link = NULL;
static void can_iap_link_respond(void *parameter)
{
    uint8_t cnt = 0;
    uint32_t msg_num = 0;
    bool status = false;
    extern struct can_config can_cfg;
    extern OS_TID tid_task_monitor;
    extern void usart_disable();

    CanTxMsg can_msg =
    {
        0, 0x1fffffbd, CAN_ID_EXT, CAN_RTR_DATA, 8, {0x55, 0xab, 0xcd, 0, 0, 0, 0, 0xaa}
    };

    uart_close(USART1,uart1_recv);

    /* 分配接收内存*/
    if (can_iap.bin_buff == NULL)
    {
        can_iap.bin_buff = (uint8_t *)dev_malloc(sizeof(uint8_t) * CAN_IAP_BIN_BUFF_LEN);
        if (can_iap.bin_buff == NULL)
        {
            printf("- can iap memory malloc failed!\r\n");
        }
    }

    iwdg_feed();
    memset(can_iap.bin_buff, 0, CAN_IAP_BIN_BUFF_LEN);

    /* 擦除flash*/
    status = flash_area_erase(ADDR_APP_BAK, can_iap.parent.get_total_size);
    if (false != status)
    {
        printf("- Earse falsh error\r\n");
        NVIC_SystemReset();
    }

    /* 开启超时看门狗*/
    soft_time_start(can_iap._timout);
    can_iap.start_time = get_hw_time_tick();

    /* 关闭不相关中断,删除不相关任务*/
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, DISABLE);
    TIM_ITConfig(TIM8, TIM_IT_Update, DISABLE);
    os_tsk_delete(tid_task_monitor);

    /* 初始化升级信息*/
    can_iap.parent.save_wirite_index = ADDR_APP_BAK;
    can_iap.parent.rev_total_size = 0; /* 实际接收数据总大小*/
    can_iap.parent.rev_total_verify = 0;
    can_iap.parent.mode = true; /* can 处于升级模式*/

    /* 响应连接*/
    can_msg.Data[3] = lib_info->dev_id[CAN_VERIFY_OFFSET1];
    can_msg.Data[4] = lib_info->dev_id[CAN_VERIFY_OFFSET2];
    can_msg.Data[5] = lib_info->dev_id[CAN_VERIFY_OFFSET3];
    can_msg.Data[6] = lib_info->dev_id[CAN_VERIFY_OFFSET4];
    do
    {
        msg_num = can_write(&can_msg);
        if (msg_num == 0)
        {
            can_control(CAN_CMD_RESET, &can_cfg);
        }
        os_dly_wait(3);
        cnt++;
    }
    while ((msg_num == 0) && (cnt < 10));

    if (cnt < 10)
    {
        printf("- Can iap link success.\r\n");
        os_dly_wait(1);

        /* 重新配置can波特率*/
        can_control(CAN_CMD_RESET, &can_iap_cfg);
    }
    else
    {
        printf("- Can iap link failed.\r\n");
        NVIC_SystemReset();
    }
}
static void can_iap_result_respond()
{
    uint8_t cnt = 0;
    uint32_t msg_num = 0;

    CanTxMsg can_msg =
    {
        0, 0x1fffffbe, CAN_ID_EXT, CAN_RTR_DATA, 8, {0}
    };

    /* 回应设备编号*/
    can_msg.Data[0] = lib_info->dev_id[CAN_VERIFY_OFFSET1];
    can_msg.Data[1] = lib_info->dev_id[CAN_VERIFY_OFFSET2];
    can_msg.Data[2] = lib_info->dev_id[CAN_VERIFY_OFFSET3];
    can_msg.Data[3] = lib_info->dev_id[CAN_VERIFY_OFFSET4];

    do
    {
        msg_num = can_write(&can_msg);
        if (msg_num == 0)
        {
            /* 重新配置can波特率*/
            can_control(CAN_CMD_RESET, &can_iap_cfg);
        }
        os_dly_wait(3);
        cnt++;
    }
    while ((msg_num == 0) && (cnt < 10));

    if (cnt >= 10)
    {
        printf("- Can iap send dev id failed.\r\n");
    }
}

static void can_iap_write_data(void *parameter)
{
    bool status = false;
    iwdg_feed();
    // printf("- Can iap write can_iap_info.buff_offset %d \r\n", can_iap.buff_offset);
    // mem_printf(can_iap.bin_buff, can_iap.buff_offset, PRINT_HEX);
    // memset(can_iap.bin_buff, 0, can_iap.buff_offset);
    // can_iap.buff_offset = 0;
    // return;
    status = flash_area_write((uint32_t *)can_iap.parent.save_wirite_index, can_iap.bin_buff, can_iap.buff_offset);
    if (true != status)
    {
        can_iap.parent.save_wirite_index += can_iap.buff_offset;
        can_iap.parent.rev_total_size += can_iap.buff_offset;
        memset(can_iap.bin_buff, 0, can_iap.buff_offset);
        can_iap.buff_offset = 0;
        //printf("rev_total_size %d    get_total_size %d\r\n", can_iap.parent.rev_total_size, can_iap.parent.get_total_size);

        /* 写入的文件大小和can接收的文件大小一致,接收完成*/
        if (can_iap.parent.rev_total_size == can_iap.parent.get_total_size)
        {
            can_iap.parent.rev_total_verify = sum_16bit_verify((uint8_t *)ADDR_APP_BAK, can_iap.parent.rev_total_size);
            /* 校验正确*/
            if (can_iap.parent.rev_total_verify == can_iap.parent.get_total_verify)
            {
                can_iap_result_respond();
                flash_update_mark('Y');
                printf("- Can iap success. %.3fs\r\n", (float)(get_hw_time_tick() - can_iap.start_time) / 1000);
            }
            else
            {
                printf("- Can iap verify error. rev verify:%d  get verify:%d\r\n",
                       (int)can_iap.parent.rev_total_verify, (int)can_iap.parent.get_total_verify);
            }
            os_dly_wait(3);
            NVIC_SystemReset();
        }
        return;
    }
    printf("- Can iap write failed.\r\n");
    os_dly_wait(3);
    NVIC_SystemReset();
}
void can_iap_timeout(void *parameter)
{
    iwdg_feed();
    /* 超过1s 没有数据*/
    if ((get_hw_time_tick() - can_iap.parent.tick) > CAN_IAP_TIMEOUT * 1000)
    {
        NVIC_SystemReset();
    }
}
void can_iap_info_init(uint8_t *const msg)
{
    /* 生产模式中开始检测的命令字*/
    if ((msg[0] == 0x01) && (msg[1] == 0x55) && (msg[2] == 0xaa))
        return;

    /* 生产模式中的配置设备id命令字*/
    if (msg[0] == 0xf0)
        return;

    /* 生产模式中结束指令命令字*/
    if ((msg[0] == 0xef) && (msg[7] == 0xaa))
        return;

    /* 终检模式*/
    if ((msg[0] == 0xee) && (msg[6] == 0x55) && (msg[7] == 0xaa))
        return;

    /* 已经处于升级模式中*/
    if (can_iap.parent.mode != false)
        return;

    can_iap.v_bsp = msg[5];  /* 获取lib版本*/
    can_iap.v_user = msg[6]; /* 获取应用层版本*/

    if ((can_iap.v_bsp != BSP_VERSION) || (can_iap.v_user != APP_VERSION))
    {
        can_iap.parent.get_total_size = (uint32_t)(msg[0] | (msg[1] << 8) | (msg[2] << 16)); /* 获取文件大小*/

        if (can_iap.parent.get_total_size < APP_DATA_SIZE)
        {
            can_iap.parent.get_total_verify = (uint16_t)(msg[3] | (msg[4] << 8)); /* 获取校验和*/
            can_iap_link = soft_time_create("_can_link", can_iap_link_respond, NULL, 1, SOFT_TIMER_TYPE_ONE_SHOT);
            soft_time_start(can_iap_link);
            can_iap._timout = soft_time_create("_can_timout", can_iap_timeout, NULL, 100, SOFT_TIMER_TYPE_PERIODIC);
            can_iap._w_data_time = soft_time_create("_w_data", can_iap_write_data, NULL, 1, SOFT_TIMER_TYPE_ONE_SHOT);
        }
    }
    else
    {
        can_iap.parent.mode = false;
        printf("- device ver haigh.\r\n");
    }
}
void can_iap_msg_handle(uint32_t id, uint8_t *msg, uint8_t dlc)
{
    /* 更新tick*/
    can_iap.parent.tick = get_hw_time_tick();

    if (id == CAN_IAP_REV_STOP)
    {
        soft_time_start(can_iap._w_data_time); /* 接收到停止帧指令，写入数据*/
    }
    else
    {
        if (can_iap.buff_offset < CAN_IAP_BIN_BUFF_LEN)
        {
            memcpy(can_iap.bin_buff + can_iap.buff_offset, msg, dlc);
            can_iap.buff_offset += dlc;
        }
    }
}
