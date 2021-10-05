#include <A_config.h>

void menu_maker_ac_control()
{
start:
    ir.enableIROut(true);
    menu_create();
    menu_add("开");
    menu_add("关");
    menu_add("模式");
    menu_add("温度");
    menu_add("风速");
    menu_add("型号");
    uint8_t m;
    switch (menu_show(1))
    {
    case 0:
        break;
    case 1: //开
        irac.next.power = true;
        irac.sendAc();
        goto start;
        break;
    case 2: //关
        irac.next.power = false;
        irac.sendAc();
        goto start;
        break;
    case 3: //模式
        menu_create();
        menu_add("自动");
        menu_add("制冷");
        menu_add("制热");
        menu_add("干燥");
        menu_add("送风");
        m = menu_show();
        if (m)
        {
            --m;
            irac.next.mode = (stdAc::opmode_t)m;
        }
        irac.sendAc();
        goto start;
        break;
    case 4: //温度
        m = msgbox_number("请输入温度", 2, 0, 30, 16, 24);
        irac.next.degrees = m;
        irac.sendAc();
        goto start;
        break;
    case 5: //风速
        menu_create();
        menu_add("自动");
        menu_add("最小");
        menu_add("小");
        menu_add("中");
        menu_add("大");
        menu_add("最大");
        menu_add("强劲");
        m = menu_show();
        if (m)
        {
            --m;
            if (m == 6)
            {
                m = 5;
                irac.next.turbo = true;
            }
            else
            {
                irac.next.turbo = false;
            }
            irac.next.fanspeed = (stdAc::fanspeed_t)m;
            irac.sendAc();
        }
        goto start;
        break;
    case 6: //型号
    {
        decode_type_t protocols[60];
        uint8_t tmp = 1;
        menu_create();
        for (int i = 1; i < kLastDecodeType; i++)
        {
            protocols[tmp] = (decode_type_t)i;
            if (irac.isProtocolSupported(protocols[tmp]))
            {
                menu_add(typeToString(protocols[tmp]).c_str());
                ++tmp;
            }
        }
        m = menu_show();
        if (m)
        {
            irac.next.protocol = protocols[m];
            irac.sendAc();
        }
        goto start;
        break;
    }
    default:
        break;
    }
    ir.enableIROut(false);
}