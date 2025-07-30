// encoding UTF-8
// 433 遥控控制灯光的源程序
#include "rf_433_key_handle_light.h"

#include "led_strip_sys.h"
#include "WS2812FX.H"
#include "ws2812fx_effect.h"
#include "led_strand_effect.h"

// static u8 flag_is_light_in_static_mode = 0; 
static u8 cur_light_dynamic_mode = CUR_LIGHT_MODE_WS2812FX_mode_blink; // 存放当前的动态模式
mode_ptr light_mode_fun_ptr = NULL; // 存放动态模式要调用的动画效果

#if 0
void light_set_brightness(u8 brightness)
{
    if (flag_is_light_in_static_mode)
    {
        // 静态模式下，设置亮度
        light_set_brightness(brightness);
        return;
    
    }

    WS2812FX_setBrightness(brightness);
}
#endif

// 动态模式下，初始化，如果开机是动态模式，才调用
void light_dynamic_mode_init(void)
{
    // 根据flash中存放的数据读取出动态模式
    if (CUR_LIGHT_MODE_WS2812FX_mode_blink == fc_effect.cur_light_dynamic_mode)
    {
        // 频闪
        cur_light_dynamic_mode = CUR_LIGHT_MODE_WS2812FX_mode_blink;
    }
    else if (CUR_LIGHT_MODE_WS2812FX_mode_breath == fc_effect.cur_light_dynamic_mode)
    {
        // 呼吸
        cur_light_dynamic_mode = CUR_LIGHT_MODE_WS2812FX_mode_breath;
    }
    else if (CUR_LIGHT_MODE_WS2812FX_mode_breath_speed == fc_effect.cur_light_dynamic_mode)
    {
        // 带有停顿效果的呼吸
        cur_light_dynamic_mode = CUR_LIGHT_MODE_WS2812FX_mode_breath_speed;
    }
    else // CUR_LIGHT_MODE_WS2812FX_mode_fire_flicker_soft == fc_effect.cur_light_dynamic_mode
    {
        // 烛光模式， 亮度像蜡烛一样漂浮不定
        cur_light_dynamic_mode = CUR_LIGHT_MODE_WS2812FX_mode_fire_flicker_soft;
    }
}

// 运行动态模式
void light_dynamic_mode_run(void)
{
    if (CUR_LIGHT_MODE_WS2812FX_mode_blink == cur_light_dynamic_mode)
    {
        // 频闪
        light_mode_fun_ptr = WS2812FX_mode_blink;
    }
    else if (CUR_LIGHT_MODE_WS2812FX_mode_breath == cur_light_dynamic_mode)
    {
        // 呼吸
        light_mode_fun_ptr = WS2812FX_mode_breath;
    }
    else if (CUR_LIGHT_MODE_WS2812FX_mode_breath_speed == cur_light_dynamic_mode)
    {
        // light_mode_fun_ptr = WS2812FX_mode_breath_speed;
        light_mode_fun_ptr = WS2812FX_mode_breath;
    }
    else
    {
        light_mode_fun_ptr = WS2812FX_mode_fire_flicker_soft;
    }

    // fc_effect.Now_state = IS_light_scene; // 灯光模式从静态切换为动态

    WS2812FX_stop(); // 停止当前灯光效果
    WS2812FX_setSegment_colorOptions(
        0,                     // 第0段
        0,                     // 起始位置
        0,                      // 结束位置
        light_mode_fun_ptr,    // 效果
        WHITE,                 // 颜色
        fc_effect.cur_light_dynamic_speed,    // 速度 
        NO_OPTIONS);           // 选项，这里像素点大小：3 REVERSE决定方向
    WS2812FX_start();
}

void light_dynamic_speed_add(void)
{

}

void light_dynamic_speed_sub(void)
{

}


/**
 * @brief 修改灯光动态模式
 *
 * @param dir  0--模式减，1--模式加

 */
void light_dynamic_mode_switch(u8 dir)
{
    // static u8 cur_light_dynamic_mode = CUR_LIGHT_MODE_WS2812FX_mode_blink;

    fc_effect.Now_state = IS_DYNAMIC_EFFECT;

    if (dir)
    {
        if (cur_light_dynamic_mode < CUR_LIGHT_MODE_WS2812FX_mode_fire_flicker_soft)
        {
            cur_light_dynamic_mode++;
        }
        else
        {
            cur_light_dynamic_mode = CUR_LIGHT_MODE_WS2812FX_mode_blink;
        }
    }
    else
    {
        if (cur_light_dynamic_mode > 0)
        {
            cur_light_dynamic_mode--;
        }
        else
        {
            cur_light_dynamic_mode = CUR_LIGHT_MODE_WS2812FX_mode_fire_flicker_soft;
        }
    }

    light_dynamic_mode_run();
}
