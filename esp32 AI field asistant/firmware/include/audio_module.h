#ifndef AUDIO_MODULE_H
#define AUDIO_MODULE_H

#include <Arduino.h>

#define I2S_MIC_SERIAL_CLOCK 41  // Example Pin - Check your schematic
#define I2S_MIC_LEFT_RIGHT_CLOCK 42
#define I2S_MIC_SERIAL_DATA 1    // Your DFRobot Mic Pin

void setup_audio();
bool record_audio(int duration_sec, uint8_t* buffer, size_t* size);

#endif