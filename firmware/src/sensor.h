#ifndef SENSOR_H
#define SENSOR_H

#include "DHT20.h"

#ifdef DHT_ENABLED
void sensorSetup();
void sensorLoop();
#endif

#endif
