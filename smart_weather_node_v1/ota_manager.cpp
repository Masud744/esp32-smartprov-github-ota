#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <Preferences.h>

#include "ota_manager.h"
#include "version_manager.h"

#define VERSION_URL "https://raw.githubusercontent.com/Masud744/esp32-smartprov-github-ota/main/firmware/latest/version.json"

Preferences otaPrefs;
bool otaRunning = false;
int retryCount = 0;
bool update_pending = false;
int boot_fails = 0;
#define MAX_OTA_RETRY 3

unsigned long lastCheck = 0;

void OTAManager::begin()
{
    otaPrefs.begin("ota", false);
    retryCount = otaPrefs.getInt("retry", 0);
    update_pending = otaPrefs.getBool("update_pending", false);
    boot_fails = otaPrefs.getInt("boot_fails", 0);

    Serial.println();
    Serial.println("[OTA] Ready");
    Serial.print("Retry Count: ");
    Serial.println(retryCount);

    if (update_pending)
    {
        boot_fails++;
        otaPrefs.putInt("boot_fails", boot_fails);
        Serial.printf("[OTA] Update Pending! Boot fail count: %d\n", boot_fails);

        if (boot_fails >= 3)
        {
            Serial.println("[OTA] SYSTEM UNSTABLE! Initiating Rollback...");
            if (Update.canRollBack())
            {
                otaPrefs.putString("rejected_version", FW_VERSION); // Save the buggy version
                Update.rollBack();
                otaPrefs.putBool("update_pending", false);
                otaPrefs.putInt("boot_fails", 0);
                Serial.println("[OTA] Rollback successful. Rebooting into old firmware...");
                delay(1000);
                ESP.restart();
            }
            else
            {
                Serial.println("[OTA] Rollback failed! No partition available.");
            }
        }
    }
}

void OTAManager::update()
{
    if (update_pending && WiFi.status() == WL_CONNECTED)
    {
        Serial.println("[OTA] Firmware Boot OK! Validating new version...");
        otaPrefs.putBool("update_pending", false);
        otaPrefs.putInt("boot_fails", 0);
        update_pending = false;
    }

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

    String rejected = otaPrefs.getString("rejected_version", "");
    if (remote == rejected)
    {
        Serial.print("[OTA] Update Blocked: Version ");
        Serial.print(rejected);
        Serial.println(" was previously rejected due to crash!");
        return;
    }

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
            otaPrefs.putString("last_good_version", FW_VERSION);
            otaPrefs.putBool("update_pending", true);
            otaPrefs.putInt("boot_fails", 0);
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
