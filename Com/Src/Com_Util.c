#include "Com_Util.h"
#include "time.h"
#include "string.h"
#include <stdio.h>
#include "Com_Debug.h"

// ����ʱ��
// APP_DATE_TIME_T utc_date;

// utcʱ��תutc8ʱ��
void Com_Util_UTC_To_UTC8(char *utc, char *utc8)
{
    struct tm utc_date;
    // 2024-06-02 07:08:22
    sscanf((char *)utc, "%04d-%02d-%02d %02d:%02d:%02d",
           &utc_date.tm_year, &utc_date.tm_mon, &utc_date.tm_mday, &utc_date.tm_hour, &utc_date.tm_min, &utc_date.tm_sec);
    utc_date.tm_year -= 1900;
    utc_date.tm_mon -= 1;
    // ��ȡutcʱ���
    time_t current_tm = mktime(&utc_date);
    // ��ȡutc8ʱ���
    current_tm += 8 * 60 * 60;
    // ʱ�����ʽ��
    struct tm *utc8_tm = localtime(&current_tm);
    // ת��Ϊ�ַ���yyyy-mm-dd hh:mm:ss
    sprintf(utc8, "%04d-%02d-%02d %02d:%02d:%02d",
            utc8_tm->tm_year + 1900, utc8_tm->tm_mon + 1, utc8_tm->tm_mday, utc8_tm->tm_hour, utc8_tm->tm_min, utc8_tm->tm_sec);
    // COM_LOG_LN("utc8: %s", utc8);
}

// γ��,���� ��ת��:4006.81888,N,11621.89413,E
float Com_Util_DMS_To_Degree(float dms)
{
    return ((int)dms / 100) + (dms - ((int)dms / 100) * 100) / 60.0;
}

/**
 * @brief ������ת��Ϊ�ַ���
 *
 * @param direction ���������ϡ���
 * @param dir N 0,E 1,S 2,W 3
 */
static void Com_Util_Direction_To_String(char dir, char *dir_str)
{
    switch (dir)
    {
    case 'N':
        strcpy(dir_str, "��");
        break;
    case 'E':
        strcpy(dir_str, "��");
        break;
    case 'S':
        strcpy(dir_str, "��");
        break;
    case 'W':
        strcpy(dir_str, "��");
        break;
    }
}

// JSON��ӡ
void Com_Util_JSON_Print(APP_JSON_T json)
{
    char temp[10] = {0};
    COM_LOG_LN("����: %s ", json.date);
    COM_LOG_LN("γ��: %f ", json.lat);
    Com_Util_Direction_To_String(json.lat_dir[0], temp);
    COM_LOG_LN("γ�ȷ���: %s ", temp);
    COM_LOG_LN("����: %f ", json.lon);
    Com_Util_Direction_To_String(json.lon_dir[0], temp);
    COM_LOG_LN("���ȷ���: %s ", temp);
    COM_LOG_LN("�ٶ�: %f km/h", json.speed * 1.852);
    COM_LOG_LN("����: %ld �� ", json.step);
}
