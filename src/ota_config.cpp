#include "ota_config.h"

OTAConfig::OTAConfig(const char* ssid,
                     const char* password,
                     const char* otaPassword,
                     const char* hostName,
                     IPAddress local_IP,
                     IPAddress gateway,
                     IPAddress subnet,
                     IPAddress dns1,
                     IPAddress dns2)
    : _ssid(ssid), _password(password), _otaPassword(otaPassword),
      _hostName(hostName), _local_IP(local_IP), _gateway(gateway),
      _subnet(subnet), _dns1(dns1), _dns2(dns2), _server(80) {}

void OTAConfig::begin() {
    Serial.begin(115200);
    setupWiFi();
    setupArduinoOTA();
    setupWebServer();
}

void OTAConfig::handle() {
    ArduinoOTA.handle();
    _server.handleClient();
}

// ================= WiFi ==================
void OTAConfig::setupWiFi() {
    WiFi.mode(WIFI_STA);
    if (!WiFi.config(_local_IP, _gateway, _subnet, _dns1, _dns2)) {
        Serial.println("⚠️ Failed to configure static IP");
    }
    WiFi.begin(_ssid, _password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n✅ WiFi connected!");
    Serial.print("📶 IP address: ");
    Serial.println(WiFi.localIP());
}

// ================= Arduino OTA ==================
void OTAConfig::setupArduinoOTA() {
    ArduinoOTA.setHostname(_hostName);
    ArduinoOTA.setPassword(_otaPassword);

    ArduinoOTA.onStart([]() { Serial.println("Start OTA update..."); });
    ArduinoOTA.onEnd([]() { Serial.println("OTA update complete!"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    ArduinoOTA.begin();
    Serial.println("✅ Arduino OTA Ready!");
}

// ================= Web Server ==================
const char* updatePage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 OTA Update</title>
  <style>
    body { background: #0e1117; color: #eee; font-family: Arial; text-align: center; padding: 30px; }
    h2 { color: #58a6ff; }
    form { margin-top: 20px; border: 2px dashed #58a6ff; padding: 20px; border-radius: 10px; display: inline-block; }
    input[type=file], input[type=submit], button {
      padding: 10px; border-radius: 5px; border: none; margin: 5px;
    }
    input[type=submit], button { background: #58a6ff; color: white; cursor: pointer; }
    input[type=submit]:hover, button:hover { background: #1f6feb; }
  </style>
</head>
<body>
  <h2>ESP32 OTA Web Update</h2>
  <form method='POST' action='/update' enctype='multipart/form-data'>
    <input type='file' name='update' required><br><br>
    <input type='submit' value='Upload Firmware'>
  </form>
  <br>
  <button onclick="fetch('/reboot').then(()=>alert('Rebooting...'));">Reboot</button>
</body>
</html>
)rawliteral";

void OTAConfig::setupWebServer() {
    _server.on("/", HTTP_GET, [this]() { handleRoot(); });
    _server.on("/update", HTTP_POST, [this]() {
        if (!_server.authenticate("admin", _otaPassword)) {
            return _server.requestAuthentication();
        }
        _server.sendHeader("Connection", "close");
        _server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart();
    }, [this]() { handleUpdate(); });
    _server.on("/reboot", HTTP_GET, [this]() { handleReboot(); });

    _server.begin();
    Serial.println("🌐 HTTP Update Server Ready!");
}

void OTAConfig::handleRoot() {
    if (!_server.authenticate("admin", _otaPassword)) {
        return _server.requestAuthentication();
    }
    _server.send(200, "text/html", updatePage);
}

void OTAConfig::handleUpdate() {
    HTTPUpload& upload = _server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("Update Start: %s\n", upload.filename.c_str());
        if (!Update.begin()) Update.printError(Serial);
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
            Update.printError(Serial);
    } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true))
            Serial.printf("✅ Update Success: %u bytes\nRebooting...\n", upload.totalSize);
        else
            Update.printError(Serial);
    }
    yield();
}

void OTAConfig::handleReboot() {
    if (!_server.authenticate("admin", _otaPassword)) {
        return _server.requestAuthentication();
    }
    _server.send(200, "text/plain", "Rebooting...");
    delay(1000);
    ESP.restart();
}
