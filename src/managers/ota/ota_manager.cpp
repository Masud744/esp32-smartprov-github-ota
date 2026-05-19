#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <ArduinoJson.h>

#include "ota_manager.h"
#include "version_manager.h"

#define VERSION_URL "https://raw.githubusercontent.com/Masud744/esp32-smartprov-github-ota/main/firmware/latest/version.json"

unsigned long lastCheck = 0;

void OTAManager::begin()
{

    Serial.println();
    Serial.println("[OTA] Ready");
}

void OTAManager::update()
{

    if (millis() - lastCheck < 10000)
        return;

    lastCheck = millis();

    if (WiFi.status() != WL_CONNECTED)
        return;

    Serial.println();
    Serial.println("[OTA] Checking...");

    HTTPClient http;

    http.begin(VERSION_URL);

    int code = http.GET();

    if (code != 200)
    {

        http.end();

        return;
    }

    String payload = http.getString();

    http.end();

    JsonDocument doc;

    deserializeJson(doc, payload);

    String remote = doc["version"];
    String firmwareURL = doc["url"];

    Serial.print("Current: ");
    Serial.println(FW_VERSION);

    Serial.print("Remote: ");
    Serial.println(remote);

    if (remote == FW_VERSION)
    {

        Serial.println("UP TO DATE");

        return;
    }

    Serial.println();
    Serial.println("UPDATE AVAILABLE");

    Serial.print("Firmware URL: ");
    Serial.println(firmwareURL);

    Serial.println("[OTA] Downloading...");

    WiFiClient client;

    http.begin(firmwareURL);

    int fw_code = http.GET();

    if (fw_code != HTTP_CODE_OK)
    {
        Serial.println("Firmware download failed");
        http.end();
        return;
    }

    int len = http.getSize();

    bool ok = Update.begin(len);

    if (!ok)
    {
        Serial.println("Not enough OTA space");
        http.end();
        return;
    }

    WiFiClient *stream = http.getStreamPtr();

    size_t written = Update.writeStream(*stream);

    if (written == len)
    {
        Serial.println("OTA written");
    }
    else
    {
        Serial.println("OTA partial");
    }

    if (Update.end())
    {
        if (Update.isFinished())
        {
            Serial.println("UPDATE SUCCESS");
            ESP.restart();
        }
    }

    http.end();
}