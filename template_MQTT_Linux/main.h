#ifndef __MAIN_H_
#define __MAIN_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#define SERVER_IP "180.106.148.146"
#define SERVER_PORT 1883
#define CLIENTID "17076576test001"
#define USERNAME "asdfghjkl"
#define PASSWORD "DA6I_LpQbIidAtjq900ieHgvz7SggplCqO3eVTdg2vI"
#define PUB_TOPIC_DATA "data_report"             // 数据上报{"temperature":25,"hydraulic_value":5}                                                          // 数据上报
#define PUB_TOPIC_EVENT "valve_state_report"     // 事件上报{"valve_state":1}
#define SUB_TOPIC_COMMAND "valve_onoff_cmd"      // 指令下发  收到数据{"taskId":1,"payload":{"valve_onoff":1}}
#define SUB_TOPIC_COMMAND_ACK "valve_onoff_resp" // 指令下发响应    {"taskId":1,"resultPayload":{"valve_onoff":1}}

int client_senddata(unsigned char *buffer, unsigned int len);
int client_getdata(unsigned char *buffer);

#endif