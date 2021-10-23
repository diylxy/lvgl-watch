#include <A_config.h>

NimBLEAddress scan_addrs[30];
NimBLEScan *pBLEScan;
uint8_t ptr_scan_addr;
NimBLEClient *pClient = nullptr;

class MyAdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks
{
    void onResult(NimBLEAdvertisedDevice *advertisedDevice)
    {
        auto str = advertisedDevice->getName();
        if (str.size() <= 1)
        {
            str = advertisedDevice->getAddress().toString();
        }
        menu_add(str.c_str());
        scan_addrs[ptr_scan_addr] = advertisedDevice->getAddress();
        ++ptr_scan_addr;
    }
};

class ClientCallbacks : public NimBLEClientCallbacks
{
    void onConnect(NimBLEClient *pClient)
    {
        /** After connection we should change the parameters if we don't need fast response times.
         *  These settings are 150ms interval, 0 latency, 450ms timout.
         *  Timeout should be a multiple of the interval, minimum is 100ms.
         *  I find a multiple of 3-5 * the interval works best for quick response/reconnect.
         *  Min interval: 120 * 1.25ms = 150, Max interval: 120 * 1.25ms = 150, 0 latency, 60 * 10ms = 600ms timeout
         */
        pClient->updateConnParams(120, 120, 0, 100);
    };

    void onDisconnect(NimBLEClient *pClient){};

    /** Called when the peripheral requests a change to the connection parameters.
     *  Return true to accept and apply them or false to reject and keep
     *  the currently used parameters. Default will return true.
     */
    bool onConnParamsUpdateRequest(NimBLEClient *pClient, const ble_gap_upd_params *params)
    {
        if (params->itvl_min < 24)
        { /** 1.25ms units */
            return false;
        }
        else if (params->itvl_max > 40)
        { /** 1.25ms units */
            return false;
        }
        else if (params->latency > 2)
        { /** Number of intervals allowed to skip */
            return false;
        }
        else if (params->supervision_timeout > 100)
        { /** 10ms units */
            return false;
        }

        return true;
    };

    /********************* Security handled here **********************
    ****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest()
    {
        return BLEGATEWAYPASS;
    };

    bool onConfirmPIN(uint32_t pass_key)
    {
        return true;
    };

    /** Pairing process complete, we can check the results in ble_gap_conn_desc */
    void onAuthenticationComplete(ble_gap_conn_desc *desc)
    {
        if (!desc->sec_state.encrypted)
        {
            Serial.println("Encrypt connection failed - disconnecting");
            /** Find the client with the connection handle provided in desc */
            NimBLEDevice::getClientByID(desc->conn_handle)->disconnect();
            return;
        }
    };
};

static void notifyCB(NimBLERemoteCharacteristic *pRemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
{
    std::string str = (isNotify == true) ? "Notification" : "Indication";
    str += " from ";
    /** NimBLEAddress and NimBLEUUID have std::string operators */
    str += std::string(pRemoteCharacteristic->getRemoteService()->getClient()->getPeerAddress());
    str += ": Service = " + std::string(pRemoteCharacteristic->getRemoteService()->getUUID());
    str += ", Characteristic = " + std::string(pRemoteCharacteristic->getUUID());
    str += ", Value = " + std::string((char *)pData, length);
    Serial.println(str.c_str());
}

/** Create a single global instance of the callback class to be used by all clients */
static ClientCallbacks clientCB;

void ble_init()
{
    NimBLEDevice::setScanDuplicateCacheSize(200);
    NimBLEDevice::init("OpenSmartwatch");
    pClient = nullptr;
}
void ble_deinit()
{
    //ble_disconnect();
    NimBLEDevice::deinit(true);
    btStop();
    pClient = nullptr;
}
void ble_disconnect()
{
    if (!pClient)
        return;
    pClient->disconnect();
}

bool ble_connect(NimBLEAddress client_addr)
{

    /** 检查是否有可复用的客户端实例 **/
    if (NimBLEDevice::getClientListSize())
    {
        //pClient = NimBLEDevice::getClientByPeerAddress(client_addr);
        if (pClient && pClient->getPeerAddress() == client_addr)
        {
            //尝试重连
            if (!pClient->connect(client_addr, false))
            {
                return false;
            }
        }
        /** 
         * 查找一个已经断开连接的设备
         */
        else
        {
            pClient = NimBLEDevice::getDisconnectedClient();
        }
    }

    /** 如果还是没有可复用的设备，就创建一个 */
    if (!pClient)
    {
        if (NimBLEDevice::getClientListSize() >= NIMBLE_MAX_CONNECTIONS)
        {
            Serial.println("错误：已连接的BLE客户端过多");
            return false;
        }

        pClient = NimBLEDevice::createClient();

        pClient->setClientCallbacks(&clientCB, false);

        pClient->setConnectionParams(12, 12, 0, 51);
        /** 设置连接超时时间 5秒*/
        pClient->setConnectTimeout(5);

        if (!pClient->connect(client_addr))
        {
            /** 连接失败，删除此实例 */
            NimBLEDevice::deleteClient(pClient);
            return false;
        }
    }

    if (!pClient->isConnected())
    {
        if (!pClient->connect(client_addr))
        {
            return false;
        }
    }
    return true;
}

int ble_get_rssi()
{
    if (!pClient)
        return 0;
    return pClient->getRssi();
}

NimBLEAddress *ble_scan()
{
    bool donotsleepstat = hal.DoNotSleep;
    hal.DoNotSleep = true;
    ptr_scan_addr = 0;
    pBLEScan = NimBLEDevice::getScan(); //create new scan
    // Set the callback for when devices are discovered, no duplicates.
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), false);
    pBLEScan->setActiveScan(true); // Set active scanning, this will get more data from the advertiser.
    pBLEScan->setInterval(97);     // How often the scan occurs / switches channels; in milliseconds,
    pBLEScan->setWindow(37);       // How long to scan during the interval; in milliseconds.
    pBLEScan->setMaxResults(0);    // do not store the scan results, use callback only.
    pBLEScan->start(0, nullptr, false);
    menu_create();
    menu_display(0);
    while (!menu_handle())
        vTaskDelay(20);
    pBLEScan->stop();
    uint16_t n = menu_get_selected();
    hal.DoNotSleep = donotsleepstat;
    if (n)
    {
        --n;
    }
    else
    {
        return NULL;
    }
    Serial.println(scan_addrs[n].toString().c_str());
    return &(scan_addrs[n]);
}

String ble_read(const char *service, const char *characteristic)
{
    if (pClient == NULL)
    {
        return "";
    }
    NimBLERemoteService *pSvc = nullptr;
    NimBLERemoteCharacteristic *pChr = nullptr;
    NimBLERemoteDescriptor *pDsc = nullptr;
    pSvc = pClient->getService(service);
    if (pSvc)
    { /** make sure it's not null */
        pChr = pSvc->getCharacteristic(characteristic);

        if (pChr)
        { /** make sure it's not null */
            if (pChr->canRead())
            {
                auto str = pChr->readValue();
                return String(str.c_str());
            }
        }
    }
    return "";
}

bool ble_write(const char *service, const char *characteristic, const String str)
{
    if (pClient == NULL)
    {
        return false;
    }
    NimBLERemoteService *pSvc = nullptr;
    NimBLERemoteCharacteristic *pChr = nullptr;
    NimBLERemoteDescriptor *pDsc = nullptr;
    pSvc = pClient->getService(service);
    if (pSvc)
    { /** make sure it's not null */
        pChr = pSvc->getCharacteristic(characteristic);
        if (pChr)
        { /** make sure it's not null */
            if (pChr->canWrite())
            {
                pChr->writeValue(str.c_str());
                return true;
            }
        }
    }
    return false;
}
