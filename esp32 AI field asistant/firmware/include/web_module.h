#ifndef WEB_MODULE_H
#define WEB_MODULE_H

#include <ESPAsyncWebServer.h>

void setup_web_server();

// --- ADD THIS NEW LINE ---
void update_dashboard_ai_log(String question, String response);

#endif