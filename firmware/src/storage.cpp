#include "storage.h"

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

static const char *CONFIG_PATH = "/config.json";
static const size_t MAX_CONFIG_SIZE = 16 * 1024; // 16 KB cap

bool saveDeviceConfig(const DeviceConfig &cfg)
{
  if (!LittleFS.begin())
  {
    Serial.println("LittleFS mount failed");
    return false;
  }

  JsonDocument doc;
  doc["deviceName"] = cfg.deviceName;
  doc["roomName"] = cfg.roomName;
  doc["locationName"] = cfg.locationName;
  doc["hostname"] = cfg.hostname;
  doc["mqttHost"] = cfg.mqttHost;
  doc["wifiSsid"] = cfg.wifiSsid;
  doc["wifiPassword"] = cfg.wifiPassword;
  doc["setTemp"] = cfg.setTemp;
  doc["heatEnabled"] = cfg.heatEnabled;
  doc["hysteresis"] = cfg.hysteresis;

  File f = LittleFS.open(CONFIG_PATH, "w");
  if (!f)
  {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  if (serializeJsonPretty(doc, f) == 0)
  {
    Serial.println("Failed to write config");
    f.close();
    return false;
  }

  f.close();
  return true;
}

bool loadDeviceConfig(DeviceConfig *cfg)
{
  if (!cfg)
    return false;

  if (!LittleFS.begin(true))
  {
    Serial.println("LittleFS mount failed");
    return false;
  }

  if (!LittleFS.exists(CONFIG_PATH))
  {
    Serial.println("Config file not found");
    return false;
  }

  File f = LittleFS.open(CONFIG_PATH, "r");
  if (!f)
  {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = f.size();
  if (size == 0 || size > MAX_CONFIG_SIZE)
  {
    Serial.println("Config file size invalid");
    f.close();
    return false;
  }

  // Use a dynamic buffer sized to file size but capped
  size_t capacity = min<size_t>(size + 1, 4096);
  JsonDocument doc;

  DeserializationError err = deserializeJson(doc, f);
  f.close();
  if (err)
  {
    Serial.print("Failed to parse config: ");
    Serial.println(err.c_str());
    return false;
  }

  strlcpy(cfg->deviceName, doc["deviceName"] | cfg->deviceName, sizeof(cfg->deviceName));
  strlcpy(cfg->roomName, doc["roomName"] | cfg->roomName, sizeof(cfg->roomName));
  strlcpy(cfg->locationName, doc["locationName"] | cfg->locationName, sizeof(cfg->locationName));
  strlcpy(cfg->hostname, doc["hostname"] | cfg->hostname, sizeof(cfg->hostname));
  strlcpy(cfg->mqttHost, doc["mqttHost"] | cfg->mqttHost, sizeof(cfg->mqttHost));
  strlcpy(cfg->wifiSsid, doc["wifiSsid"] | cfg->wifiSsid, sizeof(cfg->wifiSsid));
  strlcpy(cfg->wifiPassword, doc["wifiPassword"] | cfg->wifiPassword, sizeof(cfg->wifiPassword));

  cfg->setTemp = doc["setTemp"] | cfg->setTemp;
  cfg->heatEnabled = doc["heatEnabled"] | cfg->heatEnabled;
  cfg->hysteresis = doc["hysteresis"] | cfg->hysteresis;

  cfg->dirty = false;
  return true;
}
