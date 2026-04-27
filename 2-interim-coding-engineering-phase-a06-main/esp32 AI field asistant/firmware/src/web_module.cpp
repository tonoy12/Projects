#include "web_module.h"
#include "dashboard_html.h"
#include "sensor_module.h"
#include "feedback_module.h"
#include <ArduinoJson.h>

AsyncWebServer server(80);

// --- NEW MEMORY VARIABLES ---
int total_queries = 0;
String last_q = "Awaiting input...";
String last_r = "System ready.";

// --- NEW FUNCTION TO CATCH DATA FROM MAIN.CPP ---
void update_dashboard_ai_log(String question, String response) {
    total_queries++;
    last_q = question;
    last_r = response;
}

void setup_web_server() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", DASHBOARD_HTML);
    });

    server.on("/api/data", HTTP_GET, [](AsyncWebServerRequest *request){
        SensorReadings data = get_sensor_readings();
        
        // --- CRITICAL: INCREASED JSON SIZE TO 1024 FOR AI TEXT ---
        StaticJsonDocument<1024> doc; 
        doc["tA"] = data.tempAmbient;
        doc["tO"] = data.tempObject;
        doc["ldr"] = data.lightLevel;
        doc["gz"] = data.gyroZ;
        
        // --- ADD THE AI LOG TO THE PAYLOAD ---
        doc["qCount"] = total_queries;
        doc["lastQ"] = last_q;
        doc["lastR"] = last_r;

        doc["wifi"] = WiFi.RSSI(); 
        doc["ram"] = ESP.getFreeHeap();
        
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    server.on("/api/uv", HTTP_GET, [](AsyncWebServerRequest *request){
        static bool uv_state = false;
        uv_state = !uv_state;
        set_uv_led(uv_state);
        request->send(200, "text/plain", "OK");
    });

    server.on("/api/motor", HTTP_GET, [](AsyncWebServerRequest *request){
        pulse_vibration_motor(150);
        request->send(200, "text/plain", "OK");
    });

    server.on("/api/rgb", HTTP_GET, [](AsyncWebServerRequest *request){
        if (request->hasParam("c")) {
            String color = request->getParam("c")->value();
            if (color == "r") set_rgb(true, false, false);
            else if (color == "g") set_rgb(false, true, false);
            else if (color == "b") set_rgb(false, false, true);
            else if (color == "o") set_rgb(false, false, false);
        }
        request->send(200, "text/plain", "OK");
    });

    server.begin();
    Serial.println("Web Server Started!");
}