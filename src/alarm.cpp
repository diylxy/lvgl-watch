#include "A_config.h"
const char *class_names[] =
    {
        "数学",
        "语文",
        "英语",
        "物理",
        "化学",
        "生物",
        "历史",
        "地理",
        "政治",
        "通用",
        "信息",
        "音美",
        "体育",
        "自习",
        "班会",
        "",
};
const char *week_name[] =
    {
        "----",
        "周一",
        "周二",
        "周三",
        "周四",
        "周五",
        "周六",
        "周日",
};
alarm_file_t alarms;
RTC_DATA_ATTR alarm_t *current_alarm;

void alarm_erase()
{
    memset(&alarms, 0, sizeof(alarm_file_t));
}
void alarm_sort()
{
    int i, j;
    alarm_t alarm;
    uint16_t q1;
    alarms.alarm_count = 0;
    for (i = 0; i < 200; ++i)
    {
        if (alarms.alarm[i].type != ALARM_NONE && alarms.alarm[i].type != 0xff)
        {
            ++alarms.alarm_count;
            alarms.alarm[i].q = (int)alarms.alarm[i].week * 24 * 60 + (int)alarms.alarm[i].time_start;
        }
        else
        {
            memset(&alarms.alarm[i], 0, sizeof(alarm_t));
            alarms.alarm[i].q = 0xffff;
        }
    }
    for (i = 1; i < 200; i++)
    {
        alarm = alarms.alarm[i];
        q1 = alarms.alarm[i].q;
        for (j = i - 1; j >= 0 && alarms.alarm[j].q > q1; j--)
        {
            alarms.alarm[j + 1] = alarms.alarm[j];
            alarms.alarm[j + 1].q = alarms.alarm[j].q;
        }
        alarms.alarm[j + 1] = alarm;
        alarms.alarm[j + 1].q = q1;
    }
}

alarm_t *alarm_add(alarm_type type, const char * subtype, uint8_t week, uint16_t time_start, uint16_t time_end)
{
    if (alarms.alarm_count >= 200)
        return NULL;
    alarms.alarm[alarms.alarm_count].type = type;
    strcpy(alarms.alarm[alarms.alarm_count].subtype, subtype);
    alarms.alarm[alarms.alarm_count].week = week;
    alarms.alarm[alarms.alarm_count].time_start = time_start;
    alarms.alarm[alarms.alarm_count].time_end = time_end;
    alarms.alarm_count++;
    return &alarms.alarm[alarms.alarm_count - 1];
}

void alarm_del(uint8_t week, uint16_t time_start, uint16_t time_end)
{
    if (alarms.alarm_count == 0)
    {
        return;
    }
    for (uint8_t i = 0; i < alarms.alarm_count; ++i)
    {
        if (alarms.alarm[i].type != ALARM_NONE)
        {
            if (alarms.alarm[i].week == week && alarms.alarm[i].time_start == time_start && alarms.alarm[i].time_end == time_end)
            {
                alarms.alarm[i].type = ALARM_NONE;
                alarm_sort();
                return;
            }
        }
    }
}

