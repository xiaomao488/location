#include "App_Main.h"
#include "DS3553.h"
#include "AT6558R.h"
#include "QS100.h"
#include <stdio.h>
#include <string.h>
#include "Com_Delay.h"
#include "Com_Debug.h"
#include "Com_Config.h"

// GPS��������
u8 GPS_Data[APP_GPS_DATA_MAX_LEN] = {0};
u16 GPS_Data_Len = 0;
// �ϴ�Json
APP_JSON_T json;

// ��ʼ��
void App_Main_Init(void)
{
    Int_DS3553_Init();
    Int_AT6558R_Init();
    Int_QS100_Init();
}

// ����
void App_Main_Run(void)
{
    while (1)
    {
        // 1.��ȡGPS����
        Int_AT6558R_Get_GPS_Data(GPS_Data, &GPS_Data_Len);
        if (GPS_Data_Len)
        {
            // ����GPS����,����GPS����
            App_Main_Assamble_GPS_JSON(GPS_Data);
        }
        // 2.��ȡ����
        json.step = Int_DS3553_Get_Step();
        
        // ��ӡJSON����
        Com_Util_JSON_Print(json);

        // 3.תJSON

        // 4.�ϴ��ƶ�
        
        // 5.��ʱ2��
        Com_Delay_S(2);
    }
}

/**
 * @brief ��װGPS����
 */
static void App_Main_Assamble_GPS_JSON(u8 *data)
{
    // GPS���ݿ���,1-ʹ�ü�����,0-ʹ����ʵ����
    if (APP_USE_REAL_GPS_DATA)
    {
        App_Test_Generate_GPS_Data();
    }
    // �����߼�
    char *gnrmc = NULL;
    gnrmc = strstr((char *)data, "$GNRMC");
    char flag = 0;
    // $GNRMC,070822.000,A,4006.81888,N,11621.89413,E,0.81,359.02,020624,,,A,V*02
    // ��ʽ��$GNRMC,ʱ��,״̬,...  ����ʱ���ֶΣ���ȡ״̬
    sscanf(gnrmc, "$GNRMC,%*[^,],%c", &flag);
    if (flag == 'A')
    {
        char date[7] = {0};
        char time[7] = {0};
        sscanf(gnrmc, "$GNRMC,%6c%*6c,%f,%c,%f,%c,%f,%*f,%6c",
               time, &json.lat, json.lat_dir, &json.lon, json.lon_dir, &json.speed, date);
        // ʱ��:2024-06-02 07:08:22
        sprintf((char *)json.date, "20%c%c-%c%c-%c%c %c%c:%c%c:%c%c",
                date[4], date[5], date[2], date[3], date[0], date[1], time[0], time[1], time[2], time[3], time[4], time[5]);

        // ��β���'\0'
        json.date[19] = '\0';
        json.lat_dir[1] = '\0';
        json.lon_dir[1] = '\0';

        // ת��ΪUTC8ʱ��
        Com_Util_UTC_To_UTC8((char *)json.date, (char *)json.date);
        // γ��,���� ��ת��
        json.lat = Com_Util_DMS_To_Degree(json.lat);
        json.lon = Com_Util_DMS_To_Degree(json.lon);
    }
}

/**
 * @brief ����GPS������
 *
 */
static void App_Test_Generate_GPS_Data(void)
{
    // ģ������ - �޸�Ϊ�ַ���ָ������
    const char *gpsNmeaData[] = {
        "$GNGGA,070822.000,4006.81888,N,11621.89413,E,1,05,25.5,30.2,M,-9.6,M,,*58",
        "$GNGLL,4006.81888,N,11621.89413,E,070822.000,A,A*49",
        "$GNGSA,A,3,05,12,25,,,,,,,,,,25.5,25.5,25.5,1*02",
        "$GNGSA,A,3,10,13,,,,,,,,,,,25.5,25.5,25.5,4*05",
        "$GPGSV,3,1,11,05,54,263,24,06,32,098,,09,20,043,,11,68,061,,0*61",
        "$GPGSV,3,2,11,12,14,237,17,13,23,176,,15,05,204,,19,08,152,,0*60",
        "$GPGSV,3,3,11,20,75,327,,25,13,265,23,29,28,314,,0*51",
        "$BDGSV,2,1,05,08,64,301,,10,47,216,15,13,56,283,10,27,59,133,,0*78",
        "$BDGSV,2,2,05,38,72,326,,0*48",
        "$GNRMC,070822.000,A,4006.81888,N,11621.89413,E,0.81,359.02,051025,,,A,V*02",
        "$GNVTG,359.02,T,,M,0.81,N,1.51,K,A*22",
        "$GNZDA,070822.000,02,06,2024,00,00*47",
        "$GPTXT,01,01,01,ANTENNA OPEN*25", // ����������
        NULL                               // ��ӿ�ָ����Ϊ���������־
    };
    // ���һ��App_GPS_Data����
    memset((char *)GPS_Data, 0, sizeof(GPS_Data));
    // ��������ʱ��Ҫ�������ÿ���ַ���
    // ����App_GPS_Data��һ���㹻����ַ����黺����
    size_t offset = 0;
    for (int i = 0; gpsNmeaData[i] != NULL; i++)
    {
        size_t len = strlen(gpsNmeaData[i]);
        // ȷ�����ᳬ��Ŀ�껺������С
        if ((offset + len) < sizeof(GPS_Data))
        {
            memcpy(&GPS_Data[offset], gpsNmeaData[i], len);
            offset += len;
            // ������ӻ��з��ָ���ͬ��NMEA����
            if (offset + 1 < sizeof(GPS_Data))
            {
                GPS_Data[offset++] = '\n';
            }
        }
        else
        {
            break; // ����������
        }
    }
}
