#include "A_config.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

WebServer server(80);
static lv_obj_t *scr_webserver;
static File uploadFile;
static void handleGetTimetable()
{
    String contentType = "application/octet-stream";
    File file = SPIFFS.open("/alarm.bin", "r");
    if (file)
    {
        server.streamFile(file, contentType);
        file.close();
    }
    else
    {
        server.send(404, "text/plain", "No Such File");
    }
}
static void handleGetConfig()
{
    String contentType = "application/octet-stream";
    File file = SPIFFS.open(CONFFILE, "r");
    if (file)
    {
        server.streamFile(file, contentType);
        file.close();
    }
    else
    {
        server.send(404, "text/plain", "No Such File");
    }
}
static void handleFileUpload()
{
    HTTPUpload &upload = server.upload();
    String filename = upload.filename;
    if (filename != "alarm.bin" && filename != CONFFILE)
        return;
    if (upload.status == UPLOAD_FILE_START)
    {
        if (SPIFFS.exists("/" + filename))
        {
            SPIFFS.remove("/" + filename);
        }
        uploadFile = SPIFFS.open("/" + filename, FILE_WRITE);
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (uploadFile)
        {
            uploadFile.write(upload.buf, upload.currentSize);
        }
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (uploadFile)
        {
            uploadFile.close();
            alarm_load();
            alarm_sort();
            alarm_update();
            alarm_save();
        }
    }
}
void handleRoot()
{
    hal.conf.handleFormRequest(&server);
}

static void handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

static void wf_webserver_loop(void)
{
    server.handleClient();
    vTaskDelay(2);
    if (hal.btnEnter.isPressedRaw())
    {
        server.stop();
        WiFi.mode(WIFI_OFF);
        hal.DoNotSleep = false;
        popWatchFace();
        return;
    }
}

void wf_webserver_load(void)
{
    lv_obj_t *msgbox_full;
    String buf;
    bool sap = false;
    hal.DoNotSleep = true;
    REQUESTLV();
    scr_webserver = lv_obj_create(NULL);
    lv_scr_load(scr_webserver);
    RELEASELV();
    if (!msgbox_yn("直接使用软AP-确定\n尝试连接WiFi-取消"))
    {
        //BUG: 无法使用WiFi
        while (hal.btnEnter.isPressedRaw())
            vTaskDelay(10);
        msgbox_full = full_screen_msgbox_create(BIG_SYMBOL_SYNC, "WiFi", "正在连接WiFi，长按确定键取消", FULL_SCREEN_BG_SYNC);
        if (!hal.connectWiFi())
        {
            full_screen_msgbox_del(msgbox_full);
            while (hal.btnEnter.isPressedRaw())
                vTaskDelay(10);
            full_screen_msgbox(BIG_SYMBOL_CROSS, "WiFi", "连接失败，已启动软AP：OpenSmartwatch", FULL_SCREEN_BG_CROSS);
            WiFi.softAP("OpenSmartwatch", WIFI_AP_PASSWORD);
            sap = true;
        }
        else
        {
            full_screen_msgbox_del(msgbox_full);
            while (hal.btnEnter.isPressedRaw())
                vTaskDelay(10);
            full_screen_msgbox(BIG_SYMBOL_CHECK, "WiFi", "连接成功", FULL_SCREEN_BG_CHECK);
        }
    }
    else
    {
        WiFi.softAP("OpenSmartwatch", WIFI_AP_PASSWORD);
        full_screen_msgbox(BIG_SYMBOL_CHECK, "SoftAP", "已启动软AP：OpenSmartwatch", FULL_SCREEN_BG_CHECK);
        sap = true;
    }

    server.on("/", handleRoot);
    server.on("/alarm.bin", handleGetTimetable);
    server.on(CONFFILE, handleGetConfig);
    server.on(
        "/upload", HTTP_POST, []()
        {
            if (server.method() != HTTP_POST)
            {
                server.send(400, "text/plain", "Bad Request");
                return;
            }
            server.sendHeader("Connection", "close");
            server.send(200, "text/plain", "File Sent. (");
        },
        handleFileUpload);

    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");
    if (sap)
    {
        WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
    }
    buf = (sap ? WiFi.softAPIP().toString() : WiFi.localIP().toString()) + "\nWiFi: " + (sap ? "OpenSmartwatch" : WiFi.SSID()) + "\n确保连接到同一WiFi\n并访问此地址";
    label(buf.c_str(), 40, 40, true, 0);
    hal.DoNotSleep = true;

    hal.fLoop = wf_webserver_loop;
}
