#ifdef _PRINTF_DEBUG_LOG_H
#define _PRINTF_DEBUG_LOG_H

#ifdef DEBUG
#define printf(...)	printf(__VA_ARGS__)
#else
#define printf(...)	

#endif/* END DEBUG */

#endif/* END _PRINTF_DEBUG_LOG_H */
