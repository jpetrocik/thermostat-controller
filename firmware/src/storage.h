#ifndef STORAGE_H
#define STORAGE_H

#include "configuration.h"

// Save the given DeviceConfig to LittleFS at /config.json. Returns true on success.
bool saveDeviceConfig(const DeviceConfig &cfg);

// Load DeviceConfig from LittleFS /config.json into cfg. Returns true on success.
bool loadDeviceConfig(DeviceConfig *cfg);

#endif // STORAGE_H
