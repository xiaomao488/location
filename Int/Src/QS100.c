#include "QS100.h"
#include "gpio.h"
#include "stm32f1xx_hal.h"
#include "Com_Delay.h"
#include <string.h>
#include "usart.h"
#include "Com_Debug.h"

// 缓冲区
u8 QS100_Buff[QS100_Buff_MAX_LEN] = {0};
u16 QS100_Buff_Len = 0;
// 返回的结果
u8 QS100_Result[QS100_RESULT_MAX_LEN] = {0};
u16 QS100_Result_Len = 0;

// 唤醒
void Int_QS100_WakeUp(void)
{
    // 唤醒：高电平脉冲宽度大于 100us，小于 5s
    HAL_GPIO_WritePin(QS100_WAKEUP_GPIO_Port, QS100_WAKEUP_Pin, GPIO_PIN_SET);
    // 延时3秒
    Com_Delay_S(QS100_DELAY_3S);
    // 拉低
    HAL_GPIO_WritePin(QS100_WAKEUP_GPIO_Port, QS100_WAKEUP_Pin, GPIO_PIN_RESET);
}

// 初始化
void Int_QS100_Init(void)
{
    // 唤醒
    Int_QS100_WakeUp();
    // 后开启串口接收中断（循环确保成功）
    // 对于需要获取 AT 命令响应的场景（几乎所有 AT 指令交互都需要），必须在发送命令前确保接收中断已启动，否则会丢失响应数据
    HAL_StatusTypeDef status = HAL_ERROR;
    while (status != HAL_OK)
    {
        status = HAL_UARTEx_ReceiveToIdle_IT(&huart3, QS100_Buff, QS100_Buff_MAX_LEN);
    }
    // 发送AT命令,设备软重启+RB/NRB
    Int_QS100_Send_Cmd("AT+RB\r\n", strlen("AT+RB\r\n"));
}

// 发送命令
void Int_QS100_Send_Cmd(u8 *cmd, u8 len)
{
    // 清空缓冲区与结果容器
    memset(QS100_Buff, 0, QS100_Buff_MAX_LEN);
    QS100_Buff_Len = 0;
    memset(QS100_Result, 0, QS100_RESULT_MAX_LEN);
    QS100_Result_Len = 0;
    // 发送命令
    HAL_UART_Transmit(&huart3, cmd, len, QS100_TIMEOUT_1000);
    // COM_LOG_LN("cmd: %s, %d", cmd, len);
    // 接收响应,最多4次
    short timeout = 4;
    do
    {
        // 等待缓冲区中有数据
        uint32_t uwTick = HAL_GetTick();
        while ((QS100_Buff_Len == 0) && ((HAL_GetTick() - uwTick) < 1000))
        {
        }
        if (QS100_Buff_Len)
        {
            // 拷贝到结果容器
            memcpy(&QS100_Result[QS100_Result_Len], QS100_Buff, (size_t)QS100_Buff_Len);
            QS100_Result_Len += QS100_Buff_Len;
            // 清空缓冲区
            memset(QS100_Buff, 0, QS100_Buff_MAX_LEN);
            QS100_Buff_Len = 0;
        }
        else
        {
            // 超过10秒未收到数据
            break;
        }
    } while (--timeout && (strstr((char *)QS100_Result, "OK") == NULL) && (strstr((char *)QS100_Result, "ERROR") == NULL));

    // 打印响应
    // COM_LOG_LN("QS100: %s,len %d", QS100_Result, QS100_Result_Len);
}

// 获取响应数据长度
void Int_QS100_Get_Size(u16 size)
{

    // 获取响应数据长度
    QS100_Buff_Len = size;
    if (QS100_Buff_Len)
    {
        // 无异常,继续接收
        HAL_StatusTypeDef status = HAL_ERROR;
        while (status != HAL_OK)
        {
            status = HAL_UARTEx_ReceiveToIdle_IT(&huart3, QS100_Buff, QS100_Buff_MAX_LEN);
        }
    }
}

