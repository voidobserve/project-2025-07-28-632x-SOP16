#include "rf_433_key.h"

#include "led_strip_sys.h"
#include "WS2812FX.H"
#include "ws2812fx_effect.h"
#include "led_strand_effect.h"

#include "rf_433_key_handle_light.h"

#if RF_433_KEY_ENABLE

volatile u32 rf_433_data = 0;
volatile u8 flag_is_received_rf_433_data = 0;



// 定义按键事件
enum
{
    RF_433_KEY_EVENT_NONE = 0,

    RF_433_KEY_ID_ON_CLICK,
    RF_433_KEY_ID_ON_LONG,
    RF_433_KEY_ID_ON_HOLD,
    RF_433_KEY_ID_ON_LOOSE,

    RF_433_KEY_ID_OFF_CLICK,
    RF_433_KEY_ID_OFF_LONG,
    RF_433_KEY_ID_OFF_HOLD,
    RF_433_KEY_ID_OFF_LOOSE,

    RF_433_KEY_ID_LIGHT_ADD_CLICK,
    RF_433_KEY_ID_LIGHT_ADD_LONG,
    RF_433_KEY_ID_LIGHT_ADD_HOLD,
    RF_433_KEY_ID_LIGHT_ADD_LOOSE,

    RF_433_KEY_ID_LIGHT_SUB_CLICK,
    RF_433_KEY_ID_LIGHT_SUB_LONG,
    RF_433_KEY_ID_LIGHT_SUB_HOLD,
    RF_433_KEY_ID_LIGHT_SUB_LOOSE,

    RF_433_KEY_ID_LIGHT_CLICK,
    RF_433_KEY_ID_LIGHT_LONG,
    RF_433_KEY_ID_LIGHT_HOLD,
    RF_433_KEY_ID_LIGHT_LOOSE,

    RF_433_KEY_ID_SPEED_ADD_CLICK,
    RF_433_KEY_ID_SPEED_ADD_LONG,
    RF_433_KEY_ID_SPEED_ADD_HOLD,
    RF_433_KEY_ID_SPEED_ADD_LOOSE,

    RF_433_KEY_ID_SPEED_SUB_CLICK,
    RF_433_KEY_ID_SPEED_SUB_LONG,
    RF_433_KEY_ID_SPEED_SUB_HOLD,
    RF_433_KEY_ID_SPEED_SUB_LOOSE,

    RF_433_KEY_ID_MODE_ADD_CLICK,
    RF_433_KEY_ID_MODE_ADD_LONG,
    RF_433_KEY_ID_MODE_ADD_HOLD,
    RF_433_KEY_ID_MODE_ADD_LOOSE,

    RF_433_KEY_ID_MODE_SUB_CLICK,
    RF_433_KEY_ID_MODE_SUB_LONG,
    RF_433_KEY_ID_MODE_SUB_HOLD,
    RF_433_KEY_ID_MODE_SUB_LOOSE,

    RF_433_KEY_ID_10_PERCENT_CLICK,
    RF_433_KEY_ID_10_PERCENT_LONG,
    RF_433_KEY_ID_10_PERCENT_HOLD,
    RF_433_KEY_ID_10_PERCENT_LOOSE,

    RF_433_KEY_ID_20_PERCENT_CLICK,
    RF_433_KEY_ID_20_PERCENT_LONG,
    RF_433_KEY_ID_20_PERCENT_HOLD,
    RF_433_KEY_ID_20_PERCENT_LOOSE,

    RF_433_KEY_ID_30_PERCENT_CLICK,
    RF_433_KEY_ID_30_PERCENT_LONG,
    RF_433_KEY_ID_30_PERCENT_HOLD,
    RF_433_KEY_ID_30_PERCENT_LOOSE,

    RF_433_KEY_ID_40_PERCENT_CLICK,
    RF_433_KEY_ID_40_PERCENT_LONG,
    RF_433_KEY_ID_40_PERCENT_HOLD,
    RF_433_KEY_ID_40_PERCENT_LOOSE,

    RF_433_KEY_ID_50_PERCENT_CLICK,
    RF_433_KEY_ID_50_PERCENT_LONG,
    RF_433_KEY_ID_50_PERCENT_HOLD,
    RF_433_KEY_ID_50_PERCENT_LOOSE,

