#ifndef __RF_433_KEY_HANDLE_LIGHT_H__
#define __RF_433_KEY_HANDLE_LIGHT_H__

#include "includes.h"

enum
{
    // Automatic lights-off countdown
    AUTOMATIC_LIGHTS_OFF_COUNTDOWN_NONE = 0,
    AUTOMATIC_LIGHTS_OFF_COUNTDOWN_5_SECONDS = (u32)5  , // 5s， 测试用
    AUTOMATIC_LIGHTS_OFF_COUNTDOWN_5_MIN = (u32)5 * 60  ,
    AUTOMATIC_LIGHTS_OFF_COUNTDOWN_10_MIN = (u32)10 * 60  ,
    AUTOMATIC_LIGHTS_OFF_COUNTDOWN_30_MIN = (u32)30 * 60  ,
    AUTOMATIC_LIGHTS_OFF_COUNTDOWN_60_MIN = (u32)60 * 60  ,
};

typedef struct
{
    u32 automatic_lights_off_cnt;           // 自动关机的倒计时
    u8 flag_is_automatic_lights_off_enable; // 是否使能自动关机
} automatic_lights_off_t;

enum
{
    CUR_LIGHT_MODE_WS2812FX_mode_blink = 0,         // 频闪
    CUR_LIGHT_MODE_WS2812FX_mode_breath,            // 呼吸
    CUR_LIGHT_MODE_WS2812FX_mode_breath_speed,      // 跟随速度参数调节的呼吸
    CUR_LIGHT_MODE_WS2812FX_mode_fire_flicker_soft, // 小风吹火焰闪烁，基于color[0]计算
};

void automatic_lights_off_enable(void);                                         // 开启自动关机，应该设置完倒计时再调用
void automatic_lights_off_disable(void);                                        // 关闭自动关机，应该调用之后，再清除倒计时（也可以不清，只要保证每次开启自动关机的功能前，配置好倒计时）
void automatic_lights_off_set_times(u32 automatic_lights_off_count_down_times); // 设置自动关机的倒计时
void automatic_lights_off_handle_by_timer_isr(void *p);                         // 定时关机的处理函数，由定时器调用，周期：1ms

void light_dynamic_mode_init(void);
void light_dynamic_mode_run(void);
void light_dynamic_mode_switch(u8 dir);

void light_dynamic_speed_add(void);
void light_dynamic_speed_sub(void);

#endif
