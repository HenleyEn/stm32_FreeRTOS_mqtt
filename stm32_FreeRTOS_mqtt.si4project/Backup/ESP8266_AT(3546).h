#ifndef ESP8266_AT
#define ESP8266_AT

#ifdef __cplusplus
extern "C" {
#endif

int AT_send_cmd(uint8_t data, int len, uint8_t timeout);
int AT_recv();
int AT_ack();


#ifdef __cplusplus
}
#endif

#endif
