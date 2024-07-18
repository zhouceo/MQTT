#include "mqtt.h"
#include "main.h"

// 客户端成功连接服务器响应  0x20 0x02 0x00 0x00
const unsigned char connnectACKdata[] = {0x20, 0x02, 0x00, 0x00};
// 订阅应答
const unsigned char subACKdata[] = {0x90, 0x03}; // 0x90 0x03 0x00 0x0A 0x01

void MQTT_Init(void)
{
    mqtt_rxlen = sizeof(mqtt_rxbuff);
    mqtt_txlen = sizeof(mqtt_txbuff);
    memset(mqtt_rxbuff, 0, mqtt_rxlen);
    memset(mqtt_txbuff, 0, mqtt_txlen);
}

/**
 * @功能    连接服务器
 * @参数    服务器三元组
 * @返回值  成功 MQTT_CONNECT_SUCCESS
 *          失败 MQTT_CONNECT_FAIL
 */
unsigned char MQTT_Connect(char *ClientID, char *UserName, char *Password)
{
    unsigned int datalen = 0;
    unsigned int rxdatalen = 0;
    unsigned char buffer[256] = {0};
    mqtt_txlen = 0;
    int clintidlen = (int)strlen(ClientID);
    int usernamelen = (int)strlen(UserName);
    int passwordlen = (int)strlen(Password);
    datalen = 10 + (clintidlen + 2) + (usernamelen + 2) + (passwordlen + 2); // 可变报头的长度（10个字节）+有效载荷的长度
    /**固定报头**/
    mqtt_txbuff[mqtt_txlen++] = 0x10;
    // 剩余长度(不包括固定报头)
    do
    {
        unsigned char encodedByte = datalen % 128;
        datalen = datalen / 128;
        // 如果有更多的数据要编码，设置该字节的最高位
        if (datalen > 0)
            encodedByte = encodedByte | 128;
        mqtt_txbuff[mqtt_txlen++] = encodedByte;
    } while (datalen > 0);
    /**可变报头**/
    // 协议名(Protocol Name)
    mqtt_txbuff[mqtt_txlen++] = 0x00; // MSB(0)
    mqtt_txbuff[mqtt_txlen++] = 0x04; // LSB(4)
    mqtt_txbuff[mqtt_txlen++] = 'M';
    mqtt_txbuff[mqtt_txlen++] = 'Q';
    mqtt_txbuff[mqtt_txlen++] = 'T';
    mqtt_txbuff[mqtt_txlen++] = 'T';
    // 协议级别(Protocol Level)
    mqtt_txbuff[mqtt_txlen++] = 0x04; // Level(4)
    // 连接标志(Connect Flags)
    mqtt_txbuff[mqtt_txlen++] = 0xC2;
    // 保持连接(Keep Alive)
    mqtt_txbuff[mqtt_txlen++] = 0x00; // Keep-alive MSB
    mqtt_txbuff[mqtt_txlen++] = 100;  // Keep-alive Time Length LSB  100S心跳包
    /**有效载荷**/
    // 三元组
    mqtt_txbuff[mqtt_txlen++] = BYTE1(clintidlen); // ClientID MSB
    mqtt_txbuff[mqtt_txlen++] = BYTE0(clintidlen); // ClientID LSB
    memcpy(&mqtt_txbuff[mqtt_txlen], ClientID, clintidlen);
    mqtt_txlen += clintidlen;
    if (usernamelen > 0)
    {
        mqtt_txbuff[mqtt_txlen++] = BYTE1(usernamelen); // Username MSB
        mqtt_txbuff[mqtt_txlen++] = BYTE0(usernamelen); // Username LSB
        memcpy(&mqtt_txbuff[mqtt_txlen], UserName, usernamelen);
        mqtt_txlen += usernamelen;
    }
    if (passwordlen > 0)
    {
        mqtt_txbuff[mqtt_txlen++] = BYTE1(passwordlen); // Password MSB
        mqtt_txbuff[mqtt_txlen++] = BYTE0(passwordlen); // Password LSB
        memcpy(&mqtt_txbuff[mqtt_txlen], Password, passwordlen);
        mqtt_txlen += passwordlen;
    }
    for (int i = 0; i < 5; i++) // 循环5次，防止丢包
    {
        memset(mqtt_rxbuff, 0, mqtt_rxlen);
        MQTT_SendBuffer(mqtt_txbuff, mqtt_txlen);
        rxdatalen = client_getdata(buffer); // 从服务器获取数据
        if (rxdatalen <= 0)
            continue;
        memcpy(mqtt_rxbuff, buffer, rxdatalen);
        printf("CONNACK:"); // 登录应答
        for (int j = 0; j < rxdatalen; j++)
        {
            if (buffer[j] == 0)
                printf("0x00 ");
            else
                printf("%#04x ", buffer[j]);
        }
        printf("\r\n");
        if (mqtt_rxbuff[0] == connnectACKdata[0] && mqtt_rxbuff[1] == connnectACKdata[1] && mqtt_rxbuff[2] == connnectACKdata[2] && mqtt_rxbuff[3] == connnectACKdata[3])
        {
            return MQTT_CONNECT_SUCCESS; // 连接成功
        }
    }
    return MQTT_CONNECT_FAIL;
}

/**
 * @功能    发布消息
 * @参数    topic:订阅主题
 *          message:消息
 *          qos:消息等级,一般为0
 * @返回值  发送包长度
 */
unsigned char MQTT_Publish(char *topic, char *message, unsigned char qos)
{
    unsigned int datalen = 0;
    unsigned short id = 0;
    unsigned int topiclen = (int)strlen(topic);
    unsigned int messagelen = (int)strlen(message);
    mqtt_txlen = 0;
    printf("JSON message length:%d\r\n", messagelen);
    printf("message=%s\r\n", message);
    if (qos == 1 || qos == 2) // Qos等级为1或者2的时候报文标识符才存在，占两个字节
        datalen = (2 + topiclen) + 2 + messagelen;
    else if (qos == 0)                         // QoS等级为0，无报文标识符
        datalen = (2 + topiclen) + messagelen; // 2为可变报头前需要加上topic的长度
    /**固定报头**/
    mqtt_txbuff[mqtt_txlen++] = 0x30;
    // 剩余长度(不包括固定报头)
    do
    {
        unsigned char encodedByte = datalen % 128;
        datalen = datalen / 128;
        // 如果有更多的数据要编码，设置该字节的最高位
        if (datalen > 0)
            encodedByte = encodedByte | 128;
        mqtt_txbuff[mqtt_txlen++] = encodedByte;
    } while (datalen > 0);
    /**可变报头**/
    // 主题名
    mqtt_txbuff[mqtt_txlen++] = BYTE1(topiclen); // length MSB
    mqtt_txbuff[mqtt_txlen++] = BYTE0(topiclen); // length LSB
    memcpy(&mqtt_txbuff[mqtt_txlen], topic, topiclen);
    mqtt_txlen += topiclen;
    // 报文标识符
    if (qos == 1 || qos == 2)
    {
        mqtt_txbuff[mqtt_txlen++] = BYTE1(id);
        mqtt_txbuff[mqtt_txlen++] = BYTE0(id);
        id++;
    }
    /**有效载荷**/
    memcpy(&mqtt_txbuff[mqtt_txlen], message, messagelen);
    mqtt_txlen += messagelen;

    MQTT_SendBuffer(mqtt_txbuff, mqtt_txlen);
    return mqtt_txlen;
}

/**
 * @功能    订阅主题
 * @参数    topic:订阅主题
 *          qos:消息等级,一般为0
 *          flag:MQTT_SUBSCRIBE 订阅
 *               MQTT_UNSUBSCRIBE 取消订阅
 * @返回值  成功 MQTT_SUBSCRIBE_SUCCESS
 *          失败 MQTT_SUBSCRIBE_FAIL
 */
unsigned char MQTT_Subscribe(char *topic, unsigned char qos, unsigned char flag)
{
    unsigned int rxdatalen = 0;
    unsigned char buffer[256] = {0};
    unsigned int topiclen = (int)strlen(topic);
    unsigned int datalen = 0;
    mqtt_txlen = 0;
    datalen = 2 + 2 + topiclen + ((!flag) ? 1 : 0);
    /**固定报头**/
    if (flag == MQTT_SUBSCRIBE)
        mqtt_txbuff[mqtt_txlen++] = 0x82;
    else if (flag == MQTT_UNSUBSCRIBE)
        mqtt_txbuff[mqtt_txlen++] = 0xA2;
    // 剩余长度(不包括固定报头)
    do
    {
        unsigned char encodedByte = datalen % 128;
        datalen = datalen / 128;
        // 如果有更多的数据要编码，设置该字节的最高位
        if (datalen > 0)
            encodedByte = encodedByte | 128;
        mqtt_txbuff[mqtt_txlen++] = encodedByte;
    } while (datalen > 0);
    /**可变报头**/
    mqtt_txbuff[mqtt_txlen++] = 0x00; // MSB
    mqtt_txbuff[mqtt_txlen++] = 0x0A; // LSB
    /**有效载荷**/
    // 主题过滤器
    mqtt_txbuff[mqtt_txlen++] = BYTE1(topiclen);
    mqtt_txbuff[mqtt_txlen++] = BYTE0(topiclen);
    memcpy(&mqtt_txbuff[mqtt_txlen], topic, topiclen);
    mqtt_txlen += topiclen;
    // Qos
    if (flag == MQTT_SUBSCRIBE)
        mqtt_txbuff[mqtt_txlen++] = qos;
    for (int i = 0; i < 100; i++)
    {
        memset(mqtt_rxbuff, 0, sizeof(mqtt_rxbuff));
        MQTT_SendBuffer(mqtt_txbuff, mqtt_txlen);

        for (int j = 0; j < mqtt_txlen; j++)
        {
            if (mqtt_txbuff[j] == 0)
                printf("0x00 ");
            else
                printf("%#04x ", mqtt_txbuff[j]);
        }
        printf("\r\n");
        printf("Subscription message published successfully\r\n");
        rxdatalen = client_getdata(buffer); // 从服务器获取数据
        if (rxdatalen <= 0)
            continue;
        memcpy(mqtt_rxbuff, buffer, rxdatalen);
        printf("SUBACK:");
        for (int j = 0; j < rxdatalen; j++)
        {
            if (buffer[j] == 0)
                printf("0x00 ");
            else
                printf("%#04x ", buffer[j]);
        }
        printf("\r\n");
        if (mqtt_rxbuff[0] == subACKdata[0] && mqtt_rxbuff[1] == subACKdata[1])
        {
            return MQTT_SUBSCRIBE_SUCCESS; // 订阅成功
        }
        sleep(1);
    }
    return MQTT_SUBSCRIBE_FAIL;
}

void MQTT_SendBuffer(unsigned char *buffer, unsigned int len)
{
    client_senddata(buffer, len);
}