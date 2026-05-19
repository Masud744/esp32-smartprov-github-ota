# Changelog

All notable changes to this project will be documented in this file.

## [v2.2.0] - 2026-05-20
### Added
- **Industrial OTA Rollback Mechanism:** ESP32 now automatically rolls back to the previous active partition if the new firmware crashes consecutively for 3 times (due to boot failures).
- **Infinite Loop Prevention:** Solved the "Zombie Update" bug. After a rollback, the firmware version is recorded in NVS as a ejected_version. The device blocks future update attempts for this specific faulty version.
- **Dual Codebase Synchronization:** Fully synced the src/ modular workspace with the smart_weather_node_v1/ Arduino IDE workspace.
- **Detailed Documentation:** Overhauled README.md to professionally present the Industrial OTA architecture, data flow, and failsafes.

## [v2.1.0] - 2026-05-19
### Added
- **Stream-based OTA Download:** Migrated from httpUpdate to Update.writeStream() to fix connection drops during the download of large (~1MB) firmware payloads.
- **NVS Retry Validation:** Integrated ESP32 Preferences.h to track OTA retries. Caps automated retries at MAX_OTA_RETRY = 3 to save bandwidth and power.
- **Full DHT Framework:** Added Humidity measurement functions alongside Temperature using the DHT API.

## [v1.0.0] - 2026-05-15
### Added
- **Core Initialization:** Initial project setup for ESP32.
- **SmartProv BLE/WiFi:** Integrated local WiFi credential setup via ESP SmartProv.
- **Temperature Monitoring:** Initial capability to log Temperature data.
- **Raw GitHub JSON Parsing:** Set up standard HTTPS fetch for irmware/latest/version.json to initiate OTA.
