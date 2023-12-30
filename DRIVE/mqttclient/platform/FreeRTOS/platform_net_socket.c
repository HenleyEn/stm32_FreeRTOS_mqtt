/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-10 23:45:59
 * @LastEditTime: 2020-04-25 17:50:58
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_net_socket.h"
#include "platform_timer.h"
#include "bsp_usart_config.h"
#include "stdio.h"
#include "at_client.h"
#include "projectConfig.h"

int platform_net_socket_connect(const char *host, const char *port, int proto)
{
	int ret;
	char cmd[100];
	ret = at_send_cmd(NULL, "AT+CWMODE=3");
	if(!ret)
	{
		printf("AT+CWMODE is %d\r\n", ret);
//		return ret;
	}
	
	ret = at_send_cmd(NULL, "AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PASSWORD "\"");
	
	if(!ret)
	{
		printf("connect AP err is %d\r\n", ret);
//		return ret;
	}
	
//	AT+CIPSTART="TCP","192.168.5.3",777	
	if(proto == PLATFORM_NET_PROTO_TCP)
	{
		sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", host, port);
	}
	else
	{
		sprintf(cmd, "AT+CIPSTART=\"UDP\",\"%s\",%s", host, port);
	}
	
	ret = at_send_cmd(NULL, (uint8_t *)cmd);
	if(!ret)
	{
		printf(" %s is error %d\r\n", cmd, ret);
//		return ret;
	}
	return ret;
}
/*
int platform_net_socket_recv(int fd, void *buf, size_t len, int flags)
{

    return 0;
}
*/
int platform_net_socket_recv_timeout(int fd, unsigned char *buf, int len, int timeout)
{


	return 0;
}

/*
int platform_net_socket_write(int fd, void *buf, size_t len)
{

	return 0;
}
*/
int platform_net_socket_write_timeout(int fd, unsigned char *buf, int len, int timeout)
{
	int ret;
	char cmd[20];

	sprintf(cmd, "AT+CIPSEND=%d", len);
	ret = at_send_cmd(NULL, (uint8_t *)cmd);
	if(!ret)
	{
		printf(" %s is error %d\r\n", cmd, ret);
	//		return ret;
	}

	return 0;
}

//int platform_net_socket_close(int fd)
//{
//	return 0;
//}

int platform_net_socket_close(int fd)
{
	int ret;
	
	ret = at_send_cmd(NULL, "AT+CIPCLOSE");
	
	if(!ret)
	{
		printf("AT+CIPCLOSE is %d\r\n", ret);
		return ret;
	}

	return ret;
}

