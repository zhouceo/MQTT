#ifndef __MQTT_H__
#define __MQTT_H__

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#define BYTE0(deTemp) (*(char *)(&deTemp))
#define BYTE1(deTemp) (*((char *)(&deTemp) + 1))
#define BYTE2(deTemp) (*((char *)(&deTemp) + 2))
#define BYTE3(deTemp) (*((char *)(&deTemp) + 3))

unsigned char mqtt_rxbuff[1024 * 1024]; // 接收数据缓存区
unsigned char mqtt_txbuff[256];         // 发送数据缓存区
unsigned int mqtt_rxlen;
unsigned int mqtt_txlen;

enum
{
    MQTT_CONNECT_SUCCESS,
    MQTT_CONNECT_FAIL,
    MQTT_SUBSCRIBE_SUCCESS,
    MQTT_SUBSCRIBE_FAIL,
    MQTT_PUBLISH_SUCCESS,
    MQTT_PUBLISH_FAIL,
};
enum
{
    MQTT_SUBSCRIBE,
    MQTT_UNSUBSCRIBE
};

void MQTT_Init(void);
unsigned char MQTT_Connect(char *ClientID, char *UserName, char *Password);
unsigned char MQTT_Publish(char *topic, char *message, unsigned char qos);
unsigned char MQTT_Subscribe(char *topic, unsigned char qos, unsigned char flag);
void MQTT_SendBuffer(unsigned char *buffer, unsigned int len);

#endif // !1 __MQTT_C__
