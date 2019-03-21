#ifndef __DEV_MEMORY_H__
#define __DEV_MEMORY_H__

#include "board.h"

#define mem_pool_size 4 * 1024 /*内存池的大小*/

extern void *dev_malloc(unsigned int size);
extern void dev_free(void *pointer);
#endif /* end of include guard: __DEV_MEMORY_H__ */
