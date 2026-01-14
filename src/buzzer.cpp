#include <Arduino.h>
#include "buzzer.h"

#define PWM_CHANNEL 0
// 常用音调频率表（单位：Hz）
const int DO = 523;
const int RE = 587;
const int MI = 659;
const int FA = 698;
const int SO = 784;
const int LA = 880;
const int SI = 988;

void setup_buzzer() {
    pinMode(BUZZER_PIN, OUTPUT);
    ledcSetup(PWM_CHANNEL, 440, 8);
    ledcAttachPin(BUZZER_PIN, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, 0);
}

void beep(int freq, int duration) {
    // 修改PWM频率（改变音调）
    ledcWriteTone(PWM_CHANNEL, freq);
    // 设置占空比为50%（255/2=127，保证方波对称）
    ledcWrite(PWM_CHANNEL, 127);
    // 持续发声
    delay(duration);
    // 占空比设为0（停止发声）
    ledcWrite(PWM_CHANNEL, 0);
    // 间隔（可选）
    delay(50);
}

void buzzer_beep() {
    xTaskCreate([](void *ptr) {
        beep(DO, 200);
        beep(RE, 200);
        beep(MI, 200);
        beep(FA, 200);
        beep(SO, 200);
        beep(LA, 200);
        beep(SI, 200);
        vTaskDelete(nullptr);
    }, "buzzer-beep", 2048, nullptr, 1, nullptr);
}
