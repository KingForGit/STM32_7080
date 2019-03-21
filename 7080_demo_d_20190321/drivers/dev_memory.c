#include "dev_memory.h"

//#define DEBUG_DEV_MEMORY
#ifdef DEBUG_DEV_MEMORY
    #define mem_dbg(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define mem_dbg(fmt, ...)
#endif

enum status
{
    MEM_UNUSED = 0, /*定义内存块的使用状态(MEM_MEM_UNUSED 未使用),(MEM_USED 已使用)*/
    MEM_USED = 1
};

#pragma pack(1)
struct mem_tcb /*定义内存管理块的数据结构*/
{
    void *mem_ptr;           /*当前内存块的内存地址*/
    struct mem_tcb *nxt_ptr; /*下一个内存管理块的地址*/
    unsigned int mem_size;   /*当前内存块的大小*/
    enum status status;      /*当前内存块的状态*/
};
#pragma pack()

#define mem_tcb_size sizeof(struct mem_tcb) /*内存管理块的大小*/
static char dev_mem_pool[mem_pool_size];    /*定义用来内存分配的数组*/

#define mem_pool_start &dev_mem_pool[0]                   /*定义内存池的首地址*/
#define mem_pool_end &dev_mem_pool[mem_pool_size]         /*定义内存池的尾地址*/
#define head_node ((uint32_t)mem_pool_end - mem_tcb_size) /*头内存管理块的地址*/

static signed char dev_mem_init_flag = -1; /*内存分配系统初始化的标志(-1 未初始化),(1 已初始化)*/

/**
* @desc  : 内存分配初始化
* @param : none
* @return: none
* @Date  : 2019-1-23 09:58:05
*/
static void dev_mem_init(void)
{
    struct mem_tcb *node;
    memset(dev_mem_pool, 0x00UL, sizeof(mem_pool_size));
    node = (struct mem_tcb *)head_node;
    node->mem_ptr = mem_pool_start;
    node->nxt_ptr = node;
    node->mem_size = mem_pool_size - mem_tcb_size;
    node->status = MEM_UNUSED;

    dev_mem_init_flag = 1;
    mem_dbg("mem start ptr 0x%p\r\n", mem_pool_start);
    mem_dbg("mem end ptr 0x%p\r\n", mem_pool_end);
    mem_dbg("mem str size %d\r\n", mem_pool_size - mem_tcb_size);
}

/**
 * @desc  : 分配内存
 * @param : size 分配的内存大小
 * @return: 分配的地址
 * @Date  : 2019-1-23 10:06:04
 */
void *dev_malloc(unsigned int size)
{
    unsigned int suit_size = 0xffffffffUL;
    struct mem_tcb *n_head = NULL;
    struct mem_tcb *n_tmp = NULL;
    struct mem_tcb *suit_node = NULL; /* 查找合适的内存块*/

    if ((size == 0) || (size >= mem_pool_size))
    {
        mem_dbg("- malloc lenth error len%d\r\n", size);
        return NULL;
    }

    if (dev_mem_init_flag != 1)
    {
        mem_dbg("- dev malloc noinit\r\n");
        dev_mem_init();
    }

    n_head = n_tmp = (struct mem_tcb *)head_node;
    while (1)
    {
        if (n_tmp->status == MEM_UNUSED)
        {
            if ((size <= n_tmp->mem_size) && (n_tmp->mem_size < suit_size))
            {
                suit_node = n_tmp;
                suit_size = suit_node->mem_size;
            }
        }
        n_tmp = n_tmp->nxt_ptr;
        if (n_tmp == n_head)
        {
            if (suit_node == NULL)
            {
                mem_dbg("- malloc failed!\r\n");
                return NULL;
            }
            break;
        }
    }
    if (size <= suit_node->mem_size && (size + mem_tcb_size) >= suit_node->mem_size)
    {
        suit_node->status = MEM_USED;
        return suit_node->mem_ptr;
    }
    else if (suit_node->mem_size > (size + mem_tcb_size))
    {
        n_tmp = suit_node->mem_ptr;

        n_tmp = (struct mem_tcb *)((unsigned char *)n_tmp + size);
        n_tmp->mem_ptr = suit_node->mem_ptr;
        n_tmp->nxt_ptr = suit_node->nxt_ptr;
        n_tmp->mem_size = size;
        n_tmp->status = MEM_USED;

        suit_node->mem_ptr = (struct mem_tcb *)((unsigned char *)n_tmp + mem_tcb_size);
        suit_node->nxt_ptr = n_tmp;
        suit_node->mem_size -= (size + mem_tcb_size);
        suit_node->status = MEM_UNUSED;

        return n_tmp->mem_ptr;
    }
    else
    {
        mem_dbg("%s,size err!\r\n", __FUNCTION__);
    }
    return NULL;
}
/**
 * @desc  : 内存释放
 * @param :
 * @return:
 * @Date  :
 */
void dev_free(void *pointer)
{
    struct mem_tcb *n_head;
    struct mem_tcb *n_tmp;
    struct mem_tcb *nxt_node;

    if (pointer == NULL)
        return;

    if (dev_mem_init_flag < 0)
        return;

    n_head = n_tmp = (struct mem_tcb *)head_node;
    while (1)
    {
        if (n_tmp->mem_ptr == pointer)
        {
            if (n_tmp->status != MEM_UNUSED)
            {
                n_tmp->status = MEM_UNUSED;
                break;
            }
            else
            {
                mem_dbg("pointer:0x%08x\r\n", pointer);
                return;
            }
        }
        n_tmp = n_tmp->nxt_ptr;
        if (n_tmp == n_head)
        {
            mem_dbg("%s,can not found pointer!\r\n", __FUNCTION__);
            return;
        }
    }
AGAIN:
    n_head = n_tmp = (struct mem_tcb *)head_node;
    while (1)
    {
        nxt_node = n_tmp->nxt_ptr;
        if (nxt_node == n_head)
        {
            break;
        }
        if (n_tmp->status == MEM_UNUSED && nxt_node->status == MEM_UNUSED)
        {
            n_tmp->mem_ptr = nxt_node->mem_ptr;
            n_tmp->nxt_ptr = nxt_node->nxt_ptr;
            n_tmp->mem_size += nxt_node->mem_size + mem_tcb_size;
            n_tmp->status = MEM_UNUSED;
            goto AGAIN;
        }
        n_tmp = nxt_node;
    }
}
