/*********************************************************************************************
    *   Filename        : app_main.c

    *   Description     :

    *   Copyright:(c)JIELI  2011-2019  @ , All Rights Reserved.
*********************************************************************************************/
#include "system/includes.h"
#include "app_config.h"
#include "app_action.h"
#include "app_main.h"
#include "update.h"
#include "update_loader_download.h"
#include "app_charge.h"
#include "app_power_manage.h"
#include "asm/charge.h"


#if TCFG_KWS_VOICE_RECOGNITION_ENABLE
#include "jl_kws/jl_kws_api.h"
#endif /* #if TCFG_KWS_VOICE_RECOGNITION_ENABLE */

#define LOG_TAG_CONST       APP
#define LOG_TAG             "[APP]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

#include "rf_433_key.h"


OS_SEM LED_TASK_SEM;


/*任务列表   */
const struct task_info task_info_table[] = {
#if CONFIG_APP_FINDMY
    {"app_core",            1,     0,   640 * 2,   128  },
#else
    {"app_core",            1,     0,   640,   128  },
#endif

    {"sys_event",           7,     0,   256,   0    },
    {"btctrler",            4,     0,   512,   256  },
    {"btencry",             1,     0,   512,   128  },
    {"btstack",             3,     0,   768,   256   },
    {"systimer",		    7,	   0,   128,   0	},
    {"update",				1,	   0,   512,   0    },
    {"dw_update",		 	2,	   0,   256,   128  },
#if (RCSP_BTMATE_EN)
    {"rcsp_task",		    2,	   0,   640,	0},
#endif
#if(USER_UART_UPDATE_ENABLE)
    {"uart_update",	        1,	   0,   256,   128	},
#endif
#if (XM_MMA_EN)
    {"xm_mma",   		    2,	   0,   640,   256	},
#endif
    {"usb_msd",           	1,     0,   512,   128  },
#if TCFG_AUDIO_ENABLE
    {"audio_dec",           3,     0,   768,   128  },
    {"audio_enc",           4,     0,   512,   128  },
#endif/*TCFG_AUDIO_ENABLE*/
#if TCFG_KWS_VOICE_RECOGNITION_ENABLE
    {"kws",                 2,     0,   256,   64   },
#endif /* #if TCFG_KWS_VOICE_RECOGNITION_ENABLE */
#if (TUYA_DEMO_EN)
    {"user_deal",           2,     0,   512,   512  },//定义线程 tuya任务调度
#endif
#if (CONFIG_APP_HILINK)
    {"hilink_task",         2,     0,   1024,   0},//定义线程 hilink任务调度
#endif

    {"led_task",            2,      0,  512,    512},//灯光
    {0, 0},
};

APP_VAR app_var;

void app_var_init(void)
{
    app_var.play_poweron_tone = 1;

    app_var.auto_off_time =  TCFG_AUTO_SHUT_DOWN_TIME;
    app_var.warning_tone_v = 340;
    app_var.poweroff_tone_v = 330;
}

__attribute__((weak))
u8 get_charge_online_flag(void)
{
    return 0;
}

void clr_wdt(void);
void check_power_on_key(void)
{
#if TCFG_POWER_ON_NEED_KEY

    u32 delay_10ms_cnt = 0;
    while (1) {
        clr_wdt();
        os_time_dly(1);

        extern u8 get_power_on_status(void);
        if (get_power_on_status()) {
            log_info("+");
            delay_10ms_cnt++;
            if (delay_10ms_cnt > 70) {
                /* extern void set_key_poweron_flag(u8 flag); */
                /* set_key_poweron_flag(1); */
                return;
            }
        } else {
            log_info("-");
            delay_10ms_cnt = 0;
            log_info("enter softpoweroff\n");
            power_set_soft_poweroff();
        }
    }
#endif
}


