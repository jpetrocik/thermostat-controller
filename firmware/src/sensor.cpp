#include "configuration.h"

#ifdef DHT_ENABLED
#include <Arduino.h>
#include "DHT20.h"

static DHT20 DHT;
static float *humidityOut = 0;
static float *temperatureOut = 0;

void sensor_setup(float *humidity, float *temperature)
{
    humidityOut = humidity;
    temperatureOut = temperature;

    DHT.begin(); // ESP32 default pins 21 22
}

int sensor_loop()
{
    if (millis() - DHT.lastRead() >= 1000)
    {
        int status = DHT.read();

        switch (status)
        {
        case DHT20_OK:
            *humidityOut = DHT.getHumidity();
            *temperatureOut = DHT.getTemperature();
            // Serial.printf("Temperature: %.1f C, Humidity: %.1f %%\n", *temperatureOut, *humidityOut);
            return 1;
        }

        return status;
    }

    return 0;
}
#endif
