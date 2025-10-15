#include "configuration.h"

#ifdef WEBSERVER_ENABLED
#include <Arduino.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "switch.h"

WebServer server(80);
char webserver_jsonStatusBuffer[300];
DeviceConfig *webserver_deviceConfig;

void webServerLoop()
{
  server.handleClient();
}

void handleStatus()
{
  snprintf(webserver_jsonStatusBuffer, sizeof(webserver_jsonStatusBuffer), "{\"setTemp\":%i, \"heat\":\"on\", \"temperature\":%.1f, \"humidity\":%.1f}", (int)webserver_deviceConfig->setTemp, 0.0f, 0.0f);
  server.send(200, "application/json", webserver_jsonStatusBuffer);
}

void handleRestart()
{
  server.send(200, "application/json", "{\"message\":\"Restarting\"}");
  delay(1000);
  ESP.restart();
}

// TODO Should return deviceConfig
void handleConfigureDevice()
{
  Serial.println("Loading config data....");
  if (SPIFFS.exists("/config.json"))
  {
    // file exists, reading and loading
    File configFile = SPIFFS.open("/config.json", "r");
    if (configFile)
    {
      size_t size = configFile.size();
      std::unique_ptr<char[]> buf(new char[size + 1]);

      configFile.readBytes(buf.get(), size);
      buf.get()[size] = 0;

      server.send(200, "application/json", buf.get());
      return;
    }
  }
  server.send(200, "application/json", "{}");
}

void handleSaveConfigureDevice()
{
  int argCount = server.args();
  for (int i = 0; i < argCount; i++)
  {
    String argName = server.argName(i);
    String argValue = server.arg(i);

    if (argName == "device")
    {
        argValue.toCharArray(webserver_deviceConfig->deviceName, 20);
    }
    else if (argName == "room")
    {
        argValue.toCharArray(webserver_deviceConfig->roomName, 20);
    }
    else if (argName == "location")
    {
        argValue.toCharArray(webserver_deviceConfig->locationName, 20);
    }
    else if (argName == "mqttHost")
    {
        argValue.toCharArray(webserver_deviceConfig->mqttHost, 50);
    }
    else if (argName == "ssid")
    {
        argValue.toCharArray(webserver_deviceConfig->wifiSsid, 50);
    }
    else if (argName == "password")
    {
        argValue.toCharArray(webserver_deviceConfig->wifiPassword, 50);
    }
    else if (argName == "temp")
    {
        webserver_deviceConfig->setTemp = argValue.toInt();
    }
  }

  webserver_deviceConfig->dirty = true;

  server.send(200);
}

void webServerSetup(DeviceConfig *deviceConfig)
{
  webserver_deviceConfig = deviceConfig;

  Serial.println("Starting web server on port 80");
  if (!SPIFFS.begin())
  {
    Serial.println("Warning: SPIFFS mount failed");
  }
  server.on("/", handleStatus);
  server.on("/restart", HTTP_POST, handleRestart);
  server.on("/config", HTTP_GET, handleConfigureDevice);
  server.on("/config", HTTP_PUT, handleSaveConfigureDevice);

  server.begin();
}
#endif