void app_main()
{
    struct intent it;

    if (!UPDATE_SUPPORT_DEV_IS_NULL()) {
        int update = 0;
        update = update_result_deal();
    }

    printf(">>>>>>>>>>>>>>>>>app_main...\n");
    printf(">>> v220,2022-11-23 >>>\n");

    if (get_charge_online_flag()) {
#if(TCFG_SYS_LVD_EN == 1)
        vbat_check_init();
#endif
    } else {
        check_power_on_voltage();
    }

#if TCFG_POWER_ON_NEED_KEY
    check_power_on_key();
#endif

#if TCFG_AUDIO_ENABLE
    extern int audio_dec_init();
    extern int audio_enc_init();
    audio_dec_init();
    audio_enc_init();
#endif/*TCFG_AUDIO_ENABLE*/

#if TCFG_KWS_VOICE_RECOGNITION_ENABLE
    jl_kws_main_user_demo();
#endif /* #if TCFG_KWS_VOICE_RECOGNITION_ENABLE */

    init_intent(&it);

#if CONFIG_APP_SPP_LE
    it.name = "spp_le";
    it.action = ACTION_SPPLE_MAIN;

#elif CONFIG_APP_AT_COM || CONFIG_APP_AT_CHAR_COM
    it.name = "at_com";
    it.action = ACTION_AT_COM;

#elif CONFIG_APP_DONGLE
    it.name = "dongle";
    it.action = ACTION_DONGLE_MAIN;

#elif CONFIG_APP_MULTI
    it.name = "multi_conn";
    it.action = ACTION_MULTI_MAIN;

#elif CONFIG_APP_NONCONN_24G
    it.name = "nonconn_24g";
    it.action = ACTION_NOCONN_24G_MAIN;

#elif CONFIG_APP_HILINK
    it.name = "hilink";
    it.action = ACTION_HILINK_MAIN;

#elif CONFIG_APP_LL_SYNC
    it.name = "ll_sync";
    it.action = ACTION_LL_SYNC;

#elif CONFIG_APP_TUYA
    it.name = "tuya";
    it.action = ACTION_TUYA;

#elif CONFIG_APP_CENTRAL
    it.name = "central";
    it.action = ACTION_CENTRAL_MAIN;

#elif CONFIG_APP_DONGLE
    it.name = "dongle";
    it.action = ACTION_DONGLE_MAIN;

#elif CONFIG_APP_BEACON
    it.name = "beacon";
    it.action = ACTION_BEACON_MAIN;

#elif CONFIG_APP_IDLE
    it.name = "idle";
    it.action = ACTION_IDLE_MAIN;

#elif CONFIG_APP_CONN_24G
    it.name = "conn_24g";
    it.action = ACTION_CONN_24G_MAIN;

#elif CONFIG_APP_FINDMY
    it.name = "findmy";
    it.action = ACTION_FINDMY;

#else
    while (1) {
        printf("no app!!!");
    }
#endif


    log_info("run app>>> %s", it.name);
    log_info("%s,%s", __DATE__, __TIME__);

    start_app(&it);

#if TCFG_CHARGE_ENABLE
    set_charge_event_flag(1);
#endif
}

/*
 * app模式切换
 */
void app_switch(const char *name, int action)
{
    struct intent it;
    struct application *app;

    log_info("app_exit\n");

    init_intent(&it);
    app = get_current_app();
    if (app) {
        /*
         * 退出当前app, 会执行state_machine()函数中APP_STA_STOP 和 APP_STA_DESTORY
         */
        it.name = app->name;
        it.action = ACTION_BACK;
        start_app(&it);
    }

    /*
     * 切换到app (name)并执行action分支
     */
    it.name = name;
    it.action = action;
    start_app(&it);
}

int eSystemConfirmStopStatus(void)
{
    /* 系统进入在未来时间里，无任务超时唤醒，可根据用户选择系统停止，或者系统定时唤醒(100ms) */
    //1:Endless Sleep
    //0:100 ms wakeup
    /* log_info("100ms wakeup"); */
    return 1;
}

__attribute__((used)) int *__errno()
{
    static int err;
    return &err;
}


#if 1

// --------------------------------------------------------------------------定时器
static const u16 timer_div[] = {
    /*0000*/    1,
    /*0001*/    4,
    /*0010*/    16,
    /*0011*/    64,
    /*0100*/    2,
    /*0101*/    8,
    /*0110*/    32,
    /*0111*/    128,
    /*1000*/    256,
    /*1001*/    4 * 256,
    /*1010*/    16 * 256,
    /*1011*/    64 * 256,
    /*1100*/    2 * 256,
    /*1101*/    8 * 256,
    /*1110*/    32 * 256,
    /*1111*/    128 * 256,
};
// #define APP_TIMER_CLK           (CONFIG_BT_NORMAL_HZ/2) //clk_get("timer")
#define APP_TIMER_CLK           (24000000) //clk_get("timer")
#define MAX_TIME_CNT            0x7fff
#define MIN_TIME_CNT            0x100
#define TIMER_UNIT				1

#define TIMER_CON               JL_TIMER2->CON
#define TIMER_CNT               JL_TIMER2->CNT
#define TIMER_PRD               JL_TIMER2->PRD
#define TIMER_VETOR             IRQ_TIME2_IDX

