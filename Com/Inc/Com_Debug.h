#ifndef __COM_DEBUG_H__
#define __COM_DEBUG_H__

#include <stdio.h>
#include <string.h>
#include "Com_Config.h"

// 跨平台换行符定义
#ifdef _WIN32
#define NEW_LINE "\r\n"
#else
#define NEW_LINE "\n"
#endif

// 提取文件名（兼容Windows和类Unix路径）
#define FILE_NAME \
    (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1) : \
    (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__))

 
// 日志宏定义（开发环境启用，生产环境禁用）
#ifdef DEV
#define COM_LOG(args, ...) printf("[%s:%d]" args, FILE_NAME, __LINE__, ##__VA_ARGS__)
#define COM_LOG_LN(args, ...) printf("[%s:%d]" args NEW_LINE, FILE_NAME, __LINE__, ##__VA_ARGS__)
#else
#define COM_LOG(args, ...) 
#define COM_LOG_LN(args, ...) 
#endif

#endif /* __COM_DEBUG_H__ */
