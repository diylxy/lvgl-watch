#ifndef __CONFIG_MAMAGER_H__
#define __CONFIG_MANAGER_H__
#include <Arduino.h>
#include <ArduinoJson.h>

class configManager
{
public:
    //读取配置文件到doc
    void loadConfigs();

    //存储配置文件
    void saveConfigs();

    //获取原始配置文件-json
    String getConfigText();

    //替换原始配置文件-json，并保存
    void setConfigText(String jsonstr);

    DynamicJsonDocument doc = DynamicJsonDocument(1024);
private:
    String jsonstr;
};

extern configManager config;
#endif