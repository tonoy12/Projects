#pragma once
#include <Arduino.h>
#include <driver/i2s.h>
#include <LittleFS.h>

// --- MIC PINS ---
#define I2S_MIC_WS  5
#define I2S_MIC_SCK 4
#define I2S_MIC_SD  1 // DFRobot Mic DO Pin

#define SAMPLE_RATE 16000 
#define BITS_PER_SAMPLE 16 

// WAV Header
struct WAV_HEADER {
  char  riff[4] = {'R','I','F','F'};
  long  flength = 0;
  char  wave[4] = {'W','A','V','E'};
  char  fmt[4]  = {'f','m','t',' '};
  long  chunk_size = 16;
  short format_tag = 1;
  short num_chans = 1;
  long  srate = SAMPLE_RATE;
  long  bytes_per_sec = SAMPLE_RATE * (BITS_PER_SAMPLE/8); 
  short bytes_per_samp = (BITS_PER_SAMPLE/8);
  short bits_per_samp = BITS_PER_SAMPLE;
  char  dat[4] = {'d','a','t','a'};
  long  dlength = 0;
} myWAV_Header;

bool flg_is_recording = false;
bool flg_I2S_initialized = false;
File audio_file; // Keep the file handle open globally

bool I2S_Record_Init() {  
  i2s_config_t mic_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT, 
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, 
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 512,
    .use_apll = true
  };
  
  i2s_pin_config_t mic_pins = {
    .bck_io_num = I2S_MIC_SCK,
    .ws_io_num = I2S_MIC_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SD
  };

  if (i2s_driver_install(I2S_NUM_1, &mic_config, 0, NULL) == ESP_OK) {
    i2s_set_pin(I2S_NUM_1, &mic_pins);
    flg_I2S_initialized = true; 
  }
  return flg_I2S_initialized;  
}

bool Record_Start(String audio_filename) {
  if (!flg_I2S_initialized) return false;
  
  // TRIGGER ONCE: When button is first pressed
  if (!flg_is_recording) { 
    flg_is_recording = true;
    if (LittleFS.exists(audio_filename)) { LittleFS.remove(audio_filename); }
    
    audio_file = LittleFS.open(audio_filename, FILE_WRITE);
    if (audio_file) {
        audio_file.write((uint8_t *) &myWAV_Header, 44);
    }
    Serial.println("Recording Started... (File Open)");
    return true;
  }
  
  // CONTINUOUS STREAM: While button is held
  if (flg_is_recording && audio_file) { 
    int32_t raw_samples[512];  
    int16_t audio_buffer[512]; 
    size_t bytes_read = 0;
    
    // 100ms timeout prevents the display UI from freezing
    i2s_read(I2S_NUM_1, raw_samples, sizeof(raw_samples), &bytes_read, 100 / portTICK_PERIOD_MS);

    int samples_read = bytes_read / 4; 
    for (int i = 0; i < samples_read; i++) {
        audio_buffer[i] = (raw_samples[i] >> 12); 
    }

    // Write chunk without closing the file AND commit to memory!
    if (samples_read > 0) {
        audio_file.write((uint8_t*)audio_buffer, samples_read * 2); 
        audio_file.flush(); // <--- THIS SAVES THE FILE SIZE
    }
    return true; 
  }  
  return false;
}

bool Record_Available(String audio_filename, float* audiolength_sec) {
  if (!flg_is_recording || !flg_I2S_initialized) return false;
  
  // TRIGGER ONCE: When button is released
  if (audio_file) {
      long filesize = audio_file.size();
      audio_file.seek(0); 
      myWAV_Header.flength = filesize;  
      myWAV_Header.dlength = (filesize - 44);
      audio_file.write((uint8_t *) &myWAV_Header, 44);
      audio_file.close(); 
      
      *audiolength_sec = (float) (filesize - 44) / (SAMPLE_RATE * BITS_PER_SAMPLE / 8); 
  }
  
  flg_is_recording = false; 
  Serial.println("Recording Stopped. Saved to Internal Flash.");
  return true;   
}