//获取下个闹钟
alarm_t *alarm_get_next(uint8_t week, uint16_t now)
{
    bool flagInClass = false; //用于标识正在上课，如果正在上课，但是发现一个闹钟在课程结束时间之前，则返回这个闹钟
    uint16_t q1 = (int)week * 24 * 60 + (int)now;
    alarm_t dummy = {ALARM_CLASS, "Dummy", week, 9999, 9999, 0xffff};
    alarm_t *nearest = &dummy;
    for (uint8_t i = 0; i < alarms.alarm_count; ++i)
    {
        if (alarms.alarm[i].q <= nearest->q && alarms.alarm[i].q > q1 && alarms.alarm[i].type != ALARM_NONE)
        {
            nearest = &alarms.alarm[i];
        }
        if (alarms.alarm[i].week == week && alarms.alarm[i].time_start <= now && alarms.alarm[i].time_end > now && alarms.alarm[i].type == ALARM_CLASS && nearest->type == ALARM_CLASS)
        {
            //正在“上课”
            nearest = &alarms.alarm[i];
            flagInClass = true;
        }
        if (flagInClass == true && week == alarms.alarm[i].week && alarms.alarm[i].type != ALARM_CLASS && alarms.alarm->type != ALARM_NONE && alarms.alarm[i].time_start > nearest->time_start && alarms.alarm[i].time_start < nearest->time_end)
        {
            return &alarms.alarm[i];
        }
    }
    if (nearest != &dummy)
        return nearest;
    return NULL;
}
//获取下一节课
//我也不知道为什么不能用alarm_get_next，反正这样写就能正常运行，不然无限重启
alarm_t *class_get_next(uint8_t week, uint16_t now)
{
    uint16_t q1 = (int)week * 24 * 60 + (int)now;
    alarm_t dummy = {ALARM_CLASS, "Dummy", week, 9999, 9999, 0xffff};
    alarm_t *nearest = &dummy;
    for (uint8_t i = 0; i < alarms.alarm_count; ++i)
    {
        if (alarms.alarm[i].type != ALARM_CLASS)
            continue;
        if (alarms.alarm[i].q <= nearest->q && alarms.alarm[i].q > q1 && alarms.alarm[i].type != ALARM_NONE)
        {
            nearest = &alarms.alarm[i];
        }
        if (alarms.alarm[i].week == week && alarms.alarm[i].time_start <= now && alarms.alarm[i].time_end > now && alarms.alarm[i].type != ALARM_NONE)
        {
            return &alarms.alarm[i];
        }
    }
    if (nearest != &dummy)
        return nearest;
    return NULL;
}

//此函数同get_next，但是不会返回正在进行的课程
alarm_t *class_get_next_no_curr(uint8_t week, uint16_t now)
{
    alarm_t dummy = {ALARM_CLASS, "Dummy", 8, 9999, 9999, 0xffff};
    uint16_t q1 = (int)week * 24 * 60 + (int)now;
    alarm_t *nearest = &dummy;
    for (uint8_t i = 0; i < alarms.alarm_count; ++i)
    {
        if (alarms.alarm[i].type != ALARM_CLASS)
            continue;
        if (alarms.alarm[i].week == week && alarms.alarm[i].time_start <= now && alarms.alarm[i].time_end > now)
        {
            continue;
        }
        if (alarms.alarm[i].q <= nearest->q && alarms.alarm[i].q > q1)
        {
            nearest = &alarms.alarm[i];
        }
    }
    if (nearest != &dummy)
        return nearest;
    return NULL;
}

alarm_t *class_get_curr(uint8_t week, uint16_t now)
{
    uint16_t qn = (int)week * 24 * 60 + (int)now;
    alarm_t dummy = {ALARM_CLASS, 0, 8, 0, 0, 0};
    alarm_t *nearest = &dummy;
    for (uint8_t i = 0; i < alarms.alarm_count; ++i)
    {
        //判断，找出下课时间最近的class，且已经上完
        if (alarms.alarm[i].type != ALARM_CLASS)
            continue;
        uint16_t qi, qc;
        qi = (int)alarms.alarm[i].week * 60 * 24 + alarms.alarm[i].time_end;        //当前正在判断的闹钟
        qc = (int)nearest->week * 60 * 24 + nearest->time_end;                      //目前最接近的闹钟
        if (qi <= qn && qi > qc)
        {
            nearest = &alarms.alarm[i];
        }
        if (alarms.alarm[i].week == week && alarms.alarm[i].time_start <= now && alarms.alarm[i].time_end > now)
        {
            return &alarms.alarm[i];
        }
    }
    if (nearest != &dummy && nearest->week == week)
        return nearest;
    return NULL;
}

alarm_t *alarm_get_today(uint8_t week, uint8_t num)
{
    for (uint8_t i = 0; i < alarms.alarm_count; ++i)
    {
        if (alarms.alarm[i].week == week && alarms.alarm[i].type != ALARM_NONE)
        {
            if (num == 0)
            {
                return &alarms.alarm[i];
            }
            --num;
        }
    }
    return NULL;
}

