#include "StateManager.h"

#include "Buzzer.h"
#include "WeatherQuery.h"

int StateManager::menuIndex = 1;
CountdownInfo StateManager::countdownInfo;
GlobalState StateManager::state = DisplayClock;
uint16_t StateManager::calendarYear;
uint8_t StateManager::calendarMonth;
const char *StateManager::weatherCityName = "昆明";
WeatherInfo StateManager::weatherInfo = {weatherCityName, "0", "1", "晴"};

extern WeatherQuery weatherQuery;
extern Buzzer buzzer;

EventGroupHandle_t StateManager::eventGroup = xEventGroupCreate();

GlobalState StateManager::getState() {
    return state;
}

void StateManager::setCalendarMonth(const uint8_t m) {
    calendarMonth = m;
}

void StateManager::setCalendarYear(const uint16_t y) {
    calendarYear = y;
}

uint16_t StateManager::getCalendarYear() {
    return calendarYear;
}

uint8_t StateManager::getCalendarMonth() {
    return calendarMonth;
}

EventGroupHandle_t StateManager::getEventGroup() {
    return eventGroup;
}

const char *StateManager::getWeatherCityName() {
    return weatherCityName;
}

void StateManager::startWeatherQueryTask() {
    // 每60s调用心知天气获取一次当前实时天气
    xTaskCreate([](void *ptr) {
        while (true) {
            WeatherInfo result = weatherQuery.getRealtimeWeatherInfo();
            updateWeather(result);
            vTaskDelay(pdMS_TO_TICKS(60000));
        }
    }, "weather-query-task", 8192, nullptr, 1, nullptr);
}

void StateManager::updateState(const GlobalState newState) {
    state = newState;
    xEventGroupSetBits(eventGroup, EVENT_STATE_CHANGED); // 状态发生变化时，通知OLED显示任务
}

void StateManager::updateWeather(const WeatherInfo &info) {
    Serial.printf("更新天气信息: %s\n", info.toString().c_str());
    weatherInfo = info;
}

WeatherInfo StateManager::getWeatherInfo() {
    return weatherInfo;
}

void StateManager::updateStateByMenuItemIndex() {
    switch (menuIndex) {
        case 1:
            state = DisplayClock;
            break;
        case 2:
            state = DisplayCalendar;
            break;
        case 3:
            state = DisplayWeather;
            break;
        case 4:
            buzzer.reset(); // 重新设置倒计时，把蜂鸣器状态重置一下
            state = DisplayCountdownSet;
            break;
        default:
            break;
    }
}

void StateManager::increaseCalendarMonth() {
    calendarMonth++;
    if (calendarMonth == 13) {
        calendarMonth = 1;
        calendarYear++;
    }
}

void StateManager::decreaseCalendarMonth() {
    calendarMonth--;
    if (calendarMonth == 0) {
        calendarMonth = 12;
        calendarYear--;
    }
}

void StateManager::increaseMenuIndex() {
    menuIndex++;
    if (menuIndex == 5) {
        menuIndex = 1;
    }
}

void StateManager::decreaseMenuIndex() {
    menuIndex--;
    if (menuIndex == 0) {
        menuIndex = 4;
    }
}

int StateManager::getMenuItemIndex() {
    return menuIndex;
}

CountdownInfo &StateManager::getCountdownInfo() {
    return countdownInfo;
}

void StateManager::increaseCountdownTime() {
    switch (getCountdownInfo().countdownIndicator) {
        case 0:
            getCountdownInfo().number1 = (getCountdownInfo().number1 + 1) % 6;
            break;
        case 1:
            getCountdownInfo().number2 = (getCountdownInfo().number2 + 1) % 10;
            break;
        case 2:
            getCountdownInfo().number3 = (getCountdownInfo().number3 + 1) % 6;
            break;
        default:
            getCountdownInfo().number4 = (getCountdownInfo().number4 + 1) % 10;
            break;
    }
}
