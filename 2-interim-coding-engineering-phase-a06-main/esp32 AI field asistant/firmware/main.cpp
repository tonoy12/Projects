/**
 * @project TPJ655 - Portable AI Field Assistant and Inspection Tool
 * @team Team A06
 * @author Tonoy Ahmed (172667222)
 * @author Touhidul Islam (112422233)
 * @date April 2026
 *
 * @hardware Freenove ESP32-S3 WROOM (16MB Flash, PSRAM enabled)
 *
 * @details
 * This is the main file for the project. It sets up the whole system:
 * 1. It turns on the audio (microphone and speaker) to talk to the AI.
 * 2. It starts the sensors (temperature and movement).
 * 3. It starts the Wi-Fi dashboard so you can see data on your phone.
 *
 * @ai_declaration
 * Google Gemini was used as a coding assistant during the development of this 
 * project. It helped structure the asynchronous web server, troubleshoot PSRAM 
 * memory management, and format documentation. All final code logic, hardware 
 * assembly, and testing were independently verified by the student team.
 *
 * @dependencies
 * - ESPAsyncWebServer (For the web dashboard)
 * - ArduinoJson (For sending data)
 * - ESPmDNS (For the local web address)
 */


#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "Audio.h" 
#include "web_module.h"

#include "ui_module.h"
#include "sensor_module.h"
#include "gemini_module.h" 
#include "feedback_module.h" 


#include "audio_record.h"
#include "audio_transcribe.h"

// --- CREDENTIALS (USE PHONE HOTSPOT!) ---
const char* ssid = "poco";         
const char* password = "senecaect"; 

// --- PIN CONFIGURATION ---
#define I2S_SPK_BCLK 13
#define I2S_SPK_LRCK 12
#define I2S_SPK_DOUT 14


Audio audio;


bool is_talking = false;
bool audio_was_playing = false;
unsigned long last_sensor_update = 0;
unsigned long last_gemini_response_time = 0; 

void setup() {
    Serial.begin(115200);
    
    
    if(psramInit()){
        Serial.println("PSRAM Initialized Successfully!");
        Serial.printf("Total PSRAM: %d bytes\n", ESP.getPsramSize());
    } else {
        Serial.println("PSRAM Failed to Initialize!");
    }

    setup_ui(); 
    setup_sensors(); 
    setup_feedback(); 
    
    audio.setPinout(I2S_SPK_BCLK, I2S_SPK_LRCK, I2S_SPK_DOUT);
    audio.setVolume(14);
    I2S_Record_Init(); 
    
    if (LittleFS.begin(true)) {
        Serial.println("LittleFS Mounted.");
    }
    
    
    WiFi.setSleep(false);

    WiFi.begin(ssid, password);
    ui_update_chat("Connecting WiFi...");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        update_ui_loop();
    }
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP()); 
    // --- THE NEW MDNS SETUP ---
    if (MDNS.begin("field-ai")) {
        Serial.println("mDNS started: http://field-ai.local");
    }
    setup_web_server(); //  START THE DASHBOAR
  
    ui_update_chat("System Ready.\nHold TALK to ask.");
}

void loop() {
    audio.loop(); 
    update_ui_loop(); 

    // --- STATE 4: IDLE (OFF) ---
    // Detects when the AI finishes speaking and turns the LED off
    if (audio.isRunning()) {
        audio_was_playing = true;
    } else if (audio_was_playing) {
        audio_was_playing = false;
        set_rgb(false, false, false); // Turn LED OFF
    }

    if (ui_is_talk_held()) {
        if (!is_talking) {
            is_talking = true;
            
            // --- STATE 1: LISTENING (BLUE) ---
            set_rgb(false, false, true); 
            
            ui_update_chat("Listening...");
            if (audio.isRunning()) { audio.stopSong(); }
            Record_Start("/audio.wav"); 
        }
        Record_Start("/audio.wav"); 
        update_ui_loop(); 
    } 
    else if (is_talking && !ui_is_talk_held()) {
        is_talking = false; 
        
        // --- STATE 2: PROCESSING (MAGENTA) ---
        set_rgb(true, false, true); 
        
        ui_update_chat("Processing...");

        float audio_length = 0;
        if (Record_Available("/audio.wav", &audio_length)) {
            Serial.printf("Captured: %.2f seconds\n", audio_length);
            if (audio_length > 0.3) { 
                String user_text = SpeechToText_Deepgram("/audio.wav");
                
                if (user_text != "") {
                    ui_update_chat("You: " + user_text); 
                    String response = ask_gemini(user_text);
                    ui_update_chat(response);

                    update_dashboard_ai_log(user_text, response);
                    
                    delay(500);
                    
                    // --- STATE 3: SPEAKING (GREEN) ---
                    set_rgb(false, true, false); 
                    
                    // The physical buzz before speaking
                    pulse_vibration_motor(150); 
                    
                    audio.connecttospeech(response.c_str(), "en"); 
                } else {
                    ui_update_chat("Communication error.");
                    set_rgb(true, false, false); // Red for error
                    delay(1000);
                    set_rgb(false, false, false);
                }
            } else {
                ui_update_chat("Tap too short.");
                set_rgb(false, false, false); 
            }
        }
        last_gemini_response_time = millis();
    }
    
 
    if (ui_is_sensor_panel_open() && !is_talking) {
        if (millis() - last_sensor_update > 500) {
            last_sensor_update = millis();
            SensorReadings liveData = get_sensor_readings();
            ui_display_sensor_data(liveData);
        }
    }
}