void alarm_update()
{
    uint16_t now_min = hal.rtc.getHour() * 60 + hal.rtc.getMinute();
    if (hal.rtc.getSecond() >= 49)
        ++now_min;
    uint8_t week = hal.rtc.getDoW();
    alarm_t *a = alarm_get_next(week, now_min);
    if (a == NULL)
    {
        //今天到周末没有任何闹钟
        hal.canDeepSleepFromAlarm = true;
        a = alarm_get_next(1, 0);
        if (a == NULL)
            hal.rtc.turnOffAlarm(1);
        else
        {
            hal.rtc.turnOnAlarm(1);
            hal.rtc.setA1Time(a->week, a->time_start / 60, a->time_start % 60, 0, 0, true, false, false);
            current_alarm = a;
        }
    }
    else
    {
        uint16_t t;      //闹钟开始时间，单位分钟
        uint8_t t1 = 50; //闹钟开始时的秒数
        if (a->type == ALARM_USER)
        {
            t = a->time_start;
            ++t;
            t1 = 0;
        }
        else if (a->type == ALARM_COUNTDOWN)
        {
            t = a->time_start;
            t1 = 40;
        }
        else if (a->time_start > now_min)
        {
            t = a->time_start;
        }
        else
        {
            t = a->time_end;
        }
        if (t != 0)
            --t;
        hal.rtc.turnOnAlarm(1);
        hal.rtc.setA1Time(a->week, t / 60, t % 60, t1, 0, true, false, false);
        current_alarm = a;
        if(a->week == week)
        {
            hal.canDeepSleepFromAlarm = false;
        }
        else
        {
            hal.canDeepSleepFromAlarm = true;
        }
    }
}
/**
 * @brief 检查闹钟是否触发，如果是则弹出对话框并更新闹钟
 */
void alarm_check()
{
    uint16_t now_min = hal.rtc.getHour() * 60 + hal.rtc.getMinute();
    if (hal.rtc.checkIfAlarm(1))
    {
        if (current_alarm == NULL)
        {
        }
        else if (current_alarm->type == ALARM_CLASS)
        {
            if (current_alarm->time_start > now_min)
            {
                VIBRATE_SEQ();
                full_screen_msgbox(BIG_SYMBOL_BELL, "上课提醒", "上课铃将于10秒后响起，请做好准备", lv_palette_main(LV_PALETTE_ORANGE), 3000);
            }
            else
            {
                VIBRATE_SEQ();
                full_screen_msgbox(BIG_SYMBOL_INFO, "下课提醒", "下课铃将于10秒后响起，请做好准备", lv_palette_main(LV_PALETTE_GREEN), 3000);
            }
        }
        else if (current_alarm->type == ALARM_USER)
        {
            uint8_t c = 0; //振动计数器，如果一直未按下按键则自动退出
            hal.DoNotSleep = true;
            lv_obj_t *msg = full_screen_msgbox_create(BIG_SYMBOL_BELL, "自定义闹钟", "按下右侧两按键退出", FULL_SCREEN_BG_BELL);
            while (!(hal.btnUp.isPressedRaw() && hal.btnDown.isPressedRaw()))
            {
                VIBRATE_SEQ();
                vTaskDelay(1000);
                c++;
                if(c >= ALARM_AUTO_STOP)break;
            }
            full_screen_msgbox_del(msg);
            hal.DoNotSleep = false;
        }
        else
        {
            countdown();
        }
        alarm_update();
    }
}

bool alarm_save()
{
    File file = SPIFFS.open("/alarm.bin", FILE_WRITE);
    if (!file)
    {
        Serial.println("无法打开闹钟配置文件");
        return false;
    }
    file.write((uint8_t *)&alarms, sizeof(alarms));
    file.close();
    return true;
}

bool alarm_load()
{
    File file = SPIFFS.open("/alarm.bin", FILE_READ);
    if (!file)
    {
        Serial.println("无法打开闹钟配置文件");
        return false;
    }
    file.read((uint8_t *)&alarms, sizeof(alarms));
    file.close();
    return true;
}