#include "A_config.h"

static lv_obj_t *scr_clock;
static lv_obj_t *bg;
static lv_obj_t *bar1;
static lv_obj_t *lblDate;
static lv_obj_t *lblTime[6][2];

static lv_obj_t *lblCurrentClass[2];
static lv_obj_t *lblNextClass[2];
static lv_obj_t *lblRemaining;

static lv_obj_t *swap;

static alarm_t *curr_class = NULL;
static alarm_t *next_class = NULL;
static alarm_t *breakflag = NULL; //用于是否下课
static uint8_t last_sec = 0;
static RTC_DATA_ATTR uint8_t digits[6];
static uint8_t digits_now[6];
static uint16_t label_y[6];
static uint32_t remaining;
static bool noanimfirst = true;  //第一次不显示课程更新动画
static bool fromweather = false; //刚才的表盘是天气信息，如果为true，那么在load中会使用滑动动画
void wf_clock_anim_set(lv_obj_t *label[2], uint16_t y, uint16_t delaytime)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
    lv_anim_set_time(&a, 500);
    lv_anim_set_delay(&a, delaytime);
    lv_anim_set_var(&a, label[1]);
    lv_anim_set_values(&a, y - 32, y);
    lv_anim_start(&a);
    lv_obj_fade_in(label[1], 500, delaytime);

    lv_anim_set_var(&a, label[0]);
    lv_anim_set_values(&a, y, y + 32);
    lv_anim_start(&a);
    lv_obj_fade_out(label[0], 400, delaytime);

    swap = label[0];
    label[0] = label[1];
    label[1] = swap;
}
static uint16_t class_y = 120 + 10;
static void swap_class(uint16_t p)
{
    REQUESTLV();
    if (curr_class == NULL)
    {
        if (next_class == NULL)
        {
            lv_label_set_text(lblCurrentClass[0], "无");
            lv_label_set_text(lblNextClass[0], "无");
            RELEASELV();
            return;
        }
        else
        {
            lv_label_set_text(lblCurrentClass[1], "不可用");
            lv_label_set_text_fmt(lblNextClass[1], "%s", next_class->subtype);
        }
    }
    else if (curr_class->time_end <= p)
    {
        //当前课程已结束，如果闹钟没有冲突，此时一定有next_class->time_start > p
        lv_label_set_text_fmt(lblCurrentClass[1], "%s|课", curr_class->subtype);
        lv_label_set_text_fmt(lblNextClass[1], "间|%s", next_class->subtype);
    }
    else
    {
        //当前课程未结束
        lv_label_set_text_fmt(lblCurrentClass[1], "%s----->", curr_class->subtype);
        lv_label_set_text_fmt(lblNextClass[1], "下课|%s", next_class->subtype);
    }
    if (noanimfirst)
    {
        lv_obj_fade_in(lblCurrentClass[1], 500, 0);
        lv_obj_fade_in(lblNextClass[1], 500, 200);
        swap = lblCurrentClass[0];
        lblCurrentClass[0] = lblCurrentClass[1];
        lblCurrentClass[1] = swap;

        swap = lblNextClass[0];
        lblNextClass[0] = lblNextClass[1];
        lblNextClass[1] = swap;
        noanimfirst = false;
        RELEASELV();
        return;
    }
    wf_clock_anim_set(lblCurrentClass, class_y, 0);
    wf_clock_anim_set(lblNextClass, class_y, 200);
    RELEASELV();
}
static void wf_clock_loop()
{
    if (last_sec != hal.rtc.getSecond())
    {
        alarm_check();
        //更新时间、日期、闹钟、倒计时
        uint8_t start_i = 99;
        uint8_t week;
        uint8_t minute;
        uint8_t hour;
        uint16_t p;
        last_sec = hal.rtc.getSecond();
        hour = hal.rtc.getHour();
        digits_now[0] = hour / 10;
        digits_now[1] = hour % 10;
        minute = hal.rtc.getMinute();
        digits_now[2] = minute / 10;
        digits_now[3] = minute % 10;
        digits_now[4] = last_sec / 10;
        digits_now[5] = last_sec % 10;
        for (uint8_t i = 0; i < 6; ++i)
        {
            if (digits[i] != digits_now[i])
            {
                if (start_i == 99)
                    start_i = i;
                digits[i] = digits_now[i];
                REQUESTLV();
                lv_label_set_text_fmt(lblTime[i][1], "%d", digits[i] % 10);
                wf_clock_anim_set(lblTime[i], label_y[i], (i - start_i) * 100);
                RELEASELV();
            }
        }
        REQUESTLV();
        lv_label_set_text_fmt(lblDate, "20%02d/%02d/%02d  %s", hal.rtc.getYear(),
                              hal.rtc.getMonth(), hal.rtc.getDate(),
                              week_name[week = hal.rtc.getDoW()]);
        RELEASELV();
        p = hour * 60 + minute;
        if (curr_class != class_get_curr(week, p) || breakflag != class_get_next(week, p))
        {
            //更新闹钟信息
            breakflag = class_get_next(week, p);
            curr_class = class_get_curr(week, p);
            next_class = class_get_next_no_curr(week, p);
            alarm_update();
            if (next_class == NULL)
            {
                next_class = class_get_next_no_curr(1, 0);
            }
            swap_class(p);
        }

        uint32_t maxval = 0xffffffff;
        uint8_t dweek; //用于存储不同天闹钟相差天数
        if (curr_class == NULL)
        {
            if (next_class == NULL)
            {
                remaining = 0xffffffff;
            }
            else
            {
                dweek = next_class->week < week ||
                                (next_class->time_start <= p && next_class->week == week)
                            ? next_class->week + 7
                            : next_class->week;
                dweek -= week;
                maxval = remaining = dweek * 24 * 60 + next_class->time_start - p;
            }
        }
        else if (next_class->week != week || curr_class->week != week)
        {
            dweek = next_class->week <= week ? next_class->week + 7 : next_class->week;
            dweek -= week;
            maxval = remaining = dweek * 24 * 60 + next_class->time_start - p;
        }
        else if (next_class->time_start <= p && curr_class->time_end <= p)
        {
            //当前课程已结束，但是下节课在下周的同一天
            maxval = remaining = 7 * 24 * 60 + next_class->time_start - p;
        }
        else if (curr_class->time_end <= p)
        {
            //当前课程已结束，如果闹钟没有冲突，此时一定有next_class->time_start > p
            remaining = next_class->time_start - p;
            maxval = next_class->time_start - curr_class->time_end;
        }
        else
        {
            //当前课程未结束
            remaining = curr_class->time_end - p;
            maxval = curr_class->time_end - curr_class->time_start;
        }
        if (remaining != 0xffffffff)
        {
            remaining *= 60;
            remaining -= last_sec;
            maxval *= 60;
        }
        REQUESTLV();
        lv_bar_set_value(bar1, map(remaining, maxval, 0, 0, 100), LV_ANIM_ON);
        lv_label_set_text_fmt(lblRemaining, "%02d:%02d:%02d",
                              remaining / 3600, remaining % 3600 / 60, remaining % 60);
        RELEASELV();
    }
    hal.canDeepSleep = true;
    if (strcmp(hal.conf.getValue("watchonly"), "1") == 0)
    {
        //这就是个普通手表--去除除了闹钟以外的任何功能
        vTaskDelay(50);
        return;
    }
    if (hal.btnEnter.isPressedRaw())
    {
        uint16_t tmp;
        lv_obj_t *msgbox_full;
        hal.canDeepSleep = false;
        vTaskDelay(100);
        if (hal.btnEnter.isPressedRaw())
        {
            menu_create();
            menu_add(LV_SYMBOL_BELL " 课程管理");
            menu_add(LV_SYMBOL_DOWNLOAD " 更新天气信息");
            menu_add(LV_SYMBOL_SETTINGS " 设置");
            menu_add(LV_SYMBOL_PLAY " Bilibili");
            menu_add(LV_SYMBOL_SETTINGS " Debug 专用选项");
            switch (menu_show())
            {
            case 1:
                //课程管理
                hal.fLoop = NULL;
                pushWatchFace(wf_clock_load);
                wf_class_load();
                return;
            case 2:
                //更新天气信息
                msgbox_full = full_screen_msgbox_create(BIG_SYMBOL_SYNC,
                                                        "天气", "尝试获取天气", FULL_SCREEN_BG_SYNC);
                if (weather.refresh(hal.conf.getString("city")) == 0)
                {
                    full_screen_msgbox_del(msgbox_full);
                    while (hal.btnEnter.isPressedRaw())
                        vTaskDelay(10);
                    full_screen_msgbox(BIG_SYMBOL_CHECK, "天气",
                                       "成功获取当日天气信息", FULL_SCREEN_BG_CHECK);
                }
                else
                {
                    full_screen_msgbox_del(msgbox_full);
                    while (hal.btnEnter.isPressedRaw())
                        vTaskDelay(10);
                    full_screen_msgbox(BIG_SYMBOL_CROSS, "天气", "天气信息获取失败 ",
                                       FULL_SCREEN_BG_CROSS);
                }
                hal.disconnectWiFi();
                break;
            case 3:
                //设置
                menu_create();
                menu_add(LV_SYMBOL_WIFI " WiFi Smartconfig");
                menu_add(LV_SYMBOL_PLUS " 时间设置");
                menu_add(LV_SYMBOL_TRASH " 清除B站登录信息");
                menu_add(LV_SYMBOL_BELL " 允许振动");
                menu_add(LV_SYMBOL_REFRESH " 允许日更信息");
                menu_add(LV_SYMBOL_FILE " 启动HTTP服务器");
                menu_add("系统信息");
                switch (menu_show())
                {
                case 1:
                    //WiFi Smartconfig
                    full_screen_msgbox(BIG_SYMBOL_INFO, "WiFi Smartconfig",
                                       "请用手机下载ESPTouch应用，按提示操作，按下确定键开始",
                                       FULL_SCREEN_BG_INFO);
                    hal.DoNotSleep = true;
                    msgbox_full = full_screen_msgbox_create(BIG_SYMBOL_SYNC,
                                                            "WiFi Smartconfig",
                                                            "正在等待Smartconfig，长按确定键1s取消",
                                                            FULL_SCREEN_BG_SYNC);
                    if (hal.beginSmartconfig())
                    {
                        full_screen_msgbox_del(msgbox_full);
                        while (hal.btnEnter.isPressedRaw())
                            vTaskDelay(10);
                        full_screen_msgbox(BIG_SYMBOL_CHECK, "WiFi Smartconfig",
                                           "配网成功，已保存并自动断开连接", FULL_SCREEN_BG_CHECK);
                    }
                    else
                    {
                        full_screen_msgbox_del(msgbox_full);
                        while (hal.btnEnter.isPressedRaw())
                            vTaskDelay(10);
                        full_screen_msgbox(BIG_SYMBOL_CROSS, "WiFi Smartconfig",
                                           "配网失败", FULL_SCREEN_BG_CROSS);
                    }
                    hal.disconnectWiFi();
                    hal.DoNotSleep = false;
                    break;
                case 2:
                    //时间设置
                    menu_create();
                    menu_add(LV_SYMBOL_WIFI " 同步网络时间");
                    menu_add(LV_SYMBOL_EDIT " 时钟微调");
                    switch (menu_show())
                    {
                    case 1:
                        //同步网络时间
                        msgbox_full = full_screen_msgbox_create(BIG_SYMBOL_SYNC,
                                                                "时间同步",
                                                                "正在与NTP服务器同步时间: " CONFIG_NTP_ADDR,
                                                                FULL_SCREEN_BG_SYNC);
                        hal.DoNotSleep = true;
                        if (hal.NTPSync())
                        {
                            full_screen_msgbox_del(msgbox_full);
                            while (hal.btnEnter.isPressedRaw())
                                vTaskDelay(10);
                            full_screen_msgbox(BIG_SYMBOL_CHECK, "时间同步", "同步成功!",
                                               FULL_SCREEN_BG_CHECK, 1000);
                        }
                        else
                        {
                            full_screen_msgbox_del(msgbox_full);
                            while (hal.btnEnter.isPressedRaw())
                                vTaskDelay(10);
                            full_screen_msgbox(BIG_SYMBOL_CROSS, "时间同步",
                                               "同步失败", FULL_SCREEN_BG_CROSS, 1000);
                        }
                        hal.DoNotSleep = false;
                        break;
                    case 2:
                        //时钟微调
                        full_screen_msgbox(BIG_SYMBOL_INFO, "RTC频率偏移",
                                           "提示：正值减小振荡器频率，负值增大振荡器频率",
                                           FULL_SCREEN_BG_INFO);
                        tmp = hal.rtc.readOffset();
                        tmp = msgbox_number("请输入偏移量", 3, 0, 127, -128, tmp);
                        if (msgbox_yn("请确认是否保存"))
                        {
                            while (hal.rtc.readOffset() != tmp)
                            {
                                hal.rtc.writeOffset(tmp);
                                vTaskDelay(10);
                            }
                        }
                        break;
                    default:
                        break;
                    }
                    break;
                case 3:
                    //清除B站登录信息
                    msgbox_yn("是否清除B站登录信息？");
                    msgbox("失败？", ("需要管理员权限，请看clock.cpp line" + String(__LINE__)).c_str()); //提示：为了防止误删，需要在隐藏功能页面清除
                    break;
                case 4:
                    //允许振动
                    if (msgbox_yn("是否允许振动电机工作\n"))
                    {
                        hal.conf.setValue("enmotor", "1");
                    }
                    else
                    {
                        hal.conf.setValue("enmotor", "0");
                    }
                    hal.conf.writeConfig();
                    break;
                case 5:
                    //允许日更信息
                    if (msgbox_yn("是否允许在每天首次唤醒时自动更新信息"))
                    {
                        hal.conf.setValue("updated", "1");
                    }
                    else
                    {
                        hal.conf.setValue("updated", "0");
                    }
                    hal.conf.writeConfig();
                    break;
                case 6:
                    //启动HTTP服务器
                    pushWatchFace(wf_clock_load);
                    wf_webserver_load();
                    return;
                    break;
                case 7:
                    //系统信息
                    pushWatchFace(wf_clock_load);
                    wf_sysinfo_load();
                    return;
                    break;
                default:
                    break;
                }
                break;
            case 4:
                //Bilibili
                pushWatchFace(wf_clock_load);
                wf_bilibili_load();
                break;
            case 5:
                //DEBUG专用
                {
                    break;
                }
            default:
                break;
            }
        }
    }
    if (hal.btnDown.isPressedRaw())
    {
        fromweather = true;
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
        lv_anim_set_var(&a, bg);
        lv_anim_set_time(&a, 600);
        lv_anim_set_values(&a, 108, 0);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_height);
        lv_anim_start(&a);
        pushWatchFace(wf_clock_load);
        wf_weather_load();
        return;
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
}

