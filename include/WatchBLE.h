#ifndef __WATCH_BLE_H__
#define __WATCH_BLE_H__
#include <Arduino.h>
#include "NimBLEDevice.h"
#define BLEGATEWAYPASS 123456                //BLE网关蓝牙密码，可能用不到
#define GATEWAY_UUID_WEATHER "EAEA"          //天气 service
#define GATEWAY_UUID_WEATHER_HOURLY "1E82"   //逐小时预报 characteristic
#define GATEWAY_UUID_WEATHER_MINUTELY "1E83" //分钟级降水 characteristic
#define GATEWAY_UUID_WEATHER_ADVICE1 "1E84"  //自然语言天气1 characteristic
#define GATEWAY_UUID_WEATHER_ADVICE2 "1E85"  //自然语言天气2 characteristic
/**
 * @brief 初始化蓝牙
 */
void ble_init();

/**
 * @brief 关闭蓝牙以省电
 */
void ble_deinit();

/**
 * @brief 断开连接当前设备
 */
void ble_disconnect();

/**
 * @brief 连接指定地址的设备
 * @return 如果连接成功，保持连接，返回true。否则返回false。连接超时5秒
 */
bool ble_connect(NimBLEAddress client_addr);

/**
 * @brief 获取当前连接的设备的RSSI，如果没有连接则返回0
 * @return RSSI值
 */
int ble_get_rssi();

/**
 * @brief 扫描并列出蓝牙设备列表，等待选择。需要先初始化蓝牙。
 * @return 选择的蓝牙设备的地址指针，如果没有选择，则返回NULL
 */
NimBLEAddress *ble_scan();

/**
 * @brief 读取指定characteristic的值（字符串形式）
 */
String ble_read(const char *service, const char *characteristic);

/**
 * @brief 写入指定characteristic的值（字符串形式）
 */
bool ble_write(const char *service, const char *characteristic, const String str);

#endif