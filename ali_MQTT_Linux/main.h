#ifndef __MAIN_H_
#define __MAIN_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

/*
0锟紷/sys/i4ij6A1ED0l/4zvscHrvOlhluiNwR1R5/thing/service/property/set{"method":"thing.service.property.set","id":"1066884841","params":{"switch":0},"version":"1.0.0"}
*/

#define SERVER_IP "106.15.229.25"
#define SERVER_PORT 1883
#define CLIENTID "i4ij6A1ED0l.4zvscHrvOlhluiNwR1R5|securemode=2,signmethod=hmacsha256,timestamp=1713754649442|"
#define USERNAME "4zvscHrvOlhluiNwR1R5&i4ij6A1ED0l"
#define PASSWORD "7b3059b7689e507f239e185bb186966ab907f46a31fdd4b0d2856265c01620d6"
#define SUB_TOPIC_MESSAGE "/sys/i4ij6A1ED0l/4zvscHrvOlhluiNwR1R5/thing/service/property/set" // 涓嬪彂娑堟伅
// #define SUB_TOPIC_COMMAND "$oc/devices/661f60a62ccc1a583881ac02_device001/sys/commands/#"    // 涓嬪彂鍛戒护
#define PUB_TOPIC "/sys/i4ij6A1ED0l/4zvscHrvOlhluiNwR1R5/thing/event/property/post"

int client_senddata(unsigned char *buffer, unsigned int len);
int client_getdata(unsigned char *buffer);

#endif