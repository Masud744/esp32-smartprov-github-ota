#include <Arduino.h>

#include "config/app_config.h"
#include "managers/provision/provision_manager.h"
#include "managers/sensor/dht_manager.h"
#include "managers/version/version_manager.h"

void setup()
{

    Serial.begin(SERIAL_BAUD);

    delay(1000);

    VersionManager::print();

    ProvisionManager::begin();
    DHTManager::begin();
}

void loop()
{

    ProvisionManager::update();
    DHTManager::update();
}
