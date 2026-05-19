#include <Arduino.h>
#include <DHT.h>

#include "dht_manager.h"
#include "app_config.h"

DHT dht(DHT_PIN, DHT_TYPE);

unsigned long lastRead = 0;

void DHTManager::begin()
{

    dht.begin();

    Serial.println();
    Serial.println("[DHT] Initialized");
}

void DHTManager::update()
{

    if (millis() - lastRead < 5000)
        return;

    lastRead = millis();

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (isnan(temp))
    {

        Serial.println("[DHT] Read Failed");

        return;
    }

    Serial.println();

    Serial.print("[DHT] Temp: ");

    Serial.print(temp);

    Serial.println(" C ");

    Serial.println("%");
}