static volatile u32 __rf_data; // 定时器中断使用的接收缓冲区，避免直接覆盖全局的数据接收缓冲区

___interrupt
AT_VOLATILE_RAM_CODE
void user_timer_isr(void) // 125us
{
	
	TIMER_CON |= BIT(14);

    extern void one_wire_send(void);
    one_wire_send();  //steomotor


#if 0  // 测试中断时间
    {
        static u8 dir = 0;

        if (0 == dir)
        {
            // gpio_set_output_value(RF_433_KEY_SCAN_PIN, 0); // 1高0低
            gpio_set_output_value(IO_PORTB_07, 0); // 1高0低
            dir = 1;
        }
        else
        {
            // gpio_set_output_value(RF_433_KEY_SCAN_PIN, 1); // 1高0低
            gpio_set_output_value(IO_PORTB_07, 0); // 1高0低
            dir = 0;
        } 
    }
#endif // 测试中断时间

#if 1 // rf信号接收 （125us调用一次，由100us调用一次的版本修改而来）
    {
        static volatile u8 rf_bit_cnt; // RF信号接收的数据位计数值

        static volatile u8 flag_is_enable_recv;   // 是否使能接收的标志位，要接收到 5ms+ 的低电平才开始接收
        static volatile u8 __flag_is_recved_data; // 表示中断服务函数接收到了rf数据

        static volatile u8 low_level_cnt;  // RF信号低电平计数值
        static volatile u8 high_level_cnt; // RF信号高电平计数值

        // 在定时器 中扫描端口电平
        // if (0 == RFIN_PIN)
        if (0 == gpio_read(RF_433_KEY_SCAN_PIN))
        {
            // 测试用，看看能不能检测到低电平
            // gpio_set_output_value(IO_PORTB_00, 0); // 1高0低

            // 如果RF接收引脚为低电平，记录低电平的持续时间
            low_level_cnt++;

            /*
                下面的判断条件是避免部分遥控器或接收模块只发送24位数据，最后不拉高电平的情况
            */
            // if (low_level_cnt >= (u8)((u32)30 * 100 / 125) && rf_bit_cnt == 23) // 如果低电平大于3000us，并且已经接收了23位数据
            if (low_level_cnt >= 24 && rf_bit_cnt == 23) // 如果低电平大于3000us，并且已经接收了23位数据
            {
                // if (high_level_cnt >= (u8)((u32)6 * 100 / 125) && high_level_cnt < (u8)((u32)20 * 100 / 125))
                if (high_level_cnt >= 5 && high_level_cnt < 12)
                {
                    /* 高电平时间在 【625us ~  1500us】，认为是逻辑1*/ 
                    __rf_data |= 0x01;
                }
                // else if (high_level_cnt >= 1 /* 这里不能为0，因此不能加 【* 100 / 125】  */
                //          && high_level_cnt < ((u32)6 * 100 / 125))
                else if (high_level_cnt >= 0 && high_level_cnt < 5)
                {

                }

                __flag_is_recved_data = 1; // 接收完成标志位置一
                flag_is_enable_recv = 0;
            }
        }
        else
        {
            // 测试用，看看能不能检测到高电平
            // gpio_set_output_value(IO_PORTB_00, 1); // 1高0低

            if (low_level_cnt > 0)
            {
                // 如果之前接收到了低电平信号，现在遇到了高电平，判断是否接收完成了一位数据
                // if (low_level_cnt > (u8)((u32)50 * 100 / 125))
                if (low_level_cnt > 40)
                {
                    // 如果低电平持续时间大于50 * 100us（5ms），准备下一次再读取有效信号
                    __rf_data = 0;  // 清除接收的数据帧
                    rf_bit_cnt = 0; // 清除用来记录接收的数据位数

                    flag_is_enable_recv = 1;
                }
                else if (flag_is_enable_recv &&
                         low_level_cnt > 0 && low_level_cnt < 5 &&
                         high_level_cnt >= 5 && high_level_cnt < 12)
                {
                    /* 逻辑1 高电平时间 625 ~ 1500us，低电平时间 0 ~ 625us */
                    __rf_data |= 0x01;
                    rf_bit_cnt++;
                    if (rf_bit_cnt != 24)
                    {
                        __rf_data <<= 1; // 用于存放接收24位数据的变量左移一位
                    }
                }
                else if (flag_is_enable_recv &&
                         low_level_cnt >= 5 && low_level_cnt < 12 &&
                         high_level_cnt > 0 /* 这里不能为0，因此不能加 【* 100 / 125】  */
                         && high_level_cnt < 5)
                {
                    /* 逻辑0 高电平时间 0~625us，低电平时间 625~1500us */
                    __rf_data &= ~1;
                    rf_bit_cnt++;
                    if (rf_bit_cnt != 24)
                    {
                        __rf_data <<= 1; // 用于存放接收24位数据的变量左移一位
                    }
                }
                else
                {
                    // 如果低电平持续时间不符合0和1的判断条件，说明此时没有接收到信号
                    __rf_data = 0;
                    rf_bit_cnt = 0;
                    flag_is_enable_recv = 0;
                }

                low_level_cnt = 0; // 无论是否接收到一位数据，遇到高电平时，先清除之前的计数值
                high_level_cnt = 0;

                if (24 == rf_bit_cnt)
                {
                    // 如果接收成了24位的数据
                    __flag_is_recved_data = 1; // 接收完成标志位置一
                    flag_is_enable_recv = 0;
                }
            }
            else
            {
                // 如果接收到高电平后，低电平的计数为0

                if (0 == flag_is_enable_recv)
                {
                    __rf_data = 0;
                    rf_bit_cnt = 0;
                    flag_is_enable_recv = 0;
                }
            }

            // 如果RF接收引脚为高电平，记录高电平的持续时间
            high_level_cnt++;
        }

        if (__flag_is_recved_data) //
        {
            rf_bit_cnt = 0;
            __flag_is_recved_data = 0;
            low_level_cnt = 0;
            high_level_cnt = 0;

            // if (rf_data != 0)
            // if (0 == flag_is_recved_rf_data) /* 如果之前未接收到数据 或是 已经处理完上一次接收到的数据 */
            {
                // 现在改为只要收到新的数据，就覆盖rf_data
                rf_433_data = __rf_data;

                // flag_is_recved_rf_data = 1;
                flag_is_received_rf_433_data = 1;
            }
            // else
            // {
            //     __rf_data = 0;
            // }
        }
    }
#endif // rf信号接收 （125us调用一次，由100us调用一次的版本修改而来）

}


