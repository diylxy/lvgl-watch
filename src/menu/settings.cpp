#include "menu.h"
void menu_maker_settings()
{
    lv_obj_t *msgbox_full;
    int16_t tmp;
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
        menu_add(LV_SYMBOL_SETTINGS " 秒针微调");
        menu_add(LV_SYMBOL_EDIT " 毫秒级微调");
        menu_add(LV_SYMBOL_EDIT " RTC频率微调");
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
            //秒针微调，应对学校rtc几天慢一秒的问题
            tmp = msgbox_number("偏移秒数", 2, 0, 15, -15, 0);
            if (abs(tmp) > 3)
            {
                msgbox_full = full_screen_msgbox_create(BIG_SYMBOL_SYNC,
                                                        "秒钟微调",
                                                        "正在等待RTC芯片准备好",
                                                        FULL_SCREEN_BG_SYNC);
            }
            hal.DoNotSleep = true;
            hal.rtcOffsetSecond(tmp);
            if (abs(tmp) > 3)
            {
                full_screen_msgbox_del(msgbox_full);
            }
            full_screen_msgbox(BIG_SYMBOL_CHECK, "秒针微调", "调节成功", FULL_SCREEN_BG_CHECK, 2000);
            hal.DoNotSleep = false;
            break;
        case 3:
            //毫秒级微调，应对秒钟微调也调不准的问题
            tmp = msgbox_number("偏移毫秒数", 3, 0, 900, -900, 0);
            hal.DoNotSleep = true;
            hal.rtcOffsetms(tmp);
            full_screen_msgbox(BIG_SYMBOL_CHECK, "毫秒级微调", "调节成功", FULL_SCREEN_BG_CHECK, 2000);
            hal.DoNotSleep = false;
            break;
        case 4:
            //RTC频率微调
            full_screen_msgbox(BIG_SYMBOL_INFO, "RTC频率微调",
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
}