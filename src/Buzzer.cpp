#include <Arduino.h>
#include "Buzzer.h"

#include "StateManager.h"

#define PWM_CHANNEL 0
// 常用音调频率表（单位：Hz）
constexpr int DO = 523;
constexpr int RE = 587;
constexpr int MI = 659;
constexpr int FA = 698;
constexpr int SO = 784;
constexpr int LA = 880;
constexpr int SI = 988;

constexpr int DO_HIGH = 1046; // 高八度Do
constexpr int RE_HIGH = 1175; // 高八度Re
constexpr int MI_HIGH = 1319; // 高八度Mi
constexpr int LOW_SO = 392; // 低八度So（低音）
constexpr int LOW_MI = 330; // 低八度Mi（低音）

void beepInternal(const int freq, const int duration) {
    ledcWriteTone(PWM_CHANNEL, freq);
    ledcWrite(PWM_CHANNEL, 127);
    delay(duration);
    ledcWrite(PWM_CHANNEL, 0);
    delay(50);
}

void Buzzer::begin() {
    pinMode(BUZZER_PIN, OUTPUT);
    ledcSetup(PWM_CHANNEL, 440, 8);
    ledcAttachPin(BUZZER_PIN, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, 0);
    isRinging = false;
}

void Buzzer::beep() {
    if (!isRinging) {
        isRinging = true;
        xTaskCreate([](void *ptr) {
            for (int i = 0; i < 20 && StateManager::getState() == DisplayCountdown; i++) {
                beepInternal(DO_HIGH, 80);
                beepInternal(SO, 80);
                beepInternal(DO_HIGH, 80);
                delay(150);
            }
            vTaskDelete(nullptr);
        }, "buzzer-beep", 1024, this, 1, nullptr);
    }
}

void Buzzer::reset() {
    isRinging = false;
}
