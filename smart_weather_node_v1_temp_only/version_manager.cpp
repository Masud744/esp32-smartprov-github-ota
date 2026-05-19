#include <Arduino.h>

#include "version_manager.h"

void VersionManager::print()
{

    Serial.println();

    Serial.println("==========");

    Serial.println("FIRMWARE");

    Serial.println("==========");

    Serial.print("Version: ");

    Serial.println(FW_VERSION);
}
