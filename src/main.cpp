#include <Arduino.h>
#include "OledDisplay.h"
#include "WiFiManager.h"

OledDisplay display(OLED_IIC_DATA_PIN, OLED_IIC_SCL_PIN);

void setup() {
    Serial.begin(115200);
    WiFiManager wm;
    wm.autoConnect("ESP32时钟");
    while (!WiFi.isConnected()) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    Serial.println("Connected to WiFi");
    // StateManager::updateState(DisplayClock);
    display.begin();
}

void loop() {
}