    RF_433_KEY_ID_60_PERCENT_CLICK,
    RF_433_KEY_ID_60_PERCENT_LONG,
    RF_433_KEY_ID_60_PERCENT_HOLD,
    RF_433_KEY_ID_60_PERCENT_LOOSE,

    RF_433_KEY_ID_70_PERCENT_CLICK,
    RF_433_KEY_ID_70_PERCENT_LONG,
    RF_433_KEY_ID_70_PERCENT_HOLD,
    RF_433_KEY_ID_70_PERCENT_LOOSE,

    RF_433_KEY_ID_80_PERCENT_CLICK,
    RF_433_KEY_ID_80_PERCENT_LONG,
    RF_433_KEY_ID_80_PERCENT_HOLD,
    RF_433_KEY_ID_80_PERCENT_LOOSE,

    RF_433_KEY_ID_90_PERCENT_CLICK,
    RF_433_KEY_ID_90_PERCENT_LONG,
    RF_433_KEY_ID_90_PERCENT_HOLD,
    RF_433_KEY_ID_90_PERCENT_LOOSE,

    RF_433_KEY_ID_100_PERCENT_CLICK,
    RF_433_KEY_ID_100_PERCENT_LONG,
    RF_433_KEY_ID_100_PERCENT_HOLD,
    RF_433_KEY_ID_100_PERCENT_LOOSE,

    RF_433_KEY_ID_5_COUNT_DOWN_CLICK,
    RF_433_KEY_ID_5_COUNT_DOWN_LONG,
    RF_433_KEY_ID_5_COUNT_DOWN_HOLD,
    RF_433_KEY_ID_5_COUNT_DOWN_LOOSE,

    RF_433_KEY_ID_10_COUNT_DOWN_CLICK,
    RF_433_KEY_ID_10_COUNT_DOWN_LONG,
    RF_433_KEY_ID_10_COUNT_DOWN_HOLD,
    RF_433_KEY_ID_10_COUNT_DOWN_LOOSE,

    RF_433_KEY_ID_30_COUNT_DOWN_CLICK,
    RF_433_KEY_ID_30_COUNT_DOWN_LONG,
    RF_433_KEY_ID_30_COUNT_DOWN_HOLD,
    RF_433_KEY_ID_30_COUNT_DOWN_LOOSE,

    RF_433_KEY_ID_60_COUNT_DOWN_CLICK,
    RF_433_KEY_ID_60_COUNT_DOWN_LONG,
    RF_433_KEY_ID_60_COUNT_DOWN_HOLD,
    RF_433_KEY_ID_60_COUNT_DOWN_LOOSE,
};

#define RF_433_KEY_VALID_EVENT_NUMS (4) // 单个按键，有效的按键事件数量
// 将遥控器按键的键值和按键事件绑定
const u8 rf_433_key_event_table[][RF_433_KEY_VALID_EVENT_NUMS + 1] = {
    {RF_433_KEY_ID_ON, RF_433_KEY_ID_ON_CLICK, RF_433_KEY_ID_ON_LONG, RF_433_KEY_ID_ON_HOLD, RF_433_KEY_ID_ON_LOOSE},
    {RF_433_KEY_ID_OFF, RF_433_KEY_ID_OFF_CLICK, RF_433_KEY_ID_OFF_LONG, RF_433_KEY_ID_OFF_HOLD, RF_433_KEY_ID_OFF_LOOSE},

    {RF_433_KEY_ID_LIGHT, RF_433_KEY_ID_LIGHT_CLICK, RF_433_KEY_ID_LIGHT_LONG, RF_433_KEY_ID_LIGHT_HOLD, RF_433_KEY_ID_LIGHT_LOOSE},
    {RF_433_KEY_ID_LIGHT_ADD, RF_433_KEY_ID_LIGHT_ADD_CLICK, RF_433_KEY_ID_LIGHT_ADD_LONG, RF_433_KEY_ID_LIGHT_ADD_HOLD, RF_433_KEY_ID_LIGHT_ADD_LOOSE},
    {RF_433_KEY_ID_LIGHT_SUB, RF_433_KEY_ID_LIGHT_SUB_CLICK, RF_433_KEY_ID_LIGHT_SUB_LONG, RF_433_KEY_ID_LIGHT_SUB_HOLD, RF_433_KEY_ID_LIGHT_SUB_LOOSE},

    {RF_433_KEY_ID_SPEED_ADD, RF_433_KEY_ID_SPEED_ADD_CLICK, RF_433_KEY_ID_SPEED_ADD_LONG, RF_433_KEY_ID_SPEED_ADD_HOLD, RF_433_KEY_ID_SPEED_ADD_LOOSE},
    {RF_433_KEY_ID_SPEED_SUB, RF_433_KEY_ID_SPEED_SUB_CLICK, RF_433_KEY_ID_SPEED_SUB_LONG, RF_433_KEY_ID_SPEED_SUB_HOLD, RF_433_KEY_ID_SPEED_SUB_LOOSE},

    {RF_433_KEY_ID_MODE_ADD, RF_433_KEY_ID_MODE_ADD_CLICK, RF_433_KEY_ID_MODE_ADD_LONG, RF_433_KEY_ID_MODE_ADD_HOLD, RF_433_KEY_ID_MODE_ADD_LOOSE},
    {RF_433_KEY_ID_MODE_SUB, RF_433_KEY_ID_MODE_SUB_CLICK, RF_433_KEY_ID_MODE_SUB_LONG, RF_433_KEY_ID_MODE_SUB_HOLD, RF_433_KEY_ID_MODE_SUB_LOOSE},

    {RF_433_KEY_ID_10_PERCENT, RF_433_KEY_ID_10_PERCENT_CLICK, RF_433_KEY_ID_10_PERCENT_LONG, RF_433_KEY_ID_10_PERCENT_HOLD, RF_433_KEY_ID_10_PERCENT_LOOSE},
    {RF_433_KEY_ID_20_PERCENT, RF_433_KEY_ID_20_PERCENT_CLICK, RF_433_KEY_ID_20_PERCENT_LONG, RF_433_KEY_ID_20_PERCENT_HOLD, RF_433_KEY_ID_20_PERCENT_LOOSE},
    {RF_433_KEY_ID_30_PERCENT, RF_433_KEY_ID_30_PERCENT_CLICK, RF_433_KEY_ID_30_PERCENT_LONG, RF_433_KEY_ID_30_PERCENT_HOLD, RF_433_KEY_ID_30_PERCENT_LOOSE},
    {RF_433_KEY_ID_40_PERCENT, RF_433_KEY_ID_40_PERCENT_CLICK, RF_433_KEY_ID_40_PERCENT_LONG, RF_433_KEY_ID_40_PERCENT_HOLD, RF_433_KEY_ID_40_PERCENT_LOOSE},
    {RF_433_KEY_ID_50_PERCENT, RF_433_KEY_ID_50_PERCENT_CLICK, RF_433_KEY_ID_50_PERCENT_LONG, RF_433_KEY_ID_50_PERCENT_HOLD, RF_433_KEY_ID_50_PERCENT_LOOSE},
    {RF_433_KEY_ID_60_PERCENT, RF_433_KEY_ID_60_PERCENT_CLICK, RF_433_KEY_ID_60_PERCENT_LONG, RF_433_KEY_ID_60_PERCENT_HOLD, RF_433_KEY_ID_60_PERCENT_LOOSE},
    {RF_433_KEY_ID_70_PERCENT, RF_433_KEY_ID_70_PERCENT_CLICK, RF_433_KEY_ID_70_PERCENT_LONG, RF_433_KEY_ID_70_PERCENT_HOLD, RF_433_KEY_ID_70_PERCENT_LOOSE},
    {RF_433_KEY_ID_80_PERCENT, RF_433_KEY_ID_80_PERCENT_CLICK, RF_433_KEY_ID_80_PERCENT_LONG, RF_433_KEY_ID_80_PERCENT_HOLD, RF_433_KEY_ID_80_PERCENT_LOOSE},
    {RF_433_KEY_ID_90_PERCENT, RF_433_KEY_ID_90_PERCENT_CLICK, RF_433_KEY_ID_90_PERCENT_LONG, RF_433_KEY_ID_90_PERCENT_HOLD, RF_433_KEY_ID_90_PERCENT_LOOSE},
    {RF_433_KEY_ID_100_PERCENT, RF_433_KEY_ID_100_PERCENT_CLICK, RF_433_KEY_ID_100_PERCENT_LONG, RF_433_KEY_ID_100_PERCENT_HOLD, RF_433_KEY_ID_100_PERCENT_LOOSE},

    {RF_433_KEY_ID_5_COUNT_DOWN, RF_433_KEY_ID_5_COUNT_DOWN_CLICK, RF_433_KEY_ID_5_COUNT_DOWN_LONG, RF_433_KEY_ID_5_COUNT_DOWN_HOLD, RF_433_KEY_ID_5_COUNT_DOWN_LOOSE},
    {RF_433_KEY_ID_10_COUNT_DOWN, RF_433_KEY_ID_10_COUNT_DOWN_CLICK, RF_433_KEY_ID_10_COUNT_DOWN_LONG, RF_433_KEY_ID_10_COUNT_DOWN_HOLD, RF_433_KEY_ID_10_COUNT_DOWN_LOOSE},
    {RF_433_KEY_ID_30_COUNT_DOWN, RF_433_KEY_ID_30_COUNT_DOWN_CLICK, RF_433_KEY_ID_30_COUNT_DOWN_LONG, RF_433_KEY_ID_30_COUNT_DOWN_HOLD, RF_433_KEY_ID_30_COUNT_DOWN_LOOSE},
    {RF_433_KEY_ID_60_COUNT_DOWN, RF_433_KEY_ID_60_COUNT_DOWN_CLICK, RF_433_KEY_ID_60_COUNT_DOWN_LONG, RF_433_KEY_ID_60_COUNT_DOWN_HOLD, RF_433_KEY_ID_60_COUNT_DOWN_LOOSE},

};

