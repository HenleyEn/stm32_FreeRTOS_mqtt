/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 18:27:19
 * @LastEditTime: 2020-04-27 22:22:27
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_mutex.h"

int platform_mutex_init(platform_mutex_t* m)
{
    m->mutex = xSemaphoreCreateMutex();
    return 0;
}

int platform_mutex_lock(platform_mutex_t* m)
{
    return xSemaphoreTake(m->mutex, portMAX_DELAY);
}

int platform_mutex_lock_timeout(platform_mutex_t* m, int timeout)
{
    return xSemaphoreTake(m->mutex, timeout);
}

int platform_mutex_trylock(platform_mutex_t* m)
{
    return xSemaphoreTake(m->mutex, 0);
}

int platform_mutex_unlock(platform_mutex_t* m)
{
    return xSemaphoreGive(m->mutex);
}

int platform_mutex_unlock_from_isr(platform_mutex_t* m)
{
	static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(m->mutex, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	return pdTRUE;
}

int platform_mutex_destroy(platform_mutex_t* m)
{
    vSemaphoreDelete(m->mutex);
    return 0;
}


int platform_semaphore_init(struct platform_semaphore *semphr, int max_count, int init_count)
{
   	semphr->semaphore = xSemaphoreCreateCounting(max_count, init_count);
    return 0;
}

int platform_semphr_lock(struct platform_semaphore* semphr)
{
    return xSemaphoreTake(semphr->semaphore, portMAX_DELAY);
}

int platform_semphr_lock_timeout(struct platform_semaphore* semphr, int timeout)
{
    return xSemaphoreTake(semphr->semaphore, timeout);
}

int platform_semphr_unlock(struct platform_semaphore* semphr)
{
    return xSemaphoreGive(semphr->semaphore);
}

int platform_semphr_destroy(struct platform_semaphore* semphr)
{
    vSemaphoreDelete(semphr->semaphore);
    return 0;
}

int platform_semphr_unlock_from_isr(struct platform_semaphore* semphr)
{
	static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(semphr->semaphore, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	return pdTRUE;
}