void wf_clock_load(void)
{
    uint8_t week = hal.rtc.getDoW();
    uint16_t p = hal.rtc.getHour() * 60 + hal.rtc.getMinute();
    breakflag = class_get_next(week, p);
    curr_class = class_get_curr(week, p);
    next_class = class_get_next_no_curr(week, p);
    if (next_class == NULL)
    {
        next_class = class_get_next_no_curr(0, 0);
    }
    REQUESTLV();
    scr_clock = lv_obj_create(NULL);
    //背景
    bg = lv_obj_create(scr_clock);
    lv_obj_set_style_border_width(bg, 0, 0);
    lv_obj_set_size(bg, 240, 108);
    lv_obj_set_pos(bg, 0, 0);
    lv_obj_set_style_bg_color(bg, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
    lv_anim_set_var(&a, bg);
    lv_anim_set_time(&a, 600);
    if (fromweather)
    {
        lv_anim_set_values(&a, 0, 108);
        fromweather = false;
        lv_scr_load_anim(scr_clock, LV_SCR_LOAD_ANIM_MOVE_TOP, 300, 0, true);
    }
    else
    {
        lv_anim_set_values(&a, 70, 108);
        if (lv_scr_act())
            lv_obj_del(lv_scr_act());
        lv_scr_load(scr_clock);
    }

    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_height);
    lv_anim_start(&a);
    //课程进度条、倒计时

    bar1 = lv_bar_create(scr_clock);
    lv_obj_set_size(bar1, 200, 16);
    lv_obj_align(bar1, LV_ALIGN_CENTER, 0, 4);
    lv_obj_set_style_anim_time(bar1, 500, 0);
    lv_bar_set_value(bar1, 0, LV_ANIM_OFF);

    lblCurrentClass[0] = lv_label_create(scr_clock);
    lblNextClass[0] = lv_label_create(scr_clock);
    lv_obj_align(lblCurrentClass[0], LV_ALIGN_CENTER, -80, 20);
    lv_obj_align(lblNextClass[0], LV_ALIGN_CENTER, 80, 20);
    lv_label_set_text(lblCurrentClass[0], "");
    lv_label_set_text(lblNextClass[0], "");
    lv_obj_set_style_text_font(lblCurrentClass[0], &lv_font_chinese_16, 0);
    lv_obj_set_style_text_font(lblNextClass[0], &lv_font_chinese_16, 0);
    lblCurrentClass[1] = lv_label_create(scr_clock);
    lblNextClass[1] = lv_label_create(scr_clock);
    lv_obj_align(lblCurrentClass[1], LV_ALIGN_CENTER, -80, 20);
    lv_obj_align(lblNextClass[1], LV_ALIGN_CENTER, 80, 20);
    class_y = 20;
    lv_obj_set_style_text_font(lblCurrentClass[1], &lv_font_chinese_16, 0);
    lv_obj_set_style_text_font(lblNextClass[1], &lv_font_chinese_16, 0);
    lv_obj_set_style_opa(lblCurrentClass[1], 0, 0);
    lv_obj_set_style_opa(lblNextClass[1], 0, 0);

    lblRemaining = lv_label_create(scr_clock);
    lv_obj_set_style_text_font(lblRemaining, &lv_font_chinese_16, 0);
    lv_obj_align(lblRemaining, LV_ALIGN_CENTER, 0, 24);
    lv_label_set_text(lblRemaining, "");

    for (uint8_t i = 0; i < 4; ++i)
    {
        lblTime[i][0] = lv_label_create(scr_clock);
        lv_obj_set_style_text_font(lblTime[i][0], &num_64px, 0);
        lv_label_set_text(lblTime[i][0], "0");
        lblTime[i][1] = lv_label_create(scr_clock);
        lv_obj_set_style_text_font(lblTime[i][1], &num_64px, 0);
        lv_label_set_text(lblTime[i][1], "");
        lv_obj_set_x(lblTime[i][0], 40 + 32 * i);
        lv_obj_set_y(lblTime[i][0], 120 - 32 - 32);
        lv_obj_set_x(lblTime[i][1], 40 + 32 * i);
        lv_obj_set_y(lblTime[i][1], label_y[i] = 120 - 32 - 32); //暂存y,之后动画要用到
        lv_obj_set_style_text_color(lblTime[i][0], lv_color_white(), 0);
        lv_obj_set_style_text_color(lblTime[i][1], lv_color_white(), 0);
    }
    for (uint8_t i = 4; i < 6; ++i)
    {
        lblTime[i][0] = lv_label_create(scr_clock);
        lv_obj_set_style_text_font(lblTime[i][0], &num_32px, 0);
        lv_label_set_text(lblTime[i][0], "0");
        lblTime[i][1] = lv_label_create(scr_clock);
        lv_obj_set_style_text_font(lblTime[i][1], &num_32px, 0);
        lv_label_set_text(lblTime[i][1], "");
        lv_obj_set_x(lblTime[i][0], 104 + 16 * i);
        lv_obj_set_y(lblTime[i][0], 120 - 32 - 32 + 16 + 4);
        lv_obj_set_x(lblTime[i][1], 104 + 16 * i);
        lv_obj_set_y(lblTime[i][1], label_y[i] = 120 - 32 - 32 + 16 + 4);
        lv_obj_set_style_text_color(lblTime[i][0], lv_color_white(), 0);
        lv_obj_set_style_text_color(lblTime[i][1], lv_color_white(), 0);
    }
    for (uint8_t i = 0; i < 6; ++i)
    {
        lv_label_set_text_fmt(lblTime[i][0], "%d", digits[i] % 10);
    }
    lblDate = lv_label_create(scr_clock);
    lv_obj_set_style_text_font(lblDate, &lv_font_chinese_16, 0);
    lv_obj_align(lblDate, LV_ALIGN_CENTER, 0, -88);
    lv_obj_set_style_text_color(lblDate, lv_color_white(), 0);
    noanimfirst = true;
    swap_class(hal.rtc.getMinute() + hal.rtc.getHour() * 60);
    RELEASELV();

    hal.fLoop = wf_clock_loop;
}