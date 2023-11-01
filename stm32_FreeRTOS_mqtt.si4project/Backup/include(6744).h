#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"                  // Device header

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "projectConfig.h"

#include "USART.h"
#include "ringbuf.h"
#include "ESP8266_AT.h"
#include "device_mutex.h"







