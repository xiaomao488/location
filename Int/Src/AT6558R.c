#include "AT6558R.h"
#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "Com_Delay.h"
#include <string.h>
#include <stdio.h>
#include "Com_Debug.h"
#include "usart.h"

// GPS数据缓存
u8 GPS_Buff[AT6558R_GPS_DATA_MAX_LEN] = {0};
u16 GPS_Buff_Len = 0;

// 初始化
void Int_AT6558R_Init(void)
{
    // 使能LDO,给AT6558R提供电源
    HAL_GPIO_WritePin(GPS_EN_GPIO_Port, GPS_EN_Pin, GPIO_PIN_SET);
    // 延时3秒,等待电路稳定
    Com_Delay_S(AT6558R_DELAY_3S);
    // 设置串口通信波特率9600
    Int_AT6558R_Send_Cmd("PCAS01,1", strlen("PCAS01,1"));
    // 设置定位更新率 1000=更新率为 1Hz
    Int_AT6558R_Send_Cmd("PCAS02,1000", strlen("PCAS02,1000"));
    // 配置工作系统 $PCAS04,3*1A 北斗和 GPS 双模
    Int_AT6558R_Send_Cmd("PCAS04,3", strlen("PCAS04,3"));
    // 开启串口接收中断
    HAL_StatusTypeDef status = HAL_ERROR;
    while (status != HAL_OK)
    {   
        status = HAL_UARTEx_ReceiveToIdle_IT(&huart2, GPS_Buff, AT6558R_GPS_DATA_MAX_LEN);
    }
}

// 发送命令
void Int_AT6558R_Send_Cmd(u8 *cmd, u8 len)
{
    // 计算校验和
    u8 xor_result = cmd[0];
    for (u8 i = 1; i < len; i++)
    {
        xor_result ^= cmd[i];
    }
    // 拼接命令
    u8 cmd_arr[AT6558R_CMD_MAX_LEN] = {0};
    sprintf((char *)cmd_arr, "$%s*%02X\r\n", cmd, xor_result);
    // COM_LOG_LN("cmd_arr:%s", cmd_arr);
    // 发送命令
    HAL_UART_Transmit(&huart2, cmd_arr, strlen((char *)cmd_arr), AT6558R_TIMEOUT_1000);
}

// 接收GPS数据长度,实则处理
void Int_AT6558R_Get_Size(u16 size)
{
    // 接收的GPS数据长度
    GPS_Buff_Len = size;
    // 判断此次中断有无异常
    if (GPS_Buff_Len > 0)
    {
        // 无异常,继续接收
        HAL_StatusTypeDef status = HAL_ERROR;
        while (status != HAL_OK)
        {
            status = HAL_UARTEx_ReceiveToIdle_IT(&huart2, GPS_Buff, AT6558R_GPS_DATA_MAX_LEN);
        }
    }
}

// 给应用层提供GPS数据和长度
void Int_AT6558R_Get_GPS_Data(u8 *data, u16 *len)
{
    // 清理data与len
    memset(data, 0, (size_t)*len);
    *len = 0;

    if (GPS_Buff_Len)
    {
        // 从缓冲区拷贝数据给data与len
        memcpy(data, GPS_Buff, (size_t)GPS_Buff_Len);
        *len = GPS_Buff_Len;

        // 清理GPS数据缓存
        memset(GPS_Buff, 0, (size_t)GPS_Buff_Len);
        GPS_Buff_Len = 0;
    }
}
