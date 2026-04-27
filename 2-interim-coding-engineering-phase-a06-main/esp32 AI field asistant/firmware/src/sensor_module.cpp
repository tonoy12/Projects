#include <Arduino.h>
#include <Wire.h>
#include "sensor_module.h"

// --- THE LDR PIN ---
#define LDR_PIN 2

Adafruit_MPU6050 mpu;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup_sensors() {
    // Start I2C on Pins 6 & 7 (Use the macros from your header)
    Wire.begin(SHARED_I2C_SDA, SHARED_I2C_SCL);
    
    // SLOW DOWN the bus. 100kHz is much more resistant to the noise
    // from your new SCLK pin (GPIO 11) than the default speed.
    Wire.setClock(100000); 

    Serial.println("Initializing Sensors...");
    
    // --- SET UP THE LDR HARDWARE ---
    pinMode(LDR_PIN, INPUT);
    
    // Add a small delay for the MLX to settle
    delay(100);

    if (!mpu.begin()) {
        Serial.println("MPU6050 Not Found! Check wiring.");
    } else {
        // FORCE the sensor out of sleep (often needed after a wiring change)
        mpu.setCycleRate(MPU6050_CYCLE_40_HZ); 
        
        mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
        mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
        Serial.println("MPU6050 Online.");
    }

    if (!mlx.begin()) {
        Serial.println("MLX90614 Not Found! Check wiring.");
    } else {
        Serial.println("MLX90614 Online.");
    }
}

SensorReadings get_sensor_readings() {
    // --- FIXED: Added the 6th zero for the lightLevel variable ---
    SensorReadings data = {0, 0, 0, 0, 0, 0};

    // Read MPU6050
    sensors_event_t a, g, temp;
    // We use a safe check here
    if (mpu.getEvent(&a, &g, &temp)) { 
        data.gyroX = g.gyro.x;
        data.gyroY = g.gyro.y;
        data.gyroZ = g.gyro.z;
    }

    // Read IR Temp
    data.tempObject = mlx.readObjectTempC();
    data.tempAmbient = mlx.readAmbientTempC();

    // --- READ THE LDR SENSOR ---
    data.lightLevel = analogRead(LDR_PIN);

    return data;
}