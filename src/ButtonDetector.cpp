#include "ButtonDetector.h"
#include "StateManager.h"

OneButton ButtonDetector::leftButton = OneButton(BUTTON_LEFT_PIN, true, true);
OneButton ButtonDetector::centerButton = OneButton(BUTTON_CENTER_PIN, true, true);
OneButton ButtonDetector::rightButton = OneButton(BUTTON_RIGHT_PIN, true, true);

void left_button_click() {
    Serial.println("左边按钮单击");
    StateManager::updateState(DisplayClock);
}

void center_button_click() {
    Serial.println("中间按钮单击");
    StateManager::updateState(DisplayMainMenu);
}

void right_button_click() {
    Serial.println("右边按钮单击");
    StateManager::updateState(DisplayCountdown);
}

void left_button_long_press() {
    Serial.println("左边按钮长按");
}

void center_button_long_press() {
    Serial.println("中间按钮长按");
    StateManager::updateState(DisplayCalender);
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

void ButtonDetector::begin() {
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
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }, "one-button", 4096, nullptr, 1, nullptr);
}
