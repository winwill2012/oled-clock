#include <Arduino.h>
#include "button_detector.h"
#include "OneButton.h"
#include "buzzer.h"
#include "state_manager.h"

OneButton leftButton(BUTTON_LEFT_PIN, true, true);
OneButton centerButton(BUTTON_CENTER_PIN, true, true);
OneButton rightButton(BUTTON_RIGHT_PIN, true, true);

void left_button_click() {
    Serial.println("左边按钮单击");
    buzzer_beep();
}

void center_button_click() {
    Serial.println("中间按钮单击");
    if (get_global_state() == DisplayMainMenu) {
        xEventGroupClearBits(eventGroup, EVENT_DISPLAY_MAIN_MENU);
        xEventGroupSetBits(eventGroup, EVENT_DISPLAY_TIME);
        set_global_state(DisplayTime);
    }
}

void right_button_click() {
    Serial.println("右边按钮单击");
}

void left_button_long_press() {
    Serial.println("左边按钮长按");
}

void center_button_long_press() {
    Serial.println("中间按钮长按");
    if (get_global_state() == DisplayTime) {
        xEventGroupClearBits(eventGroup, EVENT_DISPLAY_TIME);
        xEventGroupSetBits(eventGroup, EVENT_DISPLAY_MAIN_MENU);
        set_global_state(DisplayMainMenu);
    }
}

void right_button_long_press() {
    Serial.println("右边按钮长按");
}

void left_button_double_click() {
    Serial.println("左边按钮双击");
}

void center_button_double_click() {
    Serial.println("中间按钮双击");
}

void right_button_double_click() {
    Serial.println("右边按钮双击");
}

void setup_button_detector() {
    leftButton.setLongPressIntervalMs(2000);
    centerButton.setLongPressIntervalMs(2000);
    rightButton.setLongPressIntervalMs(2000);

    leftButton.attachClick(left_button_click);
    centerButton.attachClick(center_button_click);
    rightButton.attachClick(right_button_click);

    leftButton.attachLongPressStart(left_button_long_press);
    centerButton.attachLongPressStart(center_button_long_press);
    rightButton.attachLongPressStart(right_button_long_press);

    leftButton.attachDoubleClick(left_button_double_click);
    centerButton.attachDoubleClick(center_button_double_click);
    rightButton.attachDoubleClick(right_button_double_click);

    xTaskCreate([](void *ptr) {
        while (true) {
            leftButton.tick();
            centerButton.tick();
            rightButton.tick();
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }, "one-button", 4096, nullptr, 1, nullptr);
}
