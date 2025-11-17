#include "ota_config.h"

// IP ثابت برای برد
IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

OTAConfig ota(
  "ARK2.4G",    // نام شبکه WiFi
  "Cyan14743...",
  "1234",           // رمز OTA
  "ESP32-OTA",      // نام میزبان
  local_IP, gateway, subnet
);

void setup() {
  ota.begin();
}

void loop() {
  ota.handle();
}
