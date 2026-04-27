#ifndef FEEDBACK_MODULE_H
#define FEEDBACK_MODULE_H

#include <Arduino.h>

void setup_feedback();
void set_uv_led(bool state);
void pulse_vibration_motor(int duration_ms);

void set_rgb(bool r, bool g, bool b);

#endif