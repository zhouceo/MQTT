#include "main.h"
#include "mqtt.h"

double water_consumption = 0.0;
int pressure = -50;
double import_accumulate = 0.0;
double export_accumulate = 0.0;
float battery_voltage = 0.0;
float battery_voltage_low_alarm = 0.0;
int ecl = -32768;
int rsrp = -32768;
int sinr = -32768;
int pci = -32768;
int cell_id = -2147483648;
#define IMEI "4567845678456780"
#define IMSI "4567845678456780"
#define ICCID "123451234512345"

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
    int time;
    char report_time5[10], report_time4[10], report_time3[10], report_time2[10], report_time1[10];
    memset(mqtt_message, 0, sizeof(mqtt_message));
    int recvdatalen = 0;
    // char buffer[500] = {0};
    char send_cmd[1024] = {0};
    char *p;
    char *q;
    char *end = NULL;
    char *buffer = NULL;
    while (1)
    {
        // memset(buffer, 0, sizeof(buffer));
        buffer = calloc(1024, 1);
        recvdatalen = recv(sfd, buffer, 500, 0); // 接收服务器的消息
        if (recvdatalen == -1)
        {
            printf("recv failed with error\r\n");
            free(buffer);
            return (void *)1;
        }
        printf("recvdatalen = %d\r\n", recvdatalen);
        if (recvdatalen > 0)
        {
            printf("Server sends messages:\r\n");
            for (int i = 0; i < recvdatalen; i++)
            {
                printf("%c", buffer[i]);
            }
            printf("buffer= %s\r\n", &buffer[21]);
            printf("\r\n");
            p = strstr(&buffer[21], "\"taskId\":");
            q = strstr(&buffer[21], "\"payload\":");
            printf("p = %s\n", p);
            printf("q = %s\n", q);
            if (p && q)
            {
                char payload[100] = {0};
                int taskId = 0;
                sscanf(p, "\"taskId\":%d", &taskId);
                printf("taskId = %d\n", taskId);
                q = strstr(q, "{");
                end = strstr(q, "}");
                strncpy(payload, q, end - q + 1);
                printf("payload = %s\n", payload);
                sprintf(mqtt_message, "{\"taskId\":%d,\"resultPayload\":%s}", taskId, payload);
                if (strstr(payload, "valve_onoff"))
                {
                    MQTT_Publish((char *)VALVE_ONOFF_RESP, mqtt_message, 0);
                }
                if (strstr(payload, "time"))
                {
                    MQTT_Publish((char *)SET_CONFIG_CMD_RESP, mqtt_message, 0);
                    // sscanf(payload, "{\"time\":%d,\"report_time5\":\"%[^\"]\",\"report_time4\":\"%[^\"]\",\"report_time3\":\"%[^\"]\",\"report_time2\":\"%[^\"]\",\"report_time1\":\"%[^\"]\"}", &time, report_time5, report_time4, report_time3, report_time2, report_time1);
                    // printf("time: %d\n", time);
                    // printf("report_time5: %s\n", report_time5);
                    // printf("report_time4: %s\n", report_time4);
                    // printf("report_time3: %s\n", report_time3);
                    // printf("report_time2: %s\n", report_time2);
                    // printf("report_time1: %s\n", report_time1);
                }
                printf("指令下发响应数据:%s\r\n", mqtt_message);
            }
        }
        free(buffer);
    }
}

int main(int argc, char *argv[])
{
    char timebuf[30] = {0};
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
        // 业务数据上报
        sprintf(mqtt_message, "{\"time\":\"%s\",\"water_consumption\":%.1f,\"pressure\":%d,\"switch_state\":0,\"import_accumulate\":%.1f,\"export_accumulate\":%.1f,\"IMEI\":%s,\"IMSI\":%s,\"ICCID\":%s,\"battery_voltage\":%.1f}", now_time(), (double)(water_consumption += 0.5), pressure += 5, (double)(import_accumulate += 0.5), (double)(export_accumulate += 0.5), IMEI, IMSI, ICCID, (double)(battery_voltage += 0.5));
        MQTT_Publish((char *)DATA_REPORT, mqtt_message, 0);
        memset(mqtt_message, 0, sizeof(mqtt_message));
        // 信号数据上报
        sprintf(mqtt_message, "{\"ecl\":%d,\"rsrp\":%d,\"sinr\":%d,\"pci\":%d,\"cell_id\":%d}", ecl += 5, rsrp += 5, sinr += 5, pci += 5, cell_id += 10);
        MQTT_Publish((char *)SIGNAL_REPORT, mqtt_message, 0);
        memset(mqtt_message, 0, sizeof(mqtt_message));
        // 电池低电压告警
        sprintf(mqtt_message, "{\"battery_voltage\":%.1f}", (double)(battery_voltage_low_alarm += 0.5));
        MQTT_Publish((char *)BATTERY_VOLTAGE_LOW_ALARM, mqtt_message, 0);
        memset(mqtt_message, 0, sizeof(mqtt_message));
        // 故障上报
        sprintf(mqtt_message, "{\"error_code\":0}");
        MQTT_Publish((char *)ERROR_CODE_REPORT, mqtt_message, 0);
        memset(mqtt_message, 0, sizeof(mqtt_message));

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