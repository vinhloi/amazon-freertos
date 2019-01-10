#ifndef RENESAS_SENSORS_H
#define RENESAS_SENSORS_H

#include "platform.h"

uint16_t read_isl29035_light_sensor(void);
uint32_t read_bmi160_inertial_sensor(void);
uint32_t read_bme680_environment_sensor(void);

#endif
