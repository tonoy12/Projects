// audio_transcribe.h
#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <LittleFS.h>

// --- IMPORTANT: INSERT YOUR DEEPGRAM API KEY HERE ---
const char* deepgramApiKey = "fd33a432fcb855c2461029e2a026b02be99557c9"; 

String json_object(String input, String element) {
  String content = "";
  int pos_start = input.indexOf(element);      
  if (pos_start > 0) {  
     pos_start += element.length();             
     int pos_end = input.indexOf(",\"", pos_start);  
     if (pos_end > pos_start) { 
        content = input.substring(pos_start,pos_end);  
     } 
     content.trim();                                 
     if (content.startsWith("\"")) { 
        content = content.substring(1, content.length()-1);  
     }     
  }  
  return (content);
}

String SpeechToText_Deepgram(String audio_filename) { 
  delay(1000); // Let the ESP32 hardware catch its breath
  
  if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wi-Fi dropped! Reconnecting...");
      WiFi.disconnect();
      WiFi.reconnect();
      delay(3000);
  }

  Serial.println("\n--- Starting Deepgram Connection ---");
  Serial.printf("Internal RAM Free: %d bytes\n", ESP.getFreeHeap());

  File audioFile = LittleFS.open(audio_filename, "r");  
  if (!audioFile) {
    Serial.println("ERROR - Failed to open file");
    return "";
  }
  size_t audio_size = audioFile.size();

  // STACK ALLOCATION: The compiler will automatically destroy this when done
  WiFiClientSecure client;
  client.setInsecure();   

  HTTPClient http;
  http.setTimeout(20000); 

  String url = "https://api.deepgram.com/v1/listen?model=nova-2-general&smart_format=true&numerals=true";
  
  Serial.println("Negotiating SSL...");
  
  // Notice we don't need the asterisk (*) here anymore
  if (http.begin(client, url)) {
    http.addHeader("Authorization", String("Token ") + deepgramApiKey);
    http.addHeader("Content-Type", "audio/wav");

    Serial.println("Uploading Audio Data...");
    int httpResponseCode = http.sendRequest("POST", &audioFile, audio_size);

    String transcription = "";

    if (httpResponseCode > 0) {
      Serial.printf("HTTP Response code: %d\n", httpResponseCode);
      String response = http.getString();
      transcription = json_object(response, "\"transcript\":");
    } else {
      Serial.printf("Upload Failed! Error code: %d\n", httpResponseCode);
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }

    http.end();
    client.stop(); // Forcefully sever the TCP line
    audioFile.close();
    return transcription;
  } else {
    Serial.println("Unable to connect to Deepgram server.");
    client.stop();
    audioFile.close();
    return "";
  }
}