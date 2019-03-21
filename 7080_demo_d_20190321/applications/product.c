#include "product.h"
#include "task_can.h"
#include "can_iap.h"

extern int32_t pro_storage_write(uint8_t *buff);

static struct adc_result pro_adc_value = {0};

timer_t can_send_pro = NULL; /* 通过 can发送存储的信息*/
static timer_t product_start = NULL;
static timer_t product_over = NULL;
static timer_t product_dev_id = NULL;

static uint8_t product_buff[PRO_DATA_SIZE] = {0};
static struct product_s *const product = (struct product_s *)product_buff; /* 生产环境变量*/

static volatile uint32_t pro_verify_timeout = 0;

struct _send_status
{
    bool flag;
    uint32_t tick;
    uint8_t ch;
    uint8_t gears;
};
struct _get_pro_conf
{
    uint8_t flag; /* 1:获取设备编号; 2: 获取硬件版本; 3: 获取软件版本; 4: 获取检验时间;*/
    struct product_s *data;
};
static struct _get_pro_conf can_get_conf = {0, NULL};

void view_product_info(struct product_s *data)
{
    printf("- Pro key_word: %x\r\n", data->key_word);
    printf("- Pro dev id: %s\r\n", data->dev_id);
    printf("- Pro over time: %04d-%02d-%02d %02d:%02d:%02d\r\n", data->over_time[0] + 2000, data->over_time[1],
           data->over_time[2], data->over_time[3], data->over_time[4], data->over_time[5]);
    printf("- Pro hard version: %s\r\n", data->hard_version);
    printf("- Pro soft version: %s\r\n", data->soft_version);
}

static void can_send_pro_info(void *parameter)
{
    uint8_t i;
    static uint8_t index = 0, len = 0;
    static uint8_t *p = NULL;
    struct _get_pro_conf *conf = NULL;
    uint8_t msg[8] = {0};

    conf = (struct _get_pro_conf *)parameter;

    memset(msg, 0xff, 8);
    switch (conf->flag)
    {
    case 1:
        p = (uint8_t *) & (conf->data->dev_id[0]);
        len = strlen((char *)(conf->data->dev_id));
        break;
    case 2:
        p = (uint8_t *) & (conf->data->hard_version[0]);
        len = strlen((char *)(conf->data->hard_version));
        break;
    case 3:
        p = (uint8_t *) & (conf->data->soft_version[0]);
        len = strlen((char *)(conf->data->soft_version));
        break;
    case 4:
        p = (uint8_t *) & (conf->data->over_time[0]);
        len = 6;
        break;
    case 254:
        conf->flag = 1;
        can_send_pro_info(conf);
        conf->flag = 2;
        can_send_pro_info(conf);
        conf->flag = 3;
        can_send_pro_info(conf);
        conf->flag = 4;
        can_send_pro_info(conf);
        return;
    default:
        return;
    }

    while (1)
    {
        msg[0] = conf->flag;
        for (i = 2; i < 8; i++)
        {
            if (index < len)
            {
                msg[i] = ((uint8_t *)p)[index];
                index++;
            }
            else
            {
                break;
            }
        }

        msg[1] = i - 2;
        if (i > 2)
        {
            can_send_data(PRO_OVER_CAN_ID, msg);
            memset(msg, 0xff, 8);
        }
        else
        {
            break;
        }
    }

    len = 0;
    index = 0;
    p = NULL;
    soft_time_stop(can_send_pro);
}

static void send_ch_num(uint32_t id, uint8_t ch)
{
    uint8_t msg[8] = {0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa};
    msg[0] = ch;
    can_send_data(id, msg);
}

