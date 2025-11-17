#ifndef OTA_CONFIG_H
#define OTA_CONFIG_H

#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <ArduinoOTA.h>

class OTAConfig {
public:
    OTAConfig(const char* ssid,
              const char* password,
              const char* otaPassword,
              const char* hostName,
              IPAddress local_IP,
              IPAddress gateway,
              IPAddress subnet,
              IPAddress dns1 = IPAddress(8,8,8,8),
              IPAddress dns2 = IPAddress(8,8,4,4));

    void begin();      // راه‌اندازی کامل WiFi، OTA و WebServer
    void handle();     // فراخوانی در حلقه‌ی loop()

private:
    // تنظیمات اتصال
    const char* _ssid;
    const char* _password;
    const char* _otaPassword;
    const char* _hostName;

    // شبکه
    IPAddress _local_IP;
    IPAddress _gateway;
    IPAddress _subnet;
    IPAddress _dns1;
    IPAddress _dns2;

    // وب سرور
    WebServer _server;

    void setupWiFi();
    void setupArduinoOTA();
    void setupWebServer();

    // هندلرها
    void handleRoot();
    void handleUpdate();
    void handleReboot();
};

#endif
