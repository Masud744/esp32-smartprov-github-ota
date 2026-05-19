#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <Preferences.h>

#include "ota_manager.h"
#include "../version/version_manager.h"

#define VERSION_URL "https://raw.githubusercontent.com/Masud744/esp32-smartprov-github-ota/main/firmware/latest/version.json"

Preferences otaPrefs;
bool otaRunning = false;
int retryCount = 0;
#define MAX_OTA_RETRY 3

unsigned long lastCheck = 0;

void OTAManager::begin()
{
    otaPrefs.begin("ota", false);
    otaPrefs.clear();
    retryCount = otaPrefs.getInt("retry", 0);

    Serial.println();
    Serial.println("[OTA] Ready");
    Serial.print("Retry Count: ");
    Serial.println(retryCount);
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

    if (otaRunning)
        return;

    if (retryCount >= MAX_OTA_RETRY)
    {
        Serial.println("[OTA] Retry limit reached");
        return;
    }

    otaRunning = true;
    retryCount++;
    otaPrefs.putInt("retry", retryCount);

    Serial.println("[OTA] Downloading...");

    WiFiClient client;
    http.begin(firmwareURL);
    int fw_code = http.GET();

    if (fw_code != HTTP_CODE_OK)
    {
        Serial.println("UPDATE FAILED");
        Serial.println("Firmware download failed");
        otaRunning = false;
        http.end();
        return;
    }

    int len = http.getSize();
    bool ok = Update.begin(len);

    if (!ok)
    {
        Serial.println("UPDATE FAILED");
        Serial.println("Not enough OTA space");
        otaRunning = false;
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
        Serial.println("UPDATE FAILED");
        Serial.println("OTA partial");
        otaRunning = false;
    }

    if (Update.end())
    {
        if (Update.isFinished())
        {
            Serial.println("UPDATE SUCCESS");
            otaPrefs.putInt("retry", 0);
            ESP.restart();
        }
        else
        {
            Serial.println("UPDATE FAILED");
            otaRunning = false;
        }
    }
    else
    {
        Serial.println("UPDATE FAILED");
        otaRunning = false;
    }

    http.end();
}
