#ifndef __RF_433_KEY_H__
#define __RF_433_KEY_H__

#include "includes.h"

#define RF_433_KEY_ENABLE 1 // 是否使能 433 遥控器功能

#if RF_433_KEY_ENABLE

// 433信号检测引脚
// #define RF_433_KEY_SCAN_PIN IO_PORTB_06 // PB6 ,IC 1脚 (被外部硬件拉低)
// #define RF_433_KEY_SCAN_PIN IO_PORTB_07 // PB7 ,IC 16脚 （无法控制，可能有其他程序占用）
#define RF_433_KEY_SCAN_PIN IO_PORTB_05 // PB5 IC 2脚

// #define RF_433_KEY_SCAN_PIN IO_PORTB_01 //  PB1 用开发板来测试

/*
    按键的扫描周期，单位：ms
    注意不能超过变量的最大值，值不能为0
*/

#define RF_433_KEY_SCAN_CIRCLE_TIMES (10)

#define RF_433_KEY_SCAN_FILTER_TIMES (0)                     // 按键消抖次数
#define RF_433_KEY_SCAN_LONG_PRESS_TIME_THRESHOLD ((u16)750) // 长按时间阈值，单位：ms
#define RF_433_KEY_SCAN_HOLD_PRESS_TIME_THRESHOLD ((u16)150) // 长按持续（不松手）的时间阈值，单位：ms，每隔 xx ms认为有一次长按持续事件

// #define RF_433_KEY_SCAN_LONG_PRESS_TIME_THRESHOLD ((u16)750 * 2) // 长按时间阈值，单位：ms
// #define RF_433_KEY_SCAN_HOLD_PRESS_TIME_THRESHOLD ((u16)150 * 2) // 长按持续（不松手）的时间阈值，单位：ms，每隔 xx ms认为有一次长按持续事件

// #define RF_433_KEY_SCAN_LONG_PRESS_TIME_THRESHOLD ((u16)750 * 3) // 长按时间阈值，单位：ms
// #define RF_433_KEY_SCAN_HOLD_PRESS_TIME_THRESHOLD ((u16)150 * 3) // 长按持续（不松手）的时间阈值，单位：ms，每隔 xx ms认为有一次长按持续事件
// #define RF_433_KEY_SCAN_MUILTY_CLICK_TIME_THRESHOLD (500) // 等待多击的时间间隔，单位：ms
#define RF_433_KEY_SCAN_MUILTY_CLICK_TIME_THRESHOLD (0) // 等待多击的时间间隔，单位：ms
/*
    按键有效超时时间，单位：ms
    当按键按下后，如果按键没有被按下，超过这个时间，才认为按键无效。
    按键按下后，多久没有信号才开始认为按键无效。
    如果按键有效时间内还有按键按下，有效时间会重新计算。

    例如，按键有效时间为120ms。当按键按下再松手，之后的120ms内都认为这个按键按下。
    如果120ms内又有按键按下，有效时间会回到120ms，重新计算。

    120ms，除了不能检测快速的双击，其他都可以正常检测
    快速双击时，在逻辑分析仪上观察波形，两次双击的时间检测在198ms，
    但是按键按下后会连续发送多个波形，导致这两段波形的时间间隔小于两次双击的时间检测。
    在逻辑分析仪上面看到这两段波形的间隔在27ms左右，所以不能正常检测快速双击。
*/
#define RF_433_KEY_SCAN_EFFECTIVE_TIME_OUT (120)
// #define RF_433_KEY_SCAN_EFFECTIVE_TIME_OUT (500)

/*
    定义遥控器按键键值
*/
// #define RF_433_KEY_ID_ON ((u8)(0x14 & 0xFF))

#define RF_433_KEY_ID_ON ((u8)(0x18 & 0xFF))
#define RF_433_KEY_ID_OFF ((u8)(0x84 & 0xFF))
#define RF_433_KEY_ID_LIGHT_ADD ((u8)(0x05 & 0xFF))     // 亮度加
#define RF_433_KEY_ID_LIGHT_SUB ((u8)(0x87 & 0xFF))     // 亮度减
#define RF_433_KEY_ID_SPEED_ADD ((u8)(0x93 & 0xFF))     // 速度加
#define RF_433_KEY_ID_SPEED_SUB ((u8)(0x00 & 0xFF))     // 速度减
#define RF_433_KEY_ID_LIGHT ((u8)(0x06 & 0xFF))         // 灯亮最大亮度
#define RF_433_KEY_ID_MODE_SUB ((u8)(0x8A & 0xFF))      //
#define RF_433_KEY_ID_MODE_ADD ((u8)(0x8E & 0xFF))      //
#define RF_433_KEY_ID_10_PERCENT ((u8)(0x12 & 0xFF))    // 10%
#define RF_433_KEY_ID_20_PERCENT ((u8)(0x96 & 0xFF))    // 20%
#define RF_433_KEY_ID_30_PERCENT ((u8)(0x09 & 0xFF))    // 30%
#define RF_433_KEY_ID_40_PERCENT ((u8)(0x8D & 0xFF))    // 40%
#define RF_433_KEY_ID_50_PERCENT ((u8)(0x11 & 0xFF))    // 50%
#define RF_433_KEY_ID_60_PERCENT ((u8)(0x95 & 0xFF))    // 60%
#define RF_433_KEY_ID_70_PERCENT ((u8)(0x88 & 0xFF))    // 70%
#define RF_433_KEY_ID_80_PERCENT ((u8)(0x0C & 0xFF))    // 80%
#define RF_433_KEY_ID_90_PERCENT ((u8)(0x90 & 0xFF))    // 90%
#define RF_433_KEY_ID_100_PERCENT ((u8)(0x14 & 0xFF))   // 100%
#define RF_433_KEY_ID_5_COUNT_DOWN ((u8)(0x99 & 0xFF))  // 倒计时 5
#define RF_433_KEY_ID_10_COUNT_DOWN ((u8)(0x1A & 0xFF)) // 倒计时 10
#define RF_433_KEY_ID_30_COUNT_DOWN ((u8)(0x1B & 0xFF)) // 倒计时 30
#define RF_433_KEY_ID_60_COUNT_DOWN ((u8)(0x9C & 0xFF)) // 倒计时 60

typedef struct
{
    struct key_driver_para rf_433_key_para;
    u8 rf_433_key_driver_event;   // 存放得到的按键事件，在key_driver_scan()中更新
    u8 rf_433_key_latest_key_val; // 存放最新扫描到的按键键值
    // u8 rf_433_key_last_key_val; // 存放上一次得到的按键键值
} rf_433_key_struct_t;

extern rf_433_key_struct_t rf_433_key_structure;
// extern struct key_driver_para rf_433_key_para; // rf 433 按键扫描参数 结构体

extern volatile u32 rf_433_data;                 // 存放收到的rf433数据
extern volatile u8 flag_is_received_rf_433_data; // 标志位，是否收到一次数据

void rf_433_key_config(void);

// void rf_433_key_event_handle(void);// 按键事件处理函数
void rf_433_key_event_handle(void *p); // 按键事件处理函数

// void rf_433_key_test(void * p);
#endif // #if RF_433_KEY_ENABLE

#endif
