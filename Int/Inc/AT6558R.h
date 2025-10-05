#ifndef __AT6558R_H__
#define __AT6558R_H__

#include "Com_Util.h"

// 初始化
void Int_AT6558R_Init(void);

// 发送命令
void Int_AT6558R_Send_Cmd(u8 *cmd, u8 len);

// 给应用层提供GPS数据和长度
void Int_AT6558R_Get_GPS_Data(u8 *data, u16 *len);

#endif /* __AT6558R_H__ */
