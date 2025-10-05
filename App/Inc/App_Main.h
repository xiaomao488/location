#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__

#include "Com_Util.h"

// 初始化
void App_Main_Init(void);

// 运行
void App_Main_Run(void);

// 组装GPS数据
static void App_Main_Assamble_GPS_JSON(u8 *data);

// 生成GPS假数据
static void App_Test_Generate_GPS_Data(void);

#endif /* __APP_MAIN_H__ */
