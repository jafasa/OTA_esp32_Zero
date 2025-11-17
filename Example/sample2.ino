#include "ota_config.h"

// ---------- تنظیمات شبکه ----------
const char* WIFI_SSID     = "YOUR_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* OTA_PASSWORD  = "YOUR_OTA_PASSWORD";
const char* HOST_NAME     = "ESP32-OTA-ZERO-PRO";

// IP Static
IPAddress local_IP(192, 168, 1, 120);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns1(1, 1, 1, 1);       
IPAddress dns2(8, 8, 8, 8);       

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

// ---------- امکانات کاربردی ------
unsigned long lastRebootAsk = 0;
bool rebootRequested = false;

void printNetworkInfo() {
    Serial.println("--------- Network Info ---------");
    Serial.print("Connected SSID: "); Serial.println(WIFI_SSID);
    Serial.print("Device Host:   "); Serial.println(HOST_NAME);
    Serial.print("Device IP:     "); Serial.println(WiFi.localIP());
    Serial.println("--------------------------------");
}

void setup() {
    ota.begin();
    printNetworkInfo();
    Serial.println("OTA/WEB Server/Static IP فعال شدند! ارسال 'reboot' از طریق Serial برای ریبوت دستگاه.");
}

void loop() {
    ota.handle();

    // نمونه: کنترل دمای محیط (فرضی)
    float temp = 25.5; // فرضا از سنسور خوانده شده
    Serial.print("Sample2: Current Temperature: "); Serial.println(temp);

    // دریافت فرمان ریبوت از پورت سریال
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        if (cmd == "reboot") {
            rebootRequested = true;
            lastRebootAsk = millis();
            Serial.println("! ریبوت دستگاه تا 3 ثانیه دیگر ...");
        }
    }
    if (rebootRequested && millis() - lastRebootAsk > 3000) {
        ESP.restart();
    }

    delay(5000); // نمونه تست هر 5 ثانیه
}
