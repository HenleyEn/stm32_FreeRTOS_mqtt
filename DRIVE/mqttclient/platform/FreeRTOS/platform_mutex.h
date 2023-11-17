/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 18:31:33
 * @LastEditTime: 2020-04-27 17:04:46
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _PLATFORM_MUTEX_H_
#define _PLATFORM_MUTEX_H_

#include "FreeRTOS.h"
#include "semphr.h"

typedef struct platform_mutex {
    SemaphoreHandle_t mutex;
} platform_mutex_t;

struct platform_semaphore
{
	SemaphoreHandle_t semaphore;
};
typedef struct platform_semaphore *platform_semaphore_t;


int platform_mutex_init(platform_mutex_t* m);
int platform_mutex_lock(platform_mutex_t* m);
int platform_mutex_lock_timeout(platform_mutex_t* m, int timeout);

int platform_mutex_trylock(platform_mutex_t* m);
int platform_mutex_unlock_from_isr(platform_mutex_t* m);
int platform_mutex_unlock(platform_mutex_t* m);
int platform_mutex_destroy(platform_mutex_t* m);

int platform_semaphore_init(struct platform_semaphore *semphr, int max_count, int init_count);
int platform_semphr_lock(struct platform_semaphore* semphr);
int platform_semphr_unlock(struct platform_semaphore* semphr);
int platform_semphr_destroy(struct platform_semaphore* semphr);
int platform_semphr_unlock_from_isr(struct platform_semaphore* semphr);


#endif
