#include "A_config.h"
#include <HTTPClient.h>
Weather weather;
void Weather::begin()
{
    started = false;
    File file = SPIFFS.open("/weather.bin", "r");
    if (!file)
        return;
    if (file.readBytes((char *)&hour24, sizeof(hour24)) == sizeof(hour24))
    {
        started = true;
    }
    file.close();
}

int8_t Weather::refresh(String cityName)
{
    if (hal.conf.getString("weatherappkey") == "")
    {
        Serial.println("未配置APPKEY");
        return -1;
    }
    if (!hal.connectWiFi())
    {
        Serial.println("WiFi连接失败");
        return -2;
    }
    HTTPClient http;
    hal.DoNotSleep = true;
    http.begin("http://way.jd.com/jisuapi/weather?city=" + cityName + "&appkey=" + hal.conf.getString("weatherappkey")); //HTTP
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        bool isnextday = false;
        uint8_t last_time = 0;
        uint8_t this_time = 0;
        String payload = http.getString();
        DynamicJsonDocument doc(10240);
        deserializeJson(doc, payload);

        String date = doc["result"]["result"]["date"];
        updateDate = date.substring(8, 10).toInt();
        for (uint8_t i = 0; i < 24; ++i)
        {
            String timestr;
            timestr = doc["result"]["result"]["hourly"][i]["time"].as<String>();
            timestr = timestr.substring(0, 2);
            if(timestr.substring(1) == ":")
            {
                timestr = timestr.substring(0, 1);
            }
            Serial.println(timestr);
            this_time = timestr.toInt();
            if (this_time < last_time)
                isnextday = true;
            hour24[this_time].weathernum = doc["result"]["result"]["hourly"][i]["img"].as<String>().toInt();
            hour24[this_time].weatherName = doc["result"]["result"]["hourly"][i]["weather"].as<String>();
            hour24[this_time].temperature = doc["result"]["result"]["hourly"][i]["temp"].as<String>().toInt();
            hour24[this_time].isnextday = isnextday;
        }
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        hal.disconnectWiFi();
        hal.DoNotSleep = false;
        return -3;
    }
    http.end();
    hal.disconnectWiFi();
    hal.DoNotSleep = false;
    started = true;
    return 0;
}

void Weather::save()
{
    File file = SPIFFS.open("/weather.bin", "w");
    if (!file)
        return;
    file.write((uint8_t *)&hour24, sizeof(hour24));
    file.close();
}

bool Weather::available(uint8_t date)
{
    return date == updateDate && started == true;
}
