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
// 数据上报
#define DATA_REPORT "data_report"     // 业务数据上报{"temperature":25,"hydraulic_value":5}
#define SIGNAL_REPORT "signal_report" // 信号数据上报
#define INFO_REPORT "info_report"     // 设备信息上报
// 事件上报
#define BATTERY_VOLTAGE_LOW_ALARM "battery_voltage_low_alarm" // 电池低电压告警
// 指令下发
#define VALVE_ONOFF_CMD "valve_onoff_cmd"   // 阀门开关控制
#define VALVE_ONOFF_RESP "valve_onoff_resp" // 阀门开关控制
// #define SUB_TOPIC_COMMAND "valve_onoff_cmd"      // 指令下发  收到数据{"taskId":1,"payload":{"valve_onoff":1}}
// #define SUB_TOPIC_COMMAND_ACK "valve_onoff_resp" // 指令下发响应    {"taskId":1,"resultPayload":{"valve_onoff":1}}

int client_senddata(unsigned char *buffer, unsigned int len);
int client_getdata(unsigned char *buffer);

#endif