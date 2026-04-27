#ifndef UI_MODULE_H
#define UI_MODULE_H

#include <Arduino.h>
#include "sensor_module.h" 

// Initialize Display and Touch
void setup_ui();

// Handle UI updates (call in loop)
void update_ui_loop();

// Update the Chat area with text
void ui_update_chat(String text);

// Display Sensor Data
void ui_display_sensor_data(SensorReadings data);


// Check if Talk button is currently HELD down
bool ui_is_talk_held();  // <--- THIS WAS MISSING
// Check if the sensor panel is currently visible
bool ui_is_sensor_panel_open();

#endif