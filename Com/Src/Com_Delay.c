#include "Com_Delay.h"

// 毫秒延时
void Com_Delay_ms(uint32_t ms)
{
    while (ms--)
    {
        HAL_Delay(1);
    }
}

// 秒延时
void Com_Delay_S(uint32_t s)
{
    while (s--)
    {
        HAL_Delay(1000);
    }
}