// 判断是否联,AT+CGATT?
QS100_STATUS_T Int_QS100_Is_Connect(void)
{
    Int_QS100_Send_Cmd("AT+CGATT?\r\n", strlen("AT+CGATT?\r\n"));
    if (strstr((char *)QS100_Result, "OK") != NULL)
    {
        return QS100_OK;
    }
    return QS100_ERROR;
}

// 开启Socket服务,AT+NSOCR=STREAM,6,0,0
QS100_STATUS_T Int_QS100_Open_Socket(void)
{
    // 0 表示由模组随机分配,链路号0
    Int_QS100_Send_Cmd("AT+NSOCR=STREAM,6,0,0\r\n", strlen("AT+NSOCR=STREAM,6,0,0\r\n"));
    if (strstr((char *)QS100_Result, "OK") != NULL)
    {
        return QS100_OK;
    }
    return QS100_ERROR;
}

// 连接Socket,开启TCP服务,AT+NSOCO=0,139.224.112.6,10005
QS100_STATUS_T Int_QS100_Connect_Socket(u8 *ip, u16 port)
{
    // 拼接命令
    u8 cmd[QS100_CMD_MAX_LEN_50] = {0};
    sprintf((char *)cmd, "AT+NSOCO=0,%s,%d\r\n", ip, port);
    COM_LOG_LN("cmd: %s", cmd);
    Int_QS100_Send_Cmd(cmd, strlen((char *)cmd));
    if (strstr((char *)QS100_Result, "OK") != NULL)
    {
        return QS100_OK;
    }
    return QS100_ERROR;
}

// 发送数据,AT+NSOSD=0,2,4444
QS100_STATUS_T Int_QS100_Send_Data(u8 *data, u16 len)
{
    // 数据转换为16进制
    u8 hexArr[QS100_HEX_DATA_MAX_LEN_500] = {0};
    for (u16 i = 0; i < len; i++)
    {
        // 拼接16进制,i*2代表每个字节数据对应1个16进制数的2位.
        sprintf((char *)&hexArr[i*2], "%02x", data[i]);
    }
    COM_LOG_LN("hexArr: %s", hexArr);
    // 拼接命令
    u8 cmd[QS100_CMD_MAX_LEN_500] = {0};
    sprintf((char *)cmd, "AT+NSOSD=0,%d,%s\r\n", len, hexArr);
    COM_LOG_LN("cmd: %s", cmd);
    // 发送message
    Int_QS100_Send_Cmd(cmd, strlen((char *)cmd));
    if (strstr((char *)QS100_Result, "OK") != NULL)
    {
        return QS100_OK;
    }
    return QS100_ERROR;
}

// 提供给应用层使用的上传云服务器的函数
QS100_STATUS_T Int_QS100_Upload_Data(u8 *ip, u16 port, u8 *data, u16 len)
{
    // 1. 获取连接状态
    // 循环等待,最多10秒
    short timeout = 11;
    while (Int_QS100_Is_Connect() != QS100_OK && --timeout)
    {
        // COM_LOG_LN("获取连接状态中...");
        Com_Delay_S(1);
    }
    if (timeout <= 0)
    {
        // COM_LOG_LN("获取连接状态超时");
        return QS100_TIMEOUT;
    }
    // 2.开启Socket服务
    timeout = 11;
    while (Int_QS100_Open_Socket() != QS100_OK && --timeout)
    {
        // COM_LOG_LN("开启Socket服务中...");
        Com_Delay_S(1);
    }
    if (timeout <= 0)
    {
        // COM_LOG_LN("开启Socket服务超时");
        return QS100_TIMEOUT;
    }
    // 3.连接Socket
    timeout = 11;
    while (Int_QS100_Connect_Socket(ip, port) != QS100_OK && --timeout)
    {
        // COM_LOG_LN("连接Socket中...");
        Com_Delay_S(1);
    }
    if (timeout <= 0)
    {
        // COM_LOG_LN("连接Socket超时");
        return QS100_TIMEOUT;
    }
    // 4.发送数据,只发一次
    return Int_QS100_Send_Data(data, len);
}
