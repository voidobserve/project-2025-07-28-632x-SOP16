#ifndef __RF_433_KEY_HANDLE_LIGHT_H__
#define __RF_433_KEY_HANDLE_LIGHT_H__

#include "includes.h"
 

enum
{
    CUR_LIGHT_MODE_WS2812FX_mode_blink = 0,         // 频闪
    CUR_LIGHT_MODE_WS2812FX_mode_breath,            // 呼吸
    CUR_LIGHT_MODE_WS2812FX_mode_breath_speed,      // 跟随速度参数调节的呼吸
    CUR_LIGHT_MODE_WS2812FX_mode_fire_flicker_soft, // 小风吹火焰闪烁，基于color[0]计算
};

void light_dynamic_mode_init(void);
void light_dynamic_mode_run(void);
void light_dynamic_mode_switch(u8 dir);

#endif 

