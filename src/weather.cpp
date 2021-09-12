#include "A_config.h"
#include <HTTPClient.h>
Weather weather;
/**
 * @brief 移动stream，直到找到某个字符串
 * @return 是否找到这个字符串
 */
/*
bool gotoStr(WiFiClient &stream, String str)
{
    char *strchr = (char *)malloc(str.length() + 1);
    char *strqueue = (char *)malloc(str.length() + 1); //查找队列
    int16_t strlength = str.length();
    int8_t queuehead = 0;    //队列头指针
    int8_t queuereadptr = 0; //队列读取指针
    uint8_t i;               //目标字符串位置指针
    bool found = false;
    bool notfound = false;
    char c;
    strcpy(strchr, str.c_str());
    while (stream.available())
    {
        stream.readBytes(&c, 1);
        strchr[queuehead] = c;
        ++queuehead;
        if (queuehead == strlength)
            queuehead = 0;
        queuereadptr = queuehead;
        notfound = false;
        i = 0;
        while (i < strlength)
        {
            if (strqueue[queuereadptr] != strchr[i])
            {
                notfound = true;
                break;
            }
            ++queuereadptr;
            ++i;
            if (queuereadptr == strlength)
                queuereadptr = 0;
        }
        if (notfound == false)
        {
            found = true;
            break;
        }
    }
    free(strchr);
    free(strqueue);
    return found;
}*/

void Weather::begin()
{
    started = false;
    File file = SPIFFS.open("/weather.bin", "r");
    if (!file)
        return;
    if (file.readBytes((char *)&hour24, sizeof(hour24)) == sizeof(hour24))
    {
        file.readBytes((char *)&desc1, sizeof(desc1));
        file.readBytes((char *)&desc2, sizeof(desc2));
        if (file.readBytes((char *)&rain, sizeof(rain)) == sizeof(rain))
        {
            started = true;
        }
    }
    file.close();
}

int8_t Weather::refresh(String location)
{
    if (!hal.connectWiFi())
    {
        Serial.println("WiFi连接失败");
        return -2;
    }
    HTTPClient http;
    hal.DoNotSleep = true;
    http.addHeader("Accept", "*/*");
    http.addHeader("User-Agent", "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.131 Safari/537.36");
    http.begin("http://api.caiyunapp.com/v2.5/96Ly7wgKGq6FhllM/" + location + "/weather.jsonp?hourlysteps=120&unit=metric:v2"); //HTTP
    //http.begin("http://api.caiyunapp.com/v2.5/96Ly7wgKGq6FhllM/116.0684%2C39.4978/weather.jsonp?unit=metric%3Av2"); //HTTP
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        String payload;
        DynamicJsonDocument doc(10240);
        WiFiClient &stream = http.getStream();
        stream.readStringUntil('"');
        stream.readStringUntil('"');
        stream.readStringUntil('"');
        payload = stream.readStringUntil('"');
        if (payload != "ok")
        {
            Serial.println("API已失效");
            Serial.println(payload);
            hal.DoNotSleep = false;
            http.end();
            return -3;
        }
        if (!stream.find("result"))
        {
            goto error_cannot_find_str;
        }
        if (!stream.find("\"minutely\":"))
        {
            goto error_cannot_find_str;
        }
        payload = stream.readStringUntil('}');
        payload = payload + "}";
        deserializeJson(doc, payload);
        for (uint8_t i = 0; i < 120; ++i)
        {
            rain[i] = doc["precipitation_2h"][i].as<float>() * 100;
        }
        strcpy(desc2, doc["description"].as<String>().c_str());
        if (!stream.find("\"description\":\""))
        {
            goto error_cannot_find_str;
        }
        payload = stream.readStringUntil('"');
        payload = "{\"str\":\"" + payload + "\"}";
        deserializeJson(doc, payload);
        strcpy(desc1, doc["str"].as<String>().c_str());
        //温度
        if (!stream.find("\"temperature\""))
        {
            goto error_cannot_find_str;
        }
        stream.readStringUntil('[');
        payload = stream.readStringUntil(']');
        payload = "{\"arr\":[" + payload + "]}";
        deserializeJson(doc, payload);
        for (uint8_t i = 0; i < 120; ++i)
        {
            String timestr;
            timestr = doc["arr"][i]["datetime"].as<String>();
            timestr = timestr.substring(5, 13);
            strcpy(hour24[i].date, timestr.c_str());
            hour24[i].temperature = int16_t(doc["arr"][i]["value"].as<float>() * 10);
        }
        //风力
        stream.readStringUntil('[');
        payload = stream.readStringUntil(']');
        payload = "{\"arr\":[" + payload + "]}";
        deserializeJson(doc, payload);
        for (uint8_t i = 0; i < 120; ++i)
        {
            hour24[i].winddirection = uint16_t(doc["arr"][i]["direction"].as<float>());
            hour24[i].windspeed = uint16_t(doc["arr"][i]["speed"].as<float>() * 10);
        }
        //天气类型
        if (!stream.find("skycon"))
        {
            goto error_cannot_find_str;
        }
        stream.readStringUntil('[');
        payload = stream.readStringUntil(']');
        payload = "{\"arr\":[" + payload + "]}";
        deserializeJson(doc, payload);
        for (uint8_t i = 0; i < 120; ++i)
        {
            String s = doc["arr"][i]["value"].as<String>();
            hour24[i].weathernum = codeToNum(s.c_str());
        }
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        hal.DoNotSleep = false;
        return -3;
    }
    http.end();
    hal.DoNotSleep = false;
    started = true;
    save();
    return 0;
error_cannot_find_str:
    Serial.println("找不到指定字符串");
    hal.DoNotSleep = false;
    http.end();
    return -4;
}

void Weather::save()
{
    File file = SPIFFS.open("/weather.bin", "w");
    if (!file)
        return;
    file.write((uint8_t *)&hour24, sizeof(hour24));
    file.write((uint8_t *)&desc1, sizeof(desc1));
    file.write((uint8_t *)&desc2, sizeof(desc2));
    file.write((uint8_t *)&rain, sizeof(rain));
    file.close();
}

weatherInfo24H *Weather::getWeather(uint8_t month, uint8_t date, uint8_t hour)
{
    if (started == false)
        return NULL;
    char strdate[9];
    sprintf(strdate, "%02d-%02dT%02d", month, date, hour);
    for (uint8_t i = 0; i < 48; ++i)
    {
        if (strcmp(strdate, hour24[i].date) == 0)
        {
            return &hour24[i];
        }
    }
    return NULL;
}
