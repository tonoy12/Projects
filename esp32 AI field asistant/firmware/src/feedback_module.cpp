#include "feedback_module.h"

#define UV_LED_PIN 41
#define VIB_MOTOR_PIN 42

#define RGB_R 48
#define RGB_G 47
#define RGB_B 21

void setup_feedback() {
    Serial.println("Initializing Feedback Hardware...");
    pinMode(UV_LED_PIN, OUTPUT);
    pinMode(VIB_MOTOR_PIN, OUTPUT);

    pinMode(RGB_R, OUTPUT);
    pinMode(RGB_G, OUTPUT);
    pinMode(RGB_B, OUTPUT);

    // Ensure they start turned OFF
    digitalWrite(UV_LED_PIN, LOW);
    digitalWrite(VIB_MOTOR_PIN, LOW);
    set_rgb(false, false, false);
}

void set_uv_led(bool state) {
    digitalWrite(UV_LED_PIN, state ? HIGH : LOW);
}

void pulse_vibration_motor(int duration_ms) {
    digitalWrite(VIB_MOTOR_PIN, HIGH);
    delay(duration_ms); 
    digitalWrite(VIB_MOTOR_PIN, LOW);
}

void set_rgb(bool r, bool g, bool b) {
    digitalWrite(RGB_R, r ? HIGH : LOW);
    digitalWrite(RGB_G, g ? HIGH : LOW);
    digitalWrite(RGB_B, b ? HIGH : LOW);
}