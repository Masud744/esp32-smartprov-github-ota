#include <Arduino.h>
#include <SmartProv.h>

#include "provision_manager.h"
#include "../../config/version.h"

SmartProv prov;

void ProvisionManager::begin()
{

    Serial.println();
    Serial.println("==========");
    Serial.println(DEVICE_NAME);
    Serial.println("==========");

    prov.onConnected([]()
                     {
                         Serial.println();
                         Serial.println("[WIFI] Connected");

                         Serial.print("SSID: ");
                         Serial.println(prov.getSSID());

                         Serial.print("IP: ");
                         Serial.println(prov.getIP());

                         Serial.print("RSSI: ");
                         Serial.println(prov.getRSSI());

                         Serial.print("Firmware: ");
                         Serial.println(FW_VERSION);
                     });

    prov.begin();
}

void ProvisionManager::update()
{

    prov.update();
}
