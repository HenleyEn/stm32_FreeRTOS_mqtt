#ifndef PLATFORM_MUTEX
#define PLATFORM_MUTEX

#include "include.h"

#define platform_create_mutex() 				xSemaphoreCreateMutex()
#define platform_take_mutex(mutex, timeout)		xSemaphoreTake(mutex, timeout)
#define platform_give_mutex(mutex)				xSemaphoreGive(mutex)
#define platform_destory(mutex)					vSemaphoreDelete(mutex)
typedef struct 
{
	SemaphoreHandle_t mutex;
}platform_mutex;

int platform_mutex_init(platform_mutex* mutex);
int platform_mutex_lock_timeout(platform_mutex* mutex, int timeout);
int platform_mutex_unlock(platform_mutex* mutex);
int platform_mutex_destroy(platform_mutex* mutex);

#endif




