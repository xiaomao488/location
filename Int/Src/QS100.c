#include "QS100.h"
#include "gpio.h"
#include "stm32f1xx_hal.h"
#include "Com_Delay.h"
#include <string.h>
#include "usart.h"
#include "Com_Debug.h"

// ������
u8 QS100_Buff[QS100_Buff_MAX_LEN] = {0};
u16 QS100_Buff_Len = 0;
// ���صĽ��
u8 QS100_Result[QS100_RESULT_MAX_LEN] = {0};
u16 QS100_Result_Len = 0;

// ����
void Int_QS100_WakeUp(void)
{
    // ���ѣ��ߵ�ƽ�����ȴ��� 100us��С�� 5s
    HAL_GPIO_WritePin(QS100_WAKEUP_GPIO_Port, QS100_WAKEUP_Pin, GPIO_PIN_SET);
    // ��ʱ3��
    Com_Delay_S(QS100_DELAY_3S);
    // ����
    HAL_GPIO_WritePin(QS100_WAKEUP_GPIO_Port, QS100_WAKEUP_Pin, GPIO_PIN_RESET);
}

// ��ʼ��
void Int_QS100_Init(void)
{
    // ����
    Int_QS100_WakeUp();
    // �������ڽ����жϣ�ѭ��ȷ���ɹ���
    // ������Ҫ��ȡ AT ������Ӧ�ĳ������������� AT ָ�������Ҫ���������ڷ�������ǰȷ�������ж�������������ᶪʧ��Ӧ����
    HAL_StatusTypeDef status = HAL_ERROR;
    while (status != HAL_OK)
    {
        status = HAL_UARTEx_ReceiveToIdle_IT(&huart3, QS100_Buff, QS100_Buff_MAX_LEN);
    }
    // ����AT����,�豸������+RB/NRB
    Int_QS100_Send_Cmd("AT+RB\r\n", strlen("AT+RB\r\n"));
}

// ��������
void Int_QS100_Send_Cmd(u8 *cmd, u8 len)
{
    // ��ջ�������������
    memset(QS100_Buff, 0, QS100_Buff_MAX_LEN);
    QS100_Buff_Len = 0;
    memset(QS100_Result, 0, QS100_RESULT_MAX_LEN);
    QS100_Result_Len = 0;
    // ��������
    HAL_UART_Transmit(&huart3, cmd, len, QS100_TIMEOUT_1000);
    // COM_LOG_LN("cmd: %s, %d", cmd, len);
    // ������Ӧ,���4��
    short timeout = 4;
    do
    {
        // �ȴ���������������
        uint32_t uwTick = HAL_GetTick();
        while ((QS100_Buff_Len == 0) && ((HAL_GetTick() - uwTick) < 1000))
        {
        }
        if (QS100_Buff_Len)
        {
            // �������������
            memcpy(&QS100_Result[QS100_Result_Len], QS100_Buff, (size_t)QS100_Buff_Len);
            QS100_Result_Len += QS100_Buff_Len;
            // ��ջ�����
            memset(QS100_Buff, 0, QS100_Buff_MAX_LEN);
            QS100_Buff_Len = 0;
        }
        else
        {
            // ����10��δ�յ�����
            break;
        }
    } while (--timeout && (strstr((char *)QS100_Result, "OK") == NULL) && (strstr((char *)QS100_Result, "ERROR") == NULL));

    // ��ӡ��Ӧ
    // COM_LOG_LN("QS100: %s,len %d", QS100_Result, QS100_Result_Len);
}

// ��ȡ��Ӧ���ݳ���
void Int_QS100_Get_Size(u16 size)
{

    // ��ȡ��Ӧ���ݳ���
    QS100_Buff_Len = size;
    if (QS100_Buff_Len)
    {
        // ���쳣,��������
        HAL_StatusTypeDef status = HAL_ERROR;
        while (status != HAL_OK)
        {
            status = HAL_UARTEx_ReceiveToIdle_IT(&huart3, QS100_Buff, QS100_Buff_MAX_LEN);
        }
    }
}

