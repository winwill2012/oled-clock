#include "ButtonDetector.h"

#include "NTPClient.h"
#include "StateManager.h"

OneButton ButtonDetector::leftButton = OneButton(BUTTON_LEFT_PIN, true, true);
OneButton ButtonDetector::centerButton = OneButton(BUTTON_CENTER_PIN, true, true);
OneButton ButtonDetector::rightButton = OneButton(BUTTON_RIGHT_PIN, true, true);
extern NTPClient timeClient;

void left_button_click() {
    switch (StateManager::getState()) {
        case DisplayCalendar:
            StateManager::decreaseCalendarMonth();
            break;
        case DisplayMainMenu:
            StateManager::decreaseMenuIndex();
            break;
        case DisplayCountdownSet:
            StateManager::decreaseCountdownIndicator();
            break;
        default:
            break;
    }
}

void center_button_click() {
    switch (StateManager::getState()) {
        case DisplayMainMenu:
            StateManager::updateStateByMenuItemIndex();
            break;
        case DisplayCountdownSet:
            StateManager::increaseCountdownTime();
            break;
        default:
            break;
    }
}

void right_button_click() {
    switch (StateManager::getState()) {
        case DisplayCalendar:
            StateManager::increaseCalendarMonth();
            break;
        case DisplayMainMenu:
            StateManager::increaseMenuIndex();
            break;
        case DisplayCountdownSet:
            StateManager::increaseCountdownIndicator();
            break;
        default:
            break;
    }
}

void left_button_long_press() {
    Serial.println("左边按钮长按");
}

void center_button_long_press() {
    switch (StateManager::getState()) {
        case DisplayCountdownSet:
            StateManager::updateState(DisplayCountdown);
            break;
        default:
            StateManager::updateState(DisplayMainMenu);
    }
}

void right_button_long_press() {
    Serial.println("右边按钮长按");
}

void left_button_double_click() {
    Serial.println("左边按钮双击");
}

void center_button_double_click() {
    switch (StateManager::getState()) {
        case DisplayCalendar: {
            tm timeInfo{};
            timeClient.update(); // update函数内部做了更新间隔检测，并不会每一次都从网络获取时间
            unsigned long localEpochTime = timeClient.getEpochTime();
            gmtime_r(reinterpret_cast<time_t *>(&localEpochTime), &timeInfo);
            StateManager::setCalendarYear(timeInfo.tm_year + 1900);
            StateManager::setCalendarMonth(timeInfo.tm_mon + 1);
            break;
        }
        default:
            break;
    }
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

    xTaskCreate([](void *) {
        while (true) {
            leftButton.tick();
            centerButton.tick();
            rightButton.tick();
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }, "one-button", 4096, nullptr, 1, nullptr);
}
