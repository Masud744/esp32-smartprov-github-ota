#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
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

    if (millis() - lastCheck < 30000)
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

        Serial.println("[OTA] Request Failed");

        http.end();

        return;
    }

    String payload = http.getString();

    http.end();

    JsonDocument doc;

    deserializeJson(doc, payload);

    String remote = doc["version"];

    Serial.print("Current: ");

    Serial.println(FW_VERSION);

    Serial.print("Remote: ");

    Serial.println(remote);

    if (remote != FW_VERSION)
    {

        Serial.println();

        Serial.println("UPDATE AVAILABLE");
    }
    else
    {

        Serial.println();

        Serial.println("UP TO DATE");
    }
}