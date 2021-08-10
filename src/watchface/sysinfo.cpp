#include <A_config.h>

static uint8_t page = 0;
static char *strInfo;

void wf_sysinfo_loop()
{
    uint8_t day, h, m, s, dummy;
    bool dateorday;
    uint32_t step_count;
    switch (page)
    {
    case 0:
        sprintf(strInfo, "硬件信息\n可用动态内存:%d B\n运行时间:%ds\n当前CPU频率:%dMHz\n当前Flash频率:%uMHz\n主板温度:%f C\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT),
                millis() / 1000, ESP.getCpuFreqMHz(), ESP.getFlashChipSpeed() / 1000000L, hal.rtc.getTemperature());
        break;
    case 1:
        hal.rtc.getA1Time(day, h, m, s, dummy, dateorday);
        step_count = hal.acc.readByte(0x15) + 256 * (int)hal.acc.readByte(0x16) + 65536 * (int)hal.acc.readByte(0x17);
        sprintf(strInfo, "外设信息\nRTC运行状态:%s\n当前闹钟:%s%d-%d:%d:%d\n加速度计:%s\n总步数:%d\n", (hal.rtc.oscillatorCheck() ? "正常" : "不可靠"), (dateorday ? "星期" : ""),
                day, h, m, s, hal.acc.readPowerMode() == 0x02?"正常":"省电", step_count);
        break;
    default:
        break;
    }
    if (hal.btnEnter.isPressedRaw())
    {
        free(strInfo);
        popWatchFace();
        return;
    }
    vTaskDelay(1000);
}

void wf_sysinfo_load()
{
    strInfo = (char *)malloc(240);
    hal.fLoop = wf_sysinfo_loop;
}