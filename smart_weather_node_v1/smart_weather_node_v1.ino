#include "app_config.h"
#include "provision_manager.h"

void setup() {

   Serial.begin(SERIAL_BAUD);

   delay(1000);

   ProvisionManager::begin();

}

void loop() {

   ProvisionManager::update();

}