static void send_mcu_id()
{
    static uint8_t index = 0;
    extern uint32_t stm32_mcu_id[3];
    uint8_t msg[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    if (index > 0)
    {
        msg[1] = (uint8_t)((stm32_mcu_id[0] & 0xff000000) >> 24);
        msg[2] = (uint8_t)((stm32_mcu_id[0] & 0xff0000) >> 16);
        msg[3] = (uint8_t)((stm32_mcu_id[0] & 0xff00) >> 8);
        msg[4] = (uint8_t)(stm32_mcu_id[0] & 0xff);
        msg[5] = (uint8_t)((stm32_mcu_id[1] & 0xff000000) >> 24);
        msg[6] = (uint8_t)((stm32_mcu_id[1] & 0xff0000) >> 16);
        msg[7] = (uint8_t)((stm32_mcu_id[1] & 0xff00) >> 8);
        index = 0;
    }
    else
    {
        msg[1] = (uint8_t)(stm32_mcu_id[1] & 0xff);
        msg[2] = (uint8_t)((stm32_mcu_id[2] & 0xff000000) >> 24);
        msg[3] = (uint8_t)((stm32_mcu_id[2] & 0xff0000) >> 16);
        msg[4] = (uint8_t)((stm32_mcu_id[2] & 0xff00) >> 8);
        msg[5] = (uint8_t)(stm32_mcu_id[2] & 0xff);
        index = 7;
    }
    msg[0] = index;
    can_send_data(MCU_ID_CAN_ID, msg);
}

static void send_dev_id(void *parameter)
{
    uint8_t msg[8] = {0xef, '\0', '\0', '\0', '\0', '\0', '\0', '\0'};

    uint8_t i = 0;
    static uint8_t *p = NULL;
    static uint8_t index = 0;
    p = (uint8_t *)parameter;

    //printf("dev_id: %s\r\n",p);
    msg[1] = index;
    for (i = 2; i < 8; i++)
    {
        msg[i] = p[index];
        if (index++ >= 15)
        {
            index = 0;
            //soft_time_stop(product_dev_id);
            break;
        }
    }

    can_send_data(DEV_ID_CAN_ID, msg);
}

#pragma push
#pragma diag_suppress 188
/**
 * @desc  : 发送4路电阻状态
 * @param : data 发送的数据,index 本次发送的个数
 * @return: none
 * @Date  : 2018-12-8 17:00:52
 */
static struct res_status pro_r_value[4] = {0};
static void send_res_value(uint8_t ch, uint8_t gears)
{
    uint32_t temp = 0;
    uint8_t send_msg[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    get_res_measure_result(ch, &pro_adc_value, &pro_r_value[ch]);

    temp = (uint32_t)(pro_r_value[ch].res_value * 100);
    send_msg[0] = (uint8_t)ch;            /* 当前通道号*/
    send_msg[1] = (uint8_t)gears;         /* 当前档位状态*/
    send_msg[2] = (uint8_t)(temp & 0xff); /* 电阻值*/
    send_msg[3] = (uint8_t)((temp & 0xff00) >> 8);
    send_msg[4] = (uint8_t)((temp & 0xff0000) >> 16);
    send_msg[5] = (uint8_t)((temp & 0xff000000) >> 24);

    can_send_data(RES_SEND_CAN_ID, send_msg);
}
/**
 * @desc  : 发送4路电压状态
 * @param : data 发送的数据,index 本次发送的个数
 * @return: none
 * @Date  : 2018-12-8 18:50:14
 */
static void send_volt_value(struct volt_current *data, uint8_t *index)
{
    uint8_t msg[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    struct volt_current *p;

    p = data;
    msg[0] = (uint8_t)p->ch_num - 1;          /* 当前通道号*/
    msg[1] = (uint8_t)((p->volt_value) * 10); /* 电压采集状态 扩大十倍*/

    p = (struct volt_current *)((uint8_t *)data + sizeof(struct volt_current));
    msg[2] = (uint8_t)p->ch_num - 1;          /* 当前通道号*/
    msg[3] = (uint8_t)((p->volt_value) * 10); /* 电压采集状态 扩大十倍*/

    p = (struct volt_current *)((uint8_t *)data + sizeof(struct volt_current) * 2);
    msg[4] = (uint8_t)p->ch_num - 1;          /* 当前通道号*/
    msg[5] = (uint8_t)((p->volt_value) * 10); /* 电压采集状态 扩大十倍*/

    p = (struct volt_current *)((uint8_t *)data + sizeof(struct volt_current) * 3);
    msg[6] = (uint8_t)p->ch_num - 1;          /* 当前通道号*/
    msg[7] = (uint8_t)((p->volt_value) * 10); /* 电压采集状态 扩大十倍*/

    index += 4;
    can_send_data(VOLT_SEND_CAN_ID, msg);
}
/**
 * @desc  : 发送2路电流状态
 * @param : data 发送的数据,index 本次发送的个数
 * @return: none
 * @Date  : 2018-12-8 17:00:52
 */
static void send_curr_value(struct volt_current *data, uint8_t *index)
{
    uint8_t msg[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint16_t temp = 0;

    struct volt_current *p;

    p = data;
    temp = (uint16_t)(p->current_value * 100);
    msg[0] = (uint8_t)(p->ch_num) - 1; /* 当前通道号*/
    msg[1] = (uint8_t)(temp & 0xff);   /* 电流采集状态 扩大一百倍*/
    msg[2] = (uint8_t)((temp & 0xff00) >> 8);

    p = (struct volt_current *)((uint8_t *)data + sizeof(struct volt_current));
    temp = (uint16_t)(p->current_value * 100);
    msg[3] = (uint8_t)(p->ch_num) - 1; /* 当前通道号*/
    msg[4] = (uint8_t)(temp & 0xff);   /* 电流采集状态 扩大一百倍*/
    msg[5] = (uint8_t)((temp & 0xff00) >> 8);

    index += 2;
    can_send_data(CURR_SEND_CAN_ID, msg);
}

/**
 * @desc  : 发送16路开关量状态
 * @param : data 发送的数据,index 本次发送的个数
 * @return: none
 * @Date  : 2018-12-8 17:00:52
 */
static struct switch_status pro_s_value[16] = {0};
static void send_sw_value(uint8_t ch)
{
    struct switch_status *value = NULL;
    uint8_t msg[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0x55, 0xaa, 0xff};

    value = &pro_s_value[ch - 1];
    get_switch_status(ch, value);
    msg[0] = (uint8_t)ch - 1; /* 当前通道号*/
    msg[1] = (uint8_t)value->reuse;
    msg[2] = (uint8_t)value->sw_mode;
    msg[3] = (uint8_t)value->sw_status;
    msg[4] = (uint8_t)value->down_res;
    can_send_data(SW_SEND_CAN_ID, msg);
}

static void pro_control_analysis(struct my_can_recv *data)
{
    volatile uint32_t get_tick = 0;
    static uint32_t pro_adc_get_tick=0;
    struct my_can_recv *p = NULL;
    static struct _send_status resis = {0};
    static struct _send_status sw_input = {0};

    get_tick = get_hw_time_tick();

    /* adc 200ms滤波一次*/
    if((get_tick - pro_adc_get_tick)>=200)
    {
        pro_adc_get_tick = get_tick;
        adc_read_value(&pro_adc_value);
    }

    /* 接收控制开关量采集模式id*/
    p = data;
    if (p->id == CAN_PRO_REV2_ID)
    {
        if ((p->data[4] == 0xaa) && (p->data[5] == 0x55) && (p->data[6] == 0xaa) && (p->data[7] == 0xff))
        {
            sw_input.flag = true;
            sw_input.tick = get_tick;
            sw_input.ch = p->data[0] + 1;
            if (p->data[1] == 1)
            {
                conf_switch_mode(sw_input.ch, PULL_UP, &pro_s_value[p->data[0]]);
            }
            else if (p->data[1] == 2)
            {
                conf_switch_mode(sw_input.ch, PULL_DOWN, &pro_s_value[p->data[0]]);
            }
            else
            {

                conf_switch_mode(sw_input.ch, LEVITATE, &pro_s_value[p->data[0]]);
            }
            p->id = 0;
            memset(&(p->data[0]), 0, 8);
        }
    }

    p = (struct my_can_recv *)((uint8_t *)data + sizeof(struct my_can_recv));
    if (p->id == CAN_PRO_REV3_ID) /* 接收电阻控制*/
    {
        if ((p->data[4] == 0xaa) && (p->data[5] == 0x55) && (p->data[6] == 0xaa) && (p->data[7] == 0xff))
        {
            resis.flag = true;
            resis.tick = get_tick;
            resis.ch = p->data[0];
            resis.gears = p->data[1];

            if (resis.gears == 3)
                set_res_gears(resis.ch, &pro_r_value[resis.ch], HIGH_GEARS);
            else
                set_res_gears(resis.ch, &pro_r_value[resis.ch], LOW_GEARS);

            p->id = 0;
            memset(&(p->data[0]), 0, 8);
        }
    }

    p = (struct my_can_recv *)((uint8_t *)data + sizeof(struct my_can_recv) * 2);
    if (p->id == CAN_PRO_REV4_ID) /* 接收开关量输出高低控制id*/
    {
        if ((p->data[4] == 0xaa) && (p->data[5] == 0x55) && (p->data[6] == 0xaa) && (p->data[7] == 0xff))
        {
            if (p->data[1] == 1)
            {
                if (p->data[2] == 1)
                {
                    set_switch_output(OUT_CH_ALL, IO_HIGH);
                }
                else if (p->data[2] == 2)
                {
                    set_switch_output(OUT_CH_ALL, IO_LOW);
                }
            }
        }
    }

    if (((get_tick - resis.tick) >= 500) && (resis.flag == true))
    {
        send_res_value(resis.ch, resis.gears);
        resis.ch = 0;
        resis.gears = 0;
        resis.flag = false;
    }

    if (((get_tick - sw_input.tick) >= 100) && (sw_input.flag == true))
    {
        send_sw_value(sw_input.ch);
        sw_input.ch = 0;
        sw_input.flag = false;
    }
}
#pragma pop

static void pro_verift_handle(void *parameter)
{
    static uint8_t uid_index = 0;
    static uint8_t volt_index = 0;
    static uint8_t curr_index = 0;
    struct volt_current v_value[4] = {0};
    struct volt_current c_value[2] = {0};

    pro_control_analysis((struct my_can_recv *)parameter);

    if (volt_index >= 4)
    {
        send_ch_num(VOLT_SEND_CAN_ID, volt_index);
        volt_index = 0;
    }
    else
    {
        get_volt_current_measure_result(VOLT_CH_ALL, &pro_adc_value, &v_value[0]);
        send_volt_value(&v_value[volt_index], &volt_index);
    }

    if (curr_index >= 2)
    {
        send_ch_num(CURR_SEND_CAN_ID, curr_index);
        curr_index = 0;
    }
    else
    {
        get_volt_current_measure_result(CURR_CH_ALL, &pro_adc_value, &c_value[0]);
        send_curr_value(&c_value[curr_index], &curr_index);
    }

    if (uid_index++ > 10)
    {
        uid_index = 0;
        send_mcu_id();
    }
    if ((get_hw_time_tick() - pro_verify_timeout) > 3000)
    {
        printf("- Product verify timeout.\r\n");
        NVIC_SystemReset();
    }
}

static void pro_over_handle(void *parameter)
{
    uint8_t res = 0;
    static uint8_t r_info[PRO_DATA_SIZE] = {0};
    uint8_t msg[8] = {0xf0, 0x00, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0xff};

    memset(r_info, 0, PRO_DATA_SIZE);
    product->key_word = 0x5f5f55aa;
    strncpy((char *)&product->hard_version[0], HARD_VERSION, sizeof(HARD_VERSION));   /* save haed version*/
    sprintf((char *)&product->soft_version[0], "V1.%d.%d", BSP_VERSION, APP_VERSION); /* save soft version*/

    /* 存储信息*/
    pro_storage_write(product_buff);
    pro_storage_read(r_info);

    res = strncmp((const char *)product_buff, (const char *)r_info, PRO_DATA_SIZE);

    /* 存储的信息和配置的信息一致，认为生产成功，退出生产模式*/
    if (res == 0)
    {
        can_get_conf.flag = 254;
        can_get_conf.data = (struct product_s *)r_info;

        /* 删除生产定时器*/
        soft_time_delete(&product_start);
        soft_time_delete(&product_over);
        soft_time_delete(&product_dev_id);

        /* 启动发送生产信息定时器*/
        soft_time_start(can_send_pro);
    }
    else
    {
        msg[1] = 0x02; /* 检验失败*/
        view_product_info(product);
        printf("\r\n- Recv product info\r\n");
        view_product_info((struct product_s *)r_info);
        can_send_data(PRO_OVER_CAN_ID, msg);
    }
}
static void start_pro_verify(struct my_can_recv *const msg)
{
 //   open_5v_output(); /* 模式输出5V*/
    set_switch_output(OUT_CH_ALL, IO_HIGH);

    product_start = soft_time_create("pro_start", pro_verift_handle, msg, 10, SOFT_TIMER_TYPE_PERIODIC);
    product_over = soft_time_create("pro_over", pro_over_handle, NULL, 30, SOFT_TIMER_TYPE_ONE_SHOT);
    product_dev_id = soft_time_create("pro_id", send_dev_id, &product->dev_id[0], 50, SOFT_TIMER_TYPE_PERIODIC);
    printf("- Star product verify.\r\n");
    soft_time_start(product_start);
    soft_time_start(product_dev_id);
}
void pro_verify_can_recv(uint32_t id, uint8_t *msg)
{
    static struct my_can_recv cmd_msg[3] = {0};

    switch (id)
    {
    case CAN_PRO_REV1_ID: /* 接收开始测试id*/
        break;
    case CAN_PRO_REV2_ID: /* 接收控制开关量采集模式id*/
        cmd_msg[0].id = id;
        memcpy(cmd_msg[0].data, msg, 8);
        break;
    case CAN_PRO_REV3_ID: /* 接收电阻控制*/
        cmd_msg[1].id = id;
        memcpy(cmd_msg[1].data, msg, 8);
        break;
    case CAN_PRO_REV4_ID: /* 接收开关量输出高低控制id*/
        cmd_msg[2].id = id;
        memcpy(cmd_msg[2].data, msg, 8);
        break;
    }

    pro_verify_timeout = get_hw_time_tick();

    if (id == CAN_PRO_REV1_ID)
    {
        if ((msg[2] == 0xaa) && (msg[3] == 0x55) && (msg[4] == 0xaa) && (msg[5] == 0x55) && (msg[6] == 0xaa))
        {
            /* 开始检验*/
            if (msg[0] == 0x01)
            {
                if (product_start == NULL)
                {
                    start_pro_verify(cmd_msg);
                }
            }
            else if (msg[0] == 0xf0)
            {
                /* 应用层读取配置信息*/
                can_get_conf.flag = msg[1];
                soft_time_start(can_send_pro);
            }
        }
        else if ((msg[0] == 0xf0) && (msg[7] == 0xaa))
        {
            /* 结束生产检验*/
            if (product_over != NULL)
            {
                memcpy((char *)&product->over_time[0], &msg[1], 6);
                soft_time_start(product_over);
            }
        }
        else if (msg[0] == 0xef)
        {
            /* 配置设备编号*/
            memcpy(&product->dev_id[msg[1]], &msg[2], 6);
        }
        else if ((msg[0] == 0xee) && (msg[6] == 0x55) && (msg[7] == 0xaa))
        {
            /* 工装请求终检的设备是本设备*/
            if ((msg[1] == product->dev_id[11]) && (msg[2] == product->dev_id[12]) &&
                    (msg[3] == product->dev_id[13]) && (msg[4] == product->dev_id[14]) && (msg[5] == product->dev_id[15]))
            {
                /* 开始终检*/
                start_pro_verify(cmd_msg);
            }
            else
            {
                /* code */
            }
        }
        else
        {
            /* code */
        }
    }
}

struct product_s *get_soft_library_info()
{
    pro_storage_read(product_buff);

    can_get_conf.flag = 0;
    can_get_conf.data = product;
    if (product->key_word == 0x5f5f55aa)
    {
        can_send_pro = soft_time_create("can_send_pro", can_send_pro_info, &can_get_conf, 50, SOFT_TIMER_TYPE_PERIODIC);
        return product;
    }
    return NULL;
}