static u8 rf_433_key_get_value(void);
rf_433_key_struct_t rf_433_key_structure = {
    .rf_433_key_para.scan_time = RF_433_KEY_SCAN_CIRCLE_TIMES, // 扫描间隔时间
    .rf_433_key_para.last_key = NO_KEY,
    .rf_433_key_para.filter_value = 0,
    .rf_433_key_para.filter_cnt = 0,
    .rf_433_key_para.filter_time = 0, // 按键消抖次数
    .rf_433_key_para.long_time = RF_433_KEY_SCAN_LONG_PRESS_TIME_THRESHOLD / RF_433_KEY_SCAN_CIRCLE_TIMES,
    .rf_433_key_para.hold_time = (RF_433_KEY_SCAN_LONG_PRESS_TIME_THRESHOLD + RF_433_KEY_SCAN_HOLD_PRESS_TIME_THRESHOLD) / RF_433_KEY_SCAN_CIRCLE_TIMES,
    .rf_433_key_para.press_cnt = 0,
    .rf_433_key_para.click_cnt = 0,
    .rf_433_key_para.click_delay_cnt = 0,
    .rf_433_key_para.click_delay_time = RF_433_KEY_SCAN_MUILTY_CLICK_TIME_THRESHOLD / RF_433_KEY_SCAN_CIRCLE_TIMES,
    .rf_433_key_para.notify_value = 0,
    .rf_433_key_para.key_type = KEY_DRIVER_TYPE_RF_433_KEY,
    .rf_433_key_para.get_value = rf_433_key_get_value,

    .rf_433_key_driver_event = 0,
    .rf_433_key_latest_key_val = NO_KEY,
};

// struct key_driver_para rf_433_key_para = {
//     .scan_time = RF_433_KEY_SCAN_CIRCLE_TIMES, // 扫描间隔时间
//     .last_key = NO_KEY,
//     .filter_value = 0,
//     .filter_cnt = 0,
//     .filter_time = 0,
//     .long_time = RF_433_KEY_SCAN_LONG_PRESS_TIME_THRESHOLD / RF_433_KEY_SCAN_CIRCLE_TIMES,
//     .hold_time = RF_433_KEY_SCAN_HOLD_PRESS_TIME_THRESHOLD / RF_433_KEY_SCAN_CIRCLE_TIMES,
//     .press_cnt = 0,
//     .click_cnt = 0,
//     .click_delay_cnt = 0,
//     .click_delay_time = 0,
//     .notify_value = 0,
//     .key_type = KEY_DRIVER_TYPE_RF_433_KEY,
//     .get_value = rf_433_key_get_value,
// };

