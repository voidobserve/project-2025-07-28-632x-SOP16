// encoding UTF-8
// 433 遥控控制灯光的源程序
#include "rf_433_key_handle_light.h"

#include "led_strip_sys.h"
#include "WS2812FX.H"
#include "ws2812fx_effect.h"
#include "led_strand_effect.h"

// static u8 flag_is_light_in_static_mode = 0;
static u8 cur_light_dynamic_mode = CUR_LIGHT_MODE_WS2812FX_mode_blink; // 存放当前的动态模式
static mode_ptr light_mode_fun_ptr = NULL;                             // 存放动态模式要调用的动画效果

static volatile automatic_lights_off_t automatic_lights_off_structure = {0};

// 开启自动关机，应该设置完倒计时再调用
void automatic_lights_off_enable(void)
{
    automatic_lights_off_structure.flag_is_automatic_lights_off_enable = 1;
}

// 关闭自动关机，应该调用之后，再清除倒计时（也可以不清，只要保证每次开启自动关机的功能前，配置好倒计时）
void automatic_lights_off_disable(void)
{
    automatic_lights_off_structure.flag_is_automatic_lights_off_enable = 0;
}

// 设置自动关机的倒计时
void automatic_lights_off_set_times(u32 automatic_lights_off_count_down_times)
{
    automatic_lights_off_structure.automatic_lights_off_cnt = automatic_lights_off_count_down_times;
}

// 定时关机的处理函数，由定时器调用，周期：1ms
void automatic_lights_off_handle_by_timer_isr(void *p)
{
    if (automatic_lights_off_structure.flag_is_automatic_lights_off_enable)
    {
        if (automatic_lights_off_structure.automatic_lights_off_cnt > 0)
        {
            automatic_lights_off_structure.automatic_lights_off_cnt--;
        }

        if (automatic_lights_off_structure.automatic_lights_off_cnt == 0)
        {
            automatic_lights_off_disable();
            soft_turn_off_lights();
        }
    }
}

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
    u16 speed = 0; // 存放要传入到函数 WS2812FX_setSegment_colorOptions() 的速度值

    Adafruit_NeoPixel_clear(); // 清空缓存残留
    WS2812FX_show();

    if (CUR_LIGHT_MODE_WS2812FX_mode_blink == cur_light_dynamic_mode)
    {
        // 频闪
        light_mode_fun_ptr = WS2812FX_mode_blink;
        speed = (u16)100 * 2 + 100 * 2 * fc_effect.cur_light_dynamic_speed;
    }
    else if (CUR_LIGHT_MODE_WS2812FX_mode_breath == cur_light_dynamic_mode)
    {
        // 呼吸
        light_mode_fun_ptr = WS2812FX_mode_breath_continue;
        speed = (u16)fc_effect.cur_light_dynamic_speed;
    }
    else if (CUR_LIGHT_MODE_WS2812FX_mode_breath_speed == cur_light_dynamic_mode)
    {
        // 带停顿的呼吸
        light_mode_fun_ptr = WS2812FX_mode_breath_with_pause;
        speed = (u16)fc_effect.cur_light_dynamic_speed;
    }
    else
    {
        // 由速度值控制的烛光效果
        light_mode_fun_ptr = WS2812FX_mode_fire_flicker_ctl_by_speed;
        speed = (u16)fc_effect.cur_light_dynamic_speed + 1; // 烛光效果，速度值不能为0
    }

    WS2812FX_stop(); // 停止当前灯光效果
    WS2812FX_setSegment_colorOptions(
        0,                  // 第0段
        0,                  // 起始位置
        0,                  // 结束位置
        light_mode_fun_ptr, // 效果
        WHITE,              // 颜色
        speed,              // 速度
        NO_OPTIONS);        // 选项，这里像素点大小：3 REVERSE决定方向
    WS2812FX_start();
}

void light_dynamic_speed_add(void)
{
    if (fc_effect.Now_state != IS_DYNAMIC_EFFECT)
    {
        // 不处于动态效果，直接返回
        return;
    }

    if (fc_effect.on_off_flag != DEVICE_ON)
    {
        return;
    }

    // 0~9 ， 0--快，9--慢
    if (fc_effect.cur_light_dynamic_speed > 0)
    {
        fc_effect.cur_light_dynamic_speed--;
    }

    light_dynamic_mode_run();
}

void light_dynamic_speed_sub(void)
{
    if (fc_effect.Now_state != IS_DYNAMIC_EFFECT)
    {
        // 不处于动态效果，直接返回
        return;
    }

    if (fc_effect.on_off_flag != DEVICE_ON)
    {
        return;
    }

    // 0~9 ， 0--快，9--慢
    if (fc_effect.cur_light_dynamic_speed < 9)
    {
        fc_effect.cur_light_dynamic_speed++;
    }

    light_dynamic_mode_run();
}

/**
 * @brief 修改灯光动态模式
 *
 * @param dir  0--模式减，1--模式加

 */
void light_dynamic_mode_switch(u8 dir)
{
    if (fc_effect.on_off_flag != DEVICE_ON)
    {
        return;
    }

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
