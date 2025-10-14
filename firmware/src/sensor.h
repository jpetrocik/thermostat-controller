#ifndef SENSOR_H
#define SENSOR_H

#include "configuration.h"

#ifdef DHT_ENABLED
void sensor_setup(float *humidity, float *temperature);
int sensor_loop();
#endif

#endif