static u8 rf_433_key_get_value(void)
{
    // 超时时间，在超时时间内，仍认为有按键按下
    static u8 time_out_cnt = 0;
    static u32 last_rf_433_data = NO_KEY;

    u8 ret = NO_KEY;

    // printf("get key val 0x %lx \n", rf_433_data);
    if (flag_is_received_rf_433_data)
    {
        flag_is_received_rf_433_data = 0;

        last_rf_433_data = rf_433_data;
        time_out_cnt = RF_433_KEY_SCAN_EFFECTIVE_TIME_OUT / RF_433_KEY_SCAN_CIRCLE_TIMES; //
        ret = (u8)(rf_433_data & 0xFF);

        // printf("recved 433 data\n");
    }
    else if (time_out_cnt > 0)
    {
        ret = (u8)(last_rf_433_data & 0xFF);
        time_out_cnt--;
        // printf("exterd 433 data\n");
    }

    return ret;
}

void rf_433_key_config(void)
{
    gpio_set_pull_up(RF_433_KEY_SCAN_PIN, 1);   // 上拉
    gpio_set_pull_down(RF_433_KEY_SCAN_PIN, 0); // 不下拉

    gpio_set_die(RF_433_KEY_SCAN_PIN, 1);       // 普通输入模式
    gpio_set_direction(RF_433_KEY_SCAN_PIN, 1); // 输入模式
}

/**
 * @brief 根据 key_driver_scan()得到的按键键值和按键事件，转换成自定义的按键事件
 *
 * @param key_value key_driver_scan()得到的按键键值
 * @param key_event key_driver_scan()得到的按键事件
 *
 * @return u8 自定义的按键事件
 */
static u8 rf_433_key_get_keyevent(const u8 key_val, const u8 key_event)
{
    u8 i;
    u8 rf_433_key_event = RF_433_KEY_EVENT_NONE;
    u8 rf_433_key_event_table_index = 0;

    // printf("key event %u\n", (u16)key_event);

    /* 将 key_driver_scan() 得到的按键事件映射到自定义的按键事件列表中，用于下面的查表 */
    switch (key_event)
    {
    case KEY_EVENT_CLICK:
        /* 短按，在数组 rf_433_key_event_table 的[x][1]位置*/
        rf_433_key_event_table_index = 1;
        break;
    case KEY_EVENT_LONG:
        /* 长按，在数组 rf_433_key_event_table 的[x][2]位置 */
        rf_433_key_event_table_index = 2;
        break;
    case KEY_EVENT_HOLD:
        /* 长按持续（不松手），在数组 rf_433_key_event_table 的[x][3]位置 */
        rf_433_key_event_table_index = 3;
        break;
    case KEY_EVENT_UP:
        /* 长按后松手，在数组 rf_433_key_event_table 的[x][4]位置 */
        rf_433_key_event_table_index = 4;
        break;

    default:
        // 其他按键事件，认为是没有事件
        rf_433_key_event = RF_433_KEY_EVENT_NONE;
        return rf_433_key_event;
        break;
    }

    for (i = 0; i < ARRAY_SIZE(rf_433_key_event_table); i++)
    {
        if (key_val == rf_433_key_event_table[i][0])
        {
            rf_433_key_event = rf_433_key_event_table[i][rf_433_key_event_table_index];
            break;
        }
    }

    return rf_433_key_event;
}




