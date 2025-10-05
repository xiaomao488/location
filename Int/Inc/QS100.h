#ifndef __QS100_H__
#define __QS100_H__

#include "Com_Util.h"

// 唤醒
void Int_QS100_WakeUp(void);

// 初始化
void Int_QS100_Init(void);

// 发送命令
void Int_QS100_Send_Cmd(u8 *cmd, u8 len);

// 判断是否联网
QS100_STATUS_T Int_QS100_Is_Connect(void);

// 开启Socket服务
QS100_STATUS_T Int_QS100_Open_Socket(void);

// 连接Socket,开启TCP服务
QS100_STATUS_T Int_QS100_Connect_Socket(u8 *ip, u16 port);

// 发送数据
QS100_STATUS_T Int_QS100_Send_Data(u8 *data, u16 len);

// 提供给应用层使用的上传云服务器的函数
QS100_STATUS_T Int_QS100_Upload_Data(u8 *ip, u16 port, u8 *data, u16 len);

#endif /* __QS100_H__ */
