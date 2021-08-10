#include "A_config.h"

void configManager::loadConfigs()
{
    File file = SPIFFS.open("/config.json", "r");
    if(!file)
    {
        jsonstr = "{}";
        return;
    }
    jsonstr = file.readString();
    file.close();
}

void configManager::saveConfigs()
{
    File file=SPIFFS.open("/config.json", "w");
    if(!file)
    {
        return;
    }
    serializeJson(doc, jsonstr);
    file.write((const uint8_t *)jsonstr.c_str(), jsonstr.length());
    file.close();
}

String configManager::getConfigText()
{
    return this->jsonstr;
}

void configManager::setConfigText(String jsonstr)
{
    this->jsonstr = jsonstr;
    deserializeJson(doc, jsonstr);
    saveConfigs();
}