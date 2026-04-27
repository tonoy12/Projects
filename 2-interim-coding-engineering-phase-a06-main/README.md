# Portable AI Field Assistant & Inspection Tool 🛠️

An edge-connected, handheld diagnostic device powered by an ESP32-S3. This tool provides field technicians with hands-free access to cloud AI (Gemini 2.5 Flash), non-destructive testing hardware (UV & Thermal), and a local asynchronous web dashboard. 



![Project Cover Image](docs/images/your_device_photo.jpg)

## 🌟 Key Features
* **Voice-Activated AI Querying:** Integrated INMP441 MEMS microphone and I2S amplifier pipeline communicating with Google's Gemini LLM via Deepgram Speech-to-Text.
* **Thermal Inspection:** Contactless object temperature monitoring via an MLX90614 I2C sensor.
* **Local IoT Telemetry:** Asynchronous web server (`ESPAsyncWebServer`) broadcasting 6-axis spatial data (MPU6050) and thermal readings to an interactive local dashboard via mDNS (`http://field-ai.local`).

## 📂 Repository Structure
* `/enclosure` - 3D printable STL files for the custom housing.
* `/firmware` - The complete C++ PlatformIO project (Source code and headers).
* `/hardware` - PCB schematics, Gerber manufacturing files, and a custom ESP32-S3 footprint.
* `/docs` - High-resolution images and the final engineering technical report.

## ⚙️ Hardware Bill of Materials (BOM)
* Freenove ESP32-S3 WROOM (16MB Flash, 8MB PSRAM)
* Waveshare 1.69" TFT Touch Display (SPI/I2C)
* INMP441 Digital Microphone & MAX98357A I2S Amplifier
* MLX90614 Infrared Temperature Sensor
* MPU6050 6-Axis Gyroscope/Accelerometer
* Adafruit bq25185 Charge Controller & 3.7V Li-Po Battery

## 🚀 Getting Started (Firmware Installation)
1. Clone this repository to your local machine.
2. Open the `/firmware` folder using **Visual Studio Code** with the **PlatformIO** extension installed.
3. Open `src/main.cpp` and update the Wi-Fi credentials to match your local network/hotspot:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";         
   const char* password = "YOUR_WIFI_PASSWORD";
