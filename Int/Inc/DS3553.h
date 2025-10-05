#ifndef __DS3553_H__
#define __DS3553_H__

#include "Com_Util.h"

// 初始化
void Int_DS3553_Init(void);

// 设置寄存器中某一位的值
void Int_DS3553_Write_Data(u8 bit, DS3553_STATUS_T status);

// 获取寄存器中某位值
u8 Int_DS3553_Read_Data(u8 addr);

// 获取步数
u32 Int_DS3553_Get_Step(void);

#endif /* __DS3553_H__ */