// 按键事件处理函数
void rf_433_key_event_handle(void *p)
{
    u8 rf_433_key_event = RF_433_KEY_EVENT_NONE;

    // if (NO_KEY == rf_433_key_structure.rf_433_key_latest_key_val)
    // {
    //     return;
    // }

    // printf("get key event\n");

    rf_433_key_event = rf_433_key_get_keyevent(rf_433_key_structure.rf_433_key_latest_key_val, rf_433_key_structure.rf_433_key_driver_event);
    rf_433_key_structure.rf_433_key_latest_key_val = NO_KEY;
    rf_433_key_structure.rf_433_key_driver_event = RF_433_KEY_EVENT_NONE;

#if 1
    switch (rf_433_key_event)
    {
        // =================================================================
        // ON                                                           //
        // =================================================================
    case RF_433_KEY_ID_ON_CLICK:
        // printf("on click \n");
        soft_turn_on_the_light();
        break;
    // case RF_433_KEY_ID_ON_LOOSE:
    //     printf("on loose \n");
    //     break;
        // =================================================================
        // OFF                                                           //
        // =================================================================
    case RF_433_KEY_ID_OFF_CLICK:
        // printf("off click \n");
        soft_turn_off_lights();
        break;
    // case RF_433_KEY_ID_OFF_LOOSE:
    //     printf("off loose \n");
    //     break;
        // =================================================================
        // 最大亮度                                                        //
        // =================================================================
    case RF_433_KEY_ID_LIGHT_CLICK:
        // printf("light click \n");

        // 亮度最高
        ls_set_max_bright();
        break;
    // case RF_433_KEY_ID_LIGHT_LOOSE:
    //     printf("light loose \n");
    //     break;
        // =================================================================
        // 亮度加                                                          //
        // =================================================================
    case RF_433_KEY_ID_LIGHT_ADD_CLICK:
        // printf("light add click \n");
        ls_add_bright();
        break;
    // case RF_433_KEY_ID_LIGHT_ADD_LOOSE:
    //     printf("light add loose \n");
    //     break;
        // =================================================================
        // 亮度减                                                          //
        // =================================================================
    case RF_433_KEY_ID_LIGHT_SUB_CLICK:
        // printf("light sub click \n");
        ls_sub_bright();
        break;
    // case RF_433_KEY_ID_LIGHT_SUB_LOOSE:
    //     printf("light sub loose \n");
    //     break;
        // =================================================================
        // 动态效果，速度加                                                 //
        // =================================================================
    case RF_433_KEY_ID_SPEED_ADD_CLICK:
        // printf("speed add click \n");
        ls_add_speed();
        break;
    // case RF_433_KEY_ID_SPEED_ADD_LOOSE:
    //     printf("speed add loose \n");
    //     break;

        // =================================================================
        // 动态效果，速度减                                                //
        // =================================================================
    case RF_433_KEY_ID_SPEED_SUB_CLICK:
        // printf("speed sub click \n");
        ls_sub_speed();
        break;

    // case RF_433_KEY_ID_SPEED_SUB_LOOSE:
    //     printf("speed sub loose \n");
    //     break;
        // =================================================================
        // 动态效果，切换-                                                 //
        // =================================================================
    case RF_433_KEY_ID_MODE_SUB_CLICK:
        // printf("mode sub click \n");
        light_dynamic_mode_switch(0);
        break;
    // case RF_433_KEY_ID_MODE_SUB_LOOSE:
    //     printf("mode sub loose \n");
    //     break;
        // =================================================================
        // 动态效果，切换+                                                //
        // =================================================================
    case RF_433_KEY_ID_MODE_ADD_CLICK:
        // printf("mode add click \n");
        light_dynamic_mode_switch(0);
        break;
    // case RF_433_KEY_ID_MODE_ADD_LOOSE:
    //     printf("mode add loose \n");
    //     break;

        // =================================================================
        // 10% 亮度                                                      //
        // =================================================================
    case RF_433_KEY_ID_10_PERCENT_CLICK:
        printf("10 percent click \n");
        ls_set_bright(0);
        break;
    // case RF_433_KEY_ID_10_PERCENT_LOOSE:
    //     printf("10 percent loose \n");
    //     break;

        // =================================================================
        // 20% 亮度                                                      //
        // =================================================================
    case RF_433_KEY_ID_20_PERCENT_CLICK:
        printf("20 percent click \n");
        ls_set_bright(1);
        break;
    // case RF_433_KEY_ID_20_PERCENT_LOOSE:
    //     printf("20 percent loose \n");
    //     break;

        // =================================================================
        // 30% 亮度                                                      //
        // =================================================================
    case RF_433_KEY_ID_30_PERCENT_CLICK:
        printf("30 percent click \n");
        ls_set_bright(2);
        break;
    // case RF_433_KEY_ID_30_PERCENT_LOOSE:
    //     printf("30 percent loose \n");
    //     break;

        // =================================================================
        // 40% 亮度                                                      //
        // =================================================================
    case RF_433_KEY_ID_40_PERCENT_CLICK:
        printf("40 percent click \n");
        ls_set_bright(3);
        break;
    // case RF_433_KEY_ID_40_PERCENT_LOOSE:
    //     printf("40 percent loose \n");
    //     break;

        // =================================================================
        // 50% 亮度                                                      //
        // =================================================================
    case RF_433_KEY_ID_50_PERCENT_CLICK:
        printf("50 percent click \n");
        ls_set_bright(4);
        break;
    // case RF_433_KEY_ID_50_PERCENT_LOOSE:
    //     printf("50 percent loose \n");
    //     break;

        // =================================================================
        // 60% 亮度                                                      //
        // =================================================================
    case RF_433_KEY_ID_60_PERCENT_CLICK:
        printf("60 percent click \n");
        ls_set_bright(5);
        break;
    // case RF_433_KEY_ID_60_PERCENT_LOOSE:
    //     printf("60 percent loose \n");
    //     break;

        // =================================================================
        // 70% 亮度                                                      //
        // =================================================================
    case RF_433_KEY_ID_70_PERCENT_CLICK:
        printf("70 percent click \n");
        ls_set_bright(6);
        break;
    // case RF_433_KEY_ID_70_PERCENT_LOOSE:
    //     printf("70 percent loose \n");
    //     break;

        // =================================================================
        // 80% 亮度                                                      //
        // =================================================================
    case RF_433_KEY_ID_80_PERCENT_CLICK:
        printf("80 percent click \n");
        ls_set_bright(7);
        break;
    // case RF_433_KEY_ID_80_PERCENT_LOOSE:
    //     printf("80 percent loose \n");
    //     break;

        // =================================================================
        // 90% 亮度                                                      //
        // =================================================================
    case RF_433_KEY_ID_90_PERCENT_CLICK:
        printf("90 percent click \n");
        ls_set_bright(8);
        break;
    // case RF_433_KEY_ID_90_PERCENT_LOOSE:
    //     printf("90 percent loose \n");
    //     break;

        // =================================================================
        // 100% 亮度                                                      //
        // =================================================================
    case RF_433_KEY_ID_100_PERCENT_CLICK:
        printf("100 percent click \n");
        ls_set_bright(9);
        break;
    // case RF_433_KEY_ID_100_PERCENT_LOOSE:
    //     printf("100 percent loose \n");
    //     break;

        // =================================================================
        // 定时关机 5min                                                  //
        // =================================================================
    case RF_433_KEY_ID_5_COUNT_DOWN_CLICK:
        printf("5 count down click \n");
        // set_ir_timer
        set_ir_timer(IR_TIMER_5MIN);
        break;
    // case RF_433_KEY_ID_5_COUNT_DOWN_LOOSE:
    //     printf("5 count down loose \n");
    //     break;

        // =================================================================
        // 定时关机 10min                                                  //
        // =================================================================
    case RF_433_KEY_ID_10_COUNT_DOWN_CLICK:
        printf("10 count down click \n");
        set_ir_timer(IR_TIMER_10MIN);
        break;
    // case RF_433_KEY_ID_10_COUNT_DOWN_LOOSE:
    //     printf("10 count down loose \n");
    //     break;

        // =================================================================
        // 定时关机 30min                                                  //
        // =================================================================
    case RF_433_KEY_ID_30_COUNT_DOWN_CLICK:
        printf("30 count down click \n");
        set_ir_timer(IR_TIMER_30MIN);
        break;
    // case RF_433_KEY_ID_30_COUNT_DOWN_LOOSE:
    //     printf("30 count down loose \n");
    //     break;

        // =================================================================
        // 定时关机 60min                                                  //
        // =================================================================
    case RF_433_KEY_ID_60_COUNT_DOWN_CLICK:
        printf("60 count down click \n");
        set_ir_timer(IR_TIMER_60MIN);
        break;
    // case RF_433_KEY_ID_60_COUNT_DOWN_LOOSE:
    //     printf("60 count down loose \n");
    //     break;
    }
#endif
}

#if 0
void rf_433_key_test(void * p)
{
    if (flag_is_received_rf_433_data)
    {
        flag_is_received_rf_433_data = 0;
        // return (u8)(rf_433_data & 0xFF);

        printf("rf 433 data 0x %lx\n", rf_433_data);
    }
}
#endif

#endif // #if RF_433_KEY_ENABLE