// �ж��Ƿ���,AT+CGATT?
QS100_STATUS_T Int_QS100_Is_Connect(void)
{
    Int_QS100_Send_Cmd("AT+CGATT?\r\n", strlen("AT+CGATT?\r\n"));
    if (strstr((char *)QS100_Result, "OK") != NULL)
    {
        return QS100_OK;
    }
    return QS100_ERROR;
}

// ����Socket����,AT+NSOCR=STREAM,6,0,0
QS100_STATUS_T Int_QS100_Open_Socket(void)
{
    // 0 ��ʾ��ģ���������,��·��0
    Int_QS100_Send_Cmd("AT+NSOCR=STREAM,6,0,0\r\n", strlen("AT+NSOCR=STREAM,6,0,0\r\n"));
    if (strstr((char *)QS100_Result, "OK") != NULL)
    {
        return QS100_OK;
    }
    return QS100_ERROR;
}

// ����Socket,����TCP����,AT+NSOCO=0,139.224.112.6,10005
QS100_STATUS_T Int_QS100_Connect_Socket(u8 *ip, u16 port)
{
    // ƴ������
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

// ��������,AT+NSOSD=0,2,4444
QS100_STATUS_T Int_QS100_Send_Data(u8 *data, u16 len)
{
    // ����ת��Ϊ16����
    u8 hexArr[QS100_HEX_DATA_MAX_LEN_500] = {0};
    for (u16 i = 0; i < len; i++)
    {
        // ƴ��16����,i*2����ÿ���ֽ����ݶ�Ӧ1��16��������2λ.
        sprintf((char *)&hexArr[i*2], "%02x", data[i]);
    }
    COM_LOG_LN("hexArr: %s", hexArr);
    // ƴ������
    u8 cmd[QS100_CMD_MAX_LEN_500] = {0};
    sprintf((char *)cmd, "AT+NSOSD=0,%d,%s\r\n", len, hexArr);
    COM_LOG_LN("cmd: %s", cmd);
    // ����message
    Int_QS100_Send_Cmd(cmd, strlen((char *)cmd));
    if (strstr((char *)QS100_Result, "OK") != NULL)
    {
        return QS100_OK;
    }
    return QS100_ERROR;
}

// �ṩ��Ӧ�ò�ʹ�õ��ϴ��Ʒ������ĺ���
QS100_STATUS_T Int_QS100_Upload_Data(u8 *ip, u16 port, u8 *data, u16 len)
{
    // 1. ��ȡ����״̬
    // ѭ���ȴ�,���10��
    short timeout = 11;
    while (Int_QS100_Is_Connect() != QS100_OK && --timeout)
    {
        // COM_LOG_LN("��ȡ����״̬��...");
        Com_Delay_S(1);
    }
    if (timeout <= 0)
    {
        // COM_LOG_LN("��ȡ����״̬��ʱ");
        return QS100_TIMEOUT;
    }
    // 2.����Socket����
    timeout = 11;
    while (Int_QS100_Open_Socket() != QS100_OK && --timeout)
    {
        // COM_LOG_LN("����Socket������...");
        Com_Delay_S(1);
    }
    if (timeout <= 0)
    {
        // COM_LOG_LN("����Socket����ʱ");
        return QS100_TIMEOUT;
    }
    // 3.����Socket
    timeout = 11;
    while (Int_QS100_Connect_Socket(ip, port) != QS100_OK && --timeout)
    {
        // COM_LOG_LN("����Socket��...");
        Com_Delay_S(1);
    }
    if (timeout <= 0)
    {
        // COM_LOG_LN("����Socket��ʱ");
        return QS100_TIMEOUT;
    }
    // 4.��������,ֻ��һ��
    return Int_QS100_Send_Data(data, len);
}
