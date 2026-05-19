#include "app_config.h"
#include "provision_manager.h"
#include "dht_manager.h"
#include "version_manager.h"
#include "ota_manager.h"

void setup()
{

   Serial.begin(SERIAL_BAUD);

   delay(1000);

   VersionManager::print();

   ProvisionManager::begin();
   DHTManager::begin();
   OTAManager::begin();
}

void loop()
{

   ProvisionManager::update();
   DHTManager::update();
   OTAManager::update();
}