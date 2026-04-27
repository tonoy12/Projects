#include "ui_module.h"
#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include "CST816S.h"
#include "feedback_module.h" 
#include "sensor_module.h"

// --- HARD-CODED PIN DEFINITIONS ---
#define CUSTOM_TFT_BL 18
#define CUSTOM_TFT_DC 16
#define CUSTOM_TFT_CS 15
#define CUSTOM_TFT_SCK 11
#define CUSTOM_TFT_MOSI 8
#define CUSTOM_TFT_RST 17

// --- DISPLAY & TOUCH DRIVERS ---
Arduino_DataBus *bus = new Arduino_ESP32SPI(
    CUSTOM_TFT_DC, CUSTOM_TFT_CS, CUSTOM_TFT_SCK, CUSTOM_TFT_MOSI, GFX_NOT_DEFINED
);

Arduino_GFX *gfx = new Arduino_ST7789(
    bus, CUSTOM_TFT_RST, 0, true, 240, 280, 0, 20
);

CST816S touch(6, 7, 10, 9);

static lv_disp_draw_buf_t draw_buf;
lv_obj_t * main_label;

// --- SENSOR UI ELEMENTS ---
lv_obj_t * sensor_panel;
lv_obj_t * sensor_label;
bool sensor_panel_visible = false;

// --- STATE VARIABLES ---
static bool raw_btn_held = false; 

// --- CALLBACKS ---

// 1. TALK BUTTON CALLBACK
static void btn_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_PRESSED) {
        pulse_vibration_motor(120); // <-- ADDED HAPTIC BUZZ
        raw_btn_held = true; 
    }
    else if(code == LV_EVENT_RELEASED) {
        raw_btn_held = false; 
    }
}

// 2. SENSORS BUTTON CALLBACK
static void sensor_btn_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        pulse_vibration_motor(120); // <-- ADDED HAPTIC BUZZ
        sensor_panel_visible = !sensor_panel_visible;
        if(sensor_panel_visible) {
            lv_obj_clear_flag(sensor_panel, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(sensor_panel, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

// 3. NEW UV BUTTON CALLBACK
static void uv_btn_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        pulse_vibration_motor(120); // <-- ADDED HAPTIC BUZZ

        // Secretly read the LDR sensor
        SensorReadings liveData = get_sensor_readings();

        // Check if it is too dark (Threshold set to 500 out of 4095)
        if (liveData.lightLevel < 500) {
            // It's dark! Force the UV LED off.
            set_uv_led(false);
            Serial.println("Room is too dark. UV LED disabled.");
        } else {
            // It's bright enough. Toggle the LED state.
            static bool uv_state = false;
            uv_state = !uv_state;
            set_uv_led(uv_state);
            Serial.println(uv_state ? "UV LED ON" : "UV LED OFF");
        }
    }
}

// --- DRIVER BINDINGS ---
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
    lv_disp_flush_ready(disp);
}

void my_touch_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    if (touch.available()) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch.data.x;
        data->point.y = touch.data.y;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void ui_tick(void *arg) { lv_tick_inc(5); }

// --- SETUP UI ---
void setup_ui() {
    pinMode(CUSTOM_TFT_BL, OUTPUT);
    digitalWrite(CUSTOM_TFT_BL, HIGH); 
    
    touch.begin();
    gfx->begin();
    
    lv_init();
    uint32_t w = gfx->width();
    uint32_t h = gfx->height();
    
    lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(w * h / 10 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, w * h / 10);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = w; 
    disp_drv.ver_res = h;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touch_read;
    lv_indev_drv_register(&indev_drv);

    const esp_timer_create_args_t tick_args = { .callback = &ui_tick, .name = "lvgl_tick" };
    esp_timer_handle_t tick_timer = NULL;
    esp_timer_create(&tick_args, &tick_timer);
    esp_timer_start_periodic(tick_timer, 5000);

    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN);
    
    // 1. MAIN CHAT LABEL
    main_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_color(main_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(main_label, LV_TEXT_ALIGN_CENTER, 0); 
    lv_obj_set_width(main_label, 220); 
    lv_label_set_long_mode(main_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(main_label, LV_ALIGN_CENTER, 0, -20);
    lv_label_set_text(main_label, "System Ready\nHold TALK to ask");

    // 2. TALK BUTTON (Bottom)
    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 120, 50);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "TALK");
    lv_obj_center(btn_label);

    // 3. SENSOR TOGGLE BUTTON (Top Right)
    lv_obj_t * sensor_btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(sensor_btn, 100, 45);
    lv_obj_align(sensor_btn, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_add_event_cb(sensor_btn, sensor_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_t * sensor_btn_label = lv_label_create(sensor_btn);
    lv_label_set_text(sensor_btn_label, "Sensors");
    lv_obj_center(sensor_btn_label);

    // 4. HIDDEN SENSOR DROP-DOWN PANEL
    sensor_panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(sensor_panel, 180, 100); // <-- HEIGHT INCREASED TO 100 FOR NEW TEXT
   lv_obj_align(sensor_panel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(sensor_panel, lv_color_hex(0x222222), 0); 
    lv_obj_set_style_border_color(sensor_panel, lv_color_hex(0x555555), 0);
    lv_obj_add_flag(sensor_panel, LV_OBJ_FLAG_HIDDEN); 

    sensor_label = lv_label_create(sensor_panel);
    lv_obj_set_style_text_color(sensor_label, lv_color_hex(0x00FF00), 0); 
    lv_label_set_text(sensor_label, "Loading...");
    lv_obj_center(sensor_label);

    // 5. NEW UV TOGGLE BUTTON (Top Left)
    lv_obj_t * uv_btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(uv_btn, 80, 45);
    lv_obj_align(uv_btn, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_add_event_cb(uv_btn, uv_btn_event_cb, LV_EVENT_ALL, NULL);
    
    lv_obj_t * uv_btn_label = lv_label_create(uv_btn);
    lv_label_set_text(uv_btn_label, "UV");
    lv_obj_center(uv_btn_label);
}

void update_ui_loop() { lv_timer_handler(); }

// --- UPDATED TO SHOW LIGHT LEVEL ---
void ui_display_sensor_data(SensorReadings data) {
    char buffer[160];
    snprintf(buffer, sizeof(buffer), 
        "Temp: %.1f C\n"
        "Gyro Z: %.2f\n"
        "Light: %d", // <-- ADDED LIGHT LABEL
        data.tempObject, data.gyroZ, data.lightLevel // <-- ADDED LIGHT VARIABLE
    );
    if (sensor_label) lv_label_set_text(sensor_label, buffer);
}

void ui_update_chat(String text) {
    if (main_label) lv_label_set_text(main_label, text.c_str());
}

bool ui_is_sensor_panel_open() {
    return sensor_panel_visible;
}

bool ui_is_talk_held() {
    static unsigned long last_held_time = 0;
    static bool debounced_state = false;
    
    if (raw_btn_held) {
        last_held_time = millis();
        debounced_state = true;
    } else {
        if (millis() - last_held_time > 600) {
            debounced_state = false;
        }
    }
    return debounced_state;
}

