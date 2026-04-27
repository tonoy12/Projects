#include "gemini_module.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// --- IMPORTANT: INSERT YOUR GOOGLE GEMINI API KEY HERE ---
const char* gemini_KEY = "AIzaSyCodpVSdOZ_3IbuGA3C2RdZCoapYK2GEc0"; 

String ask_gemini(String prompt_text) {
  if (prompt_text == "") return "I didn't hear anything.";
  
  Serial.println("\n--- Asking Gemini AI ---");
  Serial.println("Prompt: " + prompt_text);

  // STACK ALLOCATION: The compiler will instantly destroy this when done!
  WiFiClientSecure client;
  client.setInsecure(); 

  HTTPClient http;
  http.setTimeout(15000); 

  String url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key=" + String(gemini_KEY);
  
  String hidden_rule = "Answer conversationally in one short sentence: ";
  String payload = "{\"contents\": [{\"parts\":[{\"text\":\"" + hidden_rule + prompt_text + "\"}]}],\"tools\": [{\"googleSearch\": {}}],\"generationConfig\": {\"maxOutputTokens\": 500}}";

  Serial.println("Connecting to Gemini...");
  
  // Notice we dropped the asterisk (*) here
  if (http.begin(client, url)) {
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.sendRequest("POST", payload);
    String response = "";

    if (httpResponseCode > 0) {
      response = http.getString();
      Serial.println("\n--- RAW GEMINI RESPONSE ---");
      Serial.println(response);
      Serial.println("---------------------------\n");
    } else {
      Serial.printf("Error connecting to Gemini: %d\n", httpResponseCode);
      http.end();
      client.stop();
      return "Connection failed.";
    }

    // --- THE FIX: INSTANTLY HANG UP THE PHONE ---
    http.end();
    client.stop(); 

    DynamicJsonDocument doc(16384);
    DeserializationError error = deserializeJson(doc, response);
    
    if (error) {
        Serial.print("JSON Parse failed: ");
        Serial.println(error.c_str());
        return "Brain Error.";
    }

    if (!doc["candidates"][0].isNull()) {
        String answer = doc["candidates"][0]["content"]["parts"][0]["text"].as<String>();
        answer.replace("*", ""); 
        answer.replace("#", "");
        answer.replace("\n", " ");
        return answer;
    } else {
        if (!doc["error"]["message"].isNull()) {
            String api_error = doc["error"]["message"].as<String>();
            Serial.println("GEMINI API ERROR: " + api_error);
            return "API Error.";
        }
        return "I am confused.";
    }
  } else {
    Serial.println("Failed to start HTTP connection.");
    client.stop();
    return "HTTP Error.";
  }
}