void user_timer_init(void)
{
	u32 prd_cnt;
	u8 index;

//	printf("********* user_timer_init **********\n");
	for (index = 0; index < (sizeof(timer_div) / sizeof(timer_div[0])); index++)
	{
        prd_cnt = TIMER_UNIT * (APP_TIMER_CLK / 8000) / timer_div[index];
        if (prd_cnt > MIN_TIME_CNT && prd_cnt < MAX_TIME_CNT) {
            break;
        }
    }

	TIMER_CNT = 0;
	TIMER_PRD = prd_cnt;
	request_irq(TIMER_VETOR, 3, user_timer_isr, 0);
	TIMER_CON = (0b0001 << 10) | (index << 4) | (0x01 << 0);  // 选择晶振作为时钟源，分频系数，定时器计数模式 
}
__initcall(user_timer_init);


#endif


#include "led_strip_drive.h"
#include "led_strip_sys.h"
// #include "hardware.h" // 空的头文件
#include "neo_pixel.h"
#include "WS2812FX.H"

void main_while(void)
{


    while(1)
    {   
        // printf("%s\n",__func__);
 
        effect_stepmotor();    //声控，电机的音乐效果
        stepmotor();           //无霍尔时，电机停止指令计时
        power_motor_Init();    //电机
        meteor_period_sub();   //流星周期控制
        sound_handle();
        rf_433_key_event_handle(NULL);
        run_tick_per_10ms();
        WS2812FX_service();


        #if 0
        if (flag_is_received_rf_433_data)
        {
            flag_is_received_rf_433_data = 0;


            printf("rf 433 data 0x %lx \n", rf_433_data);
        }
        #endif

        os_time_dly(1);
    }

}



void my_main(void)
{

   
    led_gpio_init();
    led_pwm_init();
    mic_gpio_init();       
    fan_gpio_init();
    led_state_init();      
    mcu_com_init();  //电机一线通信

    read_flash_device_status_init(); // 上电读取FLASH里保存的指令数据
    full_color_init();

    rf_433_key_config();

    extern void automatic_lights_off_handle_by_timer_isr(void * p);
    sys_hi_timer_add(NULL, automatic_lights_off_handle_by_timer_isr, 1000); // 注册扫描定时器，1s周期

    os_sem_create(&LED_TASK_SEM,0);
    task_create(main_while, NULL, "led_task");

}



