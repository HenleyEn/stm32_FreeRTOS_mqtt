#include "include.h"

#if defined PLATFORM_MUTEX_USING



int platform_mutex_init(platform_mutex* mutex)
{
	mutex->mutex = platform_create_mutex();
	return TRUE;
}

int platform_mutex_lock_timeout(platform_mutex* mutex, int timeout)
{
	return platform_take_mutex(mutex->mutex, timeout);
}

int platform_mutex_unlock(platform_mutex_t* mutex)
{
    return platform_give_mutex(mutex->mutex);
}

int platform_mutex_destroy(platform_mutex* mutex)
{
    platform_destory(mutex->mutex);
    return 0;
}


#endif


