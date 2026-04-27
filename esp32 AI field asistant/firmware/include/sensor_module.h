#ifndef SENSOR_MODULE_H
#define SENSOR_MODULE_H

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_MLX90614.h>
#include <Wire.h>

#define SHARED_I2C_SDA 6
#define SHARED_I2C_SCL 7

struct SensorReadings {
    float tempObject;
    float tempAmbient;
    float gyroX;
    float gyroY;
    float gyroZ;
    int lightLevel; // <-- THIS IS THE NEW LINE
};

void setup_sensors();
SensorReadings get_sensor_readings();

#endif