#include <A_config.h>

static int8_t page = 0;
static char *strInfo;
#define SYSINFO_PAGE_CNT 2 //系统信息中页面数
static lv_obj_t *scr_sysinfo;
static lv_obj_t *lblinfo;
void wf_sysinfo_loop()
{
    uint8_t day, h, m, s, dummy;
    bool dateorday;
    if (hal.btnDown.isPressedRaw())
    {
        ++page;
        if (page == SYSINFO_PAGE_CNT)
        {
            page = 0;
        }
    }
    if (hal.btnUp.isPressedRaw())
    {
        if (page == 0)
        {
            page = SYSINFO_PAGE_CNT;
        }
        --page;
    }
    if (hal.btnEnter.isPressedRaw())
    {
        free(strInfo);
        lv_obj_fade_out(scr_sysinfo, 500, 0);
        while (hal.btnEnter.isPressedRaw())
            vTaskDelay(50);
        popWatchFace();
        return;
    }
    switch (page)
    {
    case 0:
        sprintf(strInfo, "硬件信息\n可用动态内存:%u B\n运行时间:%u s\n当前CPU频率:%u MHz\n当前Flash频率:%u MHz\n主板温度:%f C\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT),
                millis() / 1000, ESP.getCpuFreqMHz(), ESP.getFlashChipSpeed() / 1000000, hal.rtc.getTemperature());
        break;
    case 1:
        hal.rtc.getA1Time(day, h, m, s, dummy, dateorday);
        sprintf(strInfo, "外设信息\nRTC运行状态:%s\n当前闹钟:%s-%d:%d:%d\n闹钟状态:%s\n加速度计状态:%s\n", (hal.rtc.oscillatorCheck() ? "正常" : "不可靠"), 
                week_name[day], h, m, s, hal.rtc.checkAlarmEnabled(1) ? "开" : "关", hal.acc.readPowerMode() == 0x02 ? "正常" : "省电");
        break;
    default:
        page = 0;
        break;
    }
    REQUESTLV();
    lv_label_set_text(lblinfo, strInfo);
    RELEASELV();
    vTaskDelay(800);
}

void wf_sysinfo_load()
{
    while (hal.btnEnter.isPressedRaw())
        vTaskDelay(50);
    REQUESTLV();
    scr_sysinfo = lv_obj_create(NULL);
    lv_scr_load_anim(scr_sysinfo, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, true);
    lblinfo = lv_label_create(scr_sysinfo);
    lv_label_set_long_mode(lblinfo, LV_LABEL_LONG_WRAP);
    lv_obj_set_size(lblinfo, 200, LV_SIZE_CONTENT);
    lv_obj_center(lblinfo);
    lv_obj_set_style_text_font(lblinfo, &lv_font_chinese_16, 0);
    lv_obj_set_style_text_align(lblinfo, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lblinfo, "正在获取...");
    RELEASELV();
    strInfo = (char *)malloc(240);
    hal.fLoop = wf_sysinfo_loop;
}