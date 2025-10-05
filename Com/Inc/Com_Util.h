#ifndef __COM_UTIL_H__
#define __COM_UTIL_H__

#include <stdint.h>
#include <string.h>

/*** 宏定义 */

// Com公共模块
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

// DS3553-计步模块
// 地址
#define DS3553_READ_ADDR 0x4F
#define DS3553_WRITE_ADDR 0x4E
#define DS3553_CHIP_ID 0x01
#define DS3553_USER_SET 0xC3
#define DS3553_STEP_CNT_L 0xC4
#define DS3553_STEP_CNT_M 0xC5
#define DS3553_STEP_CNT_H 0xC6
// 时间
#define DS3553_DELAY_3MS 3
#define DS3553_DELAY_10MS 10
#define DS3553_TIME_OUT_1000 1000
// USER_SET寄存器
#define DS3553_USER_SET_PWR_MOD (1 << 7)
#define DS3553_USER_SET_SEN_DIS (1 << 6)
#define DS3553_USER_SET_RAISE_EN (1 << 5)
#define DS3553_USER_SET_PULSE_EN (1 << 4)
#define DS3553_USER_SET_NOISE_DIS (1 << 3)
#define DS3553_USER_SET_CLEAR_EN (1 << 2)
#define DS3553_USER_SET_PEDO_1 (1 << 1)
#define DS3553_USER_SET_PEDO_0 (1 << 0)
// 枚举
typedef enum
{
    DS3553_RESET = 0,
    DS3553_SET = 1,
} DS3553_STATUS_T;

// AT6558R-定位模块
// 长度
#define AT6558R_CMD_MAX_LEN 50
#define AT6558R_GPS_DATA_MAX_LEN 1024
// 时间
#define AT6558R_DELAY_3S 5
#define AT6558R_TIMEOUT_1000 1000

// QS100-NB-IOT模块
#define QS100_DELAY_3S 3
#define QS100_TIMEOUT_1000 1000
#define QS100_Buff_MAX_LEN 256
#define QS100_RESULT_MAX_LEN 256
#define QS100_CMD_MAX_LEN_50 50
#define QS100_CMD_MAX_LEN_500 500
#define QS100_HEX_DATA_MAX_LEN_500 500
// 枚举
typedef enum
{
    QS100_OK = 0,
    QS100_ERROR = 1,
    QS100_TIMEOUT = 2,
    QS100_BUSY = 3,
} QS100_STATUS_T;

// App-应用层
#define APP_GPS_DATA_MAX_LEN 1024
// 上传云服务器的结构体
typedef struct
{
    u8 date[20];   // yyyy-mm-dd hh:mm:ss
    float lat;     // 纬度
    u8 lat_dir[2]; // 纬度方向
    float lon;     // 经度
    u8 lon_dir[2]; // 经度方向
    float speed;   // 速度
    u32 step;      // 步数
} APP_JSON_T;
// 日期时间结构体
typedef struct
{
    int year;   // 年
    int month;  // 月
    int day;    // 日
    int hour;   // 时
    int minute; // 分
    int second; // 秒
} APP_DATE_TIME_T;

/*** 函数原型 */
// utc转本地时间
void Com_Util_UTC_To_UTC8(char *utc, char *utc8);
// 纬度,经度 分转度
float Com_Util_DMS_To_Degree(float dms);
// JSON打印
void Com_Util_JSON_Print(APP_JSON_T json);

#endif /* __COM_UTIL_H__ */
