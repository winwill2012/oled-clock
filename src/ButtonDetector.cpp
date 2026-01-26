#include "ButtonDetector.h"

#include "NTPClient.h"
#include "StateManager.h"

OneButton ButtonDetector::leftButton = OneButton(BUTTON_LEFT_PIN, true, true);
OneButton ButtonDetector::centerButton = OneButton(BUTTON_CENTER_PIN, true, true);
OneButton ButtonDetector::rightButton = OneButton(BUTTON_RIGHT_PIN, true, true);
extern NTPClient timeClient;
extern uint8_t menuSize;

void left_button_click() {
    switch (StateManager::getState()) {
        case DisplayCalendar:
            StateManager::decreaseCalendarMonth();
            break;
        case DisplayMainMenu:
            StateManager::decreaseMenuIndex();
            break;
        case DisplayCountdownSet: {
            StateManager::getCountdownInfo().countdownIndicator =
                    (StateManager::getCountdownInfo().countdownIndicator + 3) % 4;
            break;
        }
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
        case DisplayStopWatch:
            // 还没开始则开始计时
            if (StateManager::getStopWatchMillis() == 0) {
                StateManager::startStopWatch();
                // 秒表模式，需要提高屏幕刷新频率
                StateManager::updateOledRefreshInterval(1);
            } else {
                // 已经开始则停止计时
                StateManager::stopStopWatch();
                // 结束秒表模式，恢复低刷新率模式
                StateManager::updateOledRefreshInterval(100);
            }
            break;
        case DisplayGame:
            if (StateManager::getGameManY() == 31) {
                StateManager::updateGameManDY(-1);
            }
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
            StateManager::getCountdownInfo().countdownIndicator =
                    (StateManager::getCountdownInfo().countdownIndicator + 1) % 4;
            break;
        default:
            break;
    }
}

void center_button_long_press() {
    StateManager::updateState(DisplayMainMenu);
    StateManager::updateOledRefreshInterval(100);
}

void center_button_press() {
    Serial.println("center_button_press");
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
        case DisplayCountdownSet:
            StateManager::getCountdownInfo().countdownStartMillis = millis();
            StateManager::getCountdownInfo().countdownTimeInSeconds =
                    (StateManager::getCountdownInfo().number1 * 10 + StateManager::getCountdownInfo().number2) * 60 + (
                        StateManager::getCountdownInfo().number3 * 10 + StateManager::getCountdownInfo().number4);
            Serial.printf("确定倒计时时间: %d, %ld\n", StateManager::getCountdownInfo().countdownTimeInSeconds,
                          StateManager::getCountdownInfo().countdownStartMillis);
            StateManager::updateState(DisplayCountdown);
            break;
        case DisplayStopWatch:
            // 秒表模式，双击重置
            StateManager::resetStopWatch();
            break;
        default:
            break;
    }
}

void ButtonDetector::begin() {
    leftButton.setLongPressIntervalMs(2000);
    centerButton.setLongPressIntervalMs(2000);
    rightButton.setLongPressIntervalMs(2000);

    leftButton.attachClick(left_button_click);
    centerButton.attachClick(center_button_click);
    rightButton.attachClick(right_button_click);

    centerButton.attachLongPressStart(center_button_long_press);
    centerButton.attachDoubleClick(center_button_double_click);
    // centerButton.attachPress();

    xTaskCreate([](void *) {
        while (true) {
            leftButton.tick();
            centerButton.tick();
            rightButton.tick();
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }, "one-button", 4096, nullptr, 1, nullptr);
}
