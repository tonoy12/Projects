#ifndef GEMINI_MODULE_H
#define GEMINI_MODULE_H

#include <Arduino.h>

// Function to send a query to Gemini and get a clean text response
// We changed this name to match your main.cpp!
String ask_gemini(String prompt_text);

#endif