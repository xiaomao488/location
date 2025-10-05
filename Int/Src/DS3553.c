#include "DS3553.h"
#include "i2c.h"
#include "gpio.h"
#include "Com_delay.h"

// ��ʼ������
void Int_DS3553_Init(void)
{
    // cs����
    HAL_GPIO_WritePin(DS3553_CS_GPIO_Port, DS3553_CS_Pin, GPIO_PIN_RESET);
    // ��ʱ3ms����
    Com_Delay_ms(DS3553_DELAY_3MS);
    // 0����ֹ�Ʋ��������
    Int_DS3553_Write_Data(DS3553_USER_SET, DS3553_RESET);
    // cs����
    HAL_GPIO_WritePin(DS3553_CS_GPIO_Port, DS3553_CS_Pin, GPIO_PIN_SET);
    // ��ʱ10ms����
    Com_Delay_ms(DS3553_DELAY_10MS);
}

// ���üĴ�����ĳһλ��ֵ
void Int_DS3553_Write_Data(u8 bit, DS3553_STATUS_T status)
{
    // �ȶ�ԭ����ֵ
    u8 data = Int_DS3553_Read_Data(DS3553_USER_SET);
    if (status == DS3553_SET)
    {
        data |= bit;
    }
    else
    {
        data &= ~bit;
    }
    // ��д��
    HAL_I2C_Mem_Write(&hi2c1, DS3553_WRITE_ADDR, DS3553_USER_SET, I2C_MEMADD_SIZE_8BIT, &data, 1, DS3553_TIME_OUT_1000);
}

// ��ȡ�Ĵ�����ĳλֵ
u8 Int_DS3553_Read_Data(u8 addr)
{
    u8 data = 0;
    // cs����
    HAL_GPIO_WritePin(DS3553_CS_GPIO_Port, DS3553_CS_Pin, GPIO_PIN_RESET);
    // ��ʱ3ms����
    Com_Delay_ms(DS3553_DELAY_3MS);
    HAL_I2C_Mem_Read(&hi2c1, DS3553_READ_ADDR, addr, I2C_MEMADD_SIZE_8BIT, &data, 1, DS3553_TIME_OUT_1000);
    // cs����
    HAL_GPIO_WritePin(DS3553_CS_GPIO_Port, DS3553_CS_Pin, GPIO_PIN_SET);
    // ��ʱ10ms����
    Com_Delay_ms(DS3553_DELAY_10MS);
    return data;
}

// ��ȡ����
u32 Int_DS3553_Get_Step(void)
{
    return (u32)(Int_DS3553_Read_Data(DS3553_STEP_CNT_L) << 0) +
           (Int_DS3553_Read_Data(DS3553_STEP_CNT_M) << 8) +
           (Int_DS3553_Read_Data(DS3553_STEP_CNT_H) << 16);
}
