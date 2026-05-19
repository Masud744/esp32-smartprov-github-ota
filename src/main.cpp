#include <Arduino.h>

#include "config/app_config.h"
#include "managers/provision/provision_manager.h"

void setup()
{

    Serial.begin(SERIAL_BAUD);

    delay(1000);

    ProvisionManager::begin();
}

void loop()
{

    ProvisionManager::update();
}
