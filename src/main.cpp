#include <Arduino.h>
#include "ButtonDetector.h"
#include "NTPClient.h"
#include "OledDisplay.h"
#include "StateManager.h"
#include "WeatherQuery.h"
#include "WiFiManager.h"

OledDisplay display(OLED_IIC_DATA_PIN, OLED_IIC_SCL_PIN);
WeatherQuery weatherQuery("SHOEXKwNcHrAxuw09");
tm timeInfo{};
WiFiUDP udp;
NTPClient timeClient = NTPClient(udp, "time1.aliyun.com", 28800);

void setup() {
    Serial.begin(115200);
    ButtonDetector::begin();
    WiFiManager wm;
    display.begin();
    display.displayConnectWifiTips();
    wm.autoConnect("ESP32时钟");
    while (!WiFi.isConnected()) {
        Serial.print(".");
        vTaskDelay(1000);
    }
    Serial.println("");
    Serial.println("Connected to WiFi");
    StateManager::startWeatherQueryTask();
    display.loop();
}

void loop() {
}
