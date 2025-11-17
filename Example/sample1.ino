#include "ota_config.h"

// تنظیمات شبکه (مقدارهای تست، آن‌ها را به مقدار مناسب پروژه خود تغییر دهید)
const char* WIFI_SSID     = "YOUR_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* OTA_PASSWORD  = "YOUR_OTA_PASSWORD";
const char* HOST_NAME     = "ESP32-OTA-ZERO";

// IPهای شبکه (در صورت نیاز ثابت باشد)
IPAddress local_IP(192, 168, 1, 180);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns1(8,8,8,8);        // اختیاری
IPAddress dns2(8,8,4,4);        // اختیاری

OTAConfig ota(
    WIFI_SSID,
    WIFI_PASSWORD,
    OTA_PASSWORD,
    HOST_NAME,
    local_IP,
    gateway,
    subnet,
    dns1,
    dns2
);

void setup() {
    ota.begin();
    Serial.println("OTA & Web Update فعال شد!");
}

void loop() {
    ota.handle();
    // سایر توابع برنامه‌تان را اینجا بنویسید، مثلا:
    // sensorLoop();
