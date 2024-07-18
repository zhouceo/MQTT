#ifndef __MAIN_H_
#define __MAIN_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#define SERVER_IP "117.78.5.125"
#define SERVER_PORT 1883
#define CLIENTID "661f60a62ccc1a583881ac02_device001_0_1_2024042202"
#define USERNAME "661f60a62ccc1a583881ac02_device001"
#define PASSWORD "10900072fcec5ad94f18b9cb8eb40b5979765f2eda5405d5747e46789b60b6ba"
#define SUB_TOPIC_MESSAGE "$oc/devices/661f60a62ccc1a583881ac02_device001/sys/messages/down" // 下发消息
#define SUB_TOPIC_COMMAND "$oc/devices/661f60a62ccc1a583881ac02_device001/sys/commands/#"    // 下发命令
#define PUB_TOPIC "$oc/devices/661f60a62ccc1a583881ac02_device001/sys/properties/report"

int client_senddata(unsigned char *buffer, unsigned int len);
int client_getdata(unsigned char *buffer);

#endif