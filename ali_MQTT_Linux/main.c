#include "main.h"
#include "mqtt.h"

double TEMP = 10.0;
char mqtt_message[1024]; // 数据缓存区
int sfd = 0;             // 套接字描述符
pthread_t pid;           // 保存线程的ID

void sys_err(const char *str)
{
    perror(str);
    exit(1);
}
// 字符串截取函数
void my_substr(char *str, char *buff, int s, int l)
{
    for (int i = 0; i < l; i++)
    {
        buff[i] = *(str + s + i);
    }
}

void *ReceiveData(void *arg)
{
    char request_id[100] = {0};
    int recvdatalen = 0;
    char buffer[1024] = {0};
    char send_cmd[500] = {0};
    while (1)
    {
        recvdatalen = recv(sfd, buffer, sizeof(buffer), 0); // 接收服务器的消息
        if (recvdatalen == -1)
        {
            printf("recv failed with error\r\n");
            return (void *)1;
        }
        if (recvdatalen > 0)
        {
            printf("Server sends messages:\r\n");
            for (int i = 5; i < recvdatalen; i++)
            {
                printf("%c", buffer[i]);
            }
            printf("\r\n");
            char *p = NULL;
            p = strstr(&buffer[5], SUB_TOPIC_MESSAGE);
            if (p)
            {
                p = strstr(p, "\"switch\":");
                if (p)
                {
                    int lightSwitchValue = -1;
                    if (sscanf(p, "\"switch\":%d", &lightSwitchValue) != -1)
                    {
                        printf("swtich = %d\n", lightSwitchValue);
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0)
        sys_err("socket error");
    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &ser_addr.sin_addr.s_addr);
    int cfd = connect(sfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
    if (cfd < 0)
        sys_err("connect error");
    printf("Connected to server.\r\n");
    MQTT_Init();
    while (1)
    {
        // 登录服务器
        if (MQTT_Connect((char *)CLIENTID, (char *)USERNAME, (char *)PASSWORD) == MQTT_CONNECT_SUCCESS)
        {
            break;
        }
        // 延时1s，重新连接
        sleep(1);
        printf("MQTT server is logging in again....\r\n");
    }
    printf("Successfully connected to server\r\n");
    // 连接服务器成功后，创建线程
    int ret = pthread_create(&pid, NULL, ReceiveData, NULL);
    if (ret != 0)
        sys_err("pthread_create failed");
    /*
        // 订阅
        int status = MQTT_Subscribe((char *)SUB_TOPIC_COMMAND, 1, MQTT_SUBSCRIBE);
        if (status == MQTT_SUBSCRIBE_FAIL)
        {
            printf("MQTT_Subscribe failed\r\n");
            return 1;
        }
        printf("MQTT_Subscribe success\r\n");
    */
    // 发布
    while (1)
    {
        sprintf(mqtt_message, "{\
			\"method\":\"thing.event.property.post\",\
			\"id\":\"102277852\",\
			\"params\":{\
				\"Humidity\":%.1f\
			},\
			\"version\":\"1.0.0\"\
		}",
                (double)(TEMP += 0.2)); // 温度

        // 发布主题
        MQTT_Publish((char *)PUB_TOPIC, mqtt_message, 0);
        printf("MQTT publish message successfully\r\n");
        sleep(5);
    }

    return 0;
}

// 发送数据到服务器
int client_senddata(unsigned char *buffer, unsigned int len)
{
    int ret = send(sfd, (const char *)buffer, len, 0); // 发送数据到服务器
    if (ret == -1)
    {
        printf("send failed with error\r\n");
        return 1;
    }
    return 0;
}
// 从服务器获取数据
int client_getdata(unsigned char *buffer)
{
    int ret = recv(sfd, (char *)buffer, 200, 0);
    if (ret < 0)
    {
        printf("recv failed with error\r\n");
        return 1;
    }
    return ret;
}