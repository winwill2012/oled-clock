#include "StateManager.h"

#include "Buzzer.h"
#include "WeatherQuery.h"

int StateManager::menuIndex = 1;
CountdownInfo StateManager::countdownInfo;
int StateManager::gameManY = 31;
int StateManager::gameManDY = 0;
unsigned long StateManager::stopWatchStartMills = 0;
unsigned long StateManager::frozenStopWatchMills = 0;
int StateManager::oledRefreshInterval = 100;
GlobalState StateManager::state = DisplayGame;
uint16_t StateManager::calendarYear;
uint8_t StateManager::calendarMonth;
const char *StateManager::weatherCityName = "昆明";
WeatherInfo StateManager::weatherInfo = {weatherCityName, "0", "1", "晴"};

extern WeatherQuery weatherQuery;
extern Buzzer buzzer;
extern uint8_t menuSize;

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

void StateManager::startStopWatch() {
    stopWatchStartMills = millis();
}

void StateManager::stopStopWatch() {
    if (frozenStopWatchMills == 0) {
        frozenStopWatchMills = millis() - stopWatchStartMills;
    }
    stopWatchStartMills = 0;
}

void StateManager::resetStopWatch() {
    stopWatchStartMills = 0;
    frozenStopWatchMills = 0;
}

int StateManager::getStopWatchMillis() {
    if (frozenStopWatchMills > 0) {
        return frozenStopWatchMills;
    }
    if (stopWatchStartMills > 0) {
        return millis() - stopWatchStartMills;
    }
    return 0;
}

void StateManager::updateOledRefreshInterval(int t) {
    oledRefreshInterval = t;
}

int StateManager::getOledRefreshInterval() {
    return oledRefreshInterval;
}

int StateManager::getGameManY() {
    return gameManY;
}

void StateManager::updateGameManY() {
    gameManY += gameManDY;
}

int StateManager::getGameManDY() {
    return gameManDY;
}

void StateManager::updateGameManDY(int d) {
    gameManDY = d;
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
        case 0:
            state = DisplayClock;
            break;
        case 1:
            state = DisplayCalendar;
            break;
        case 2:
            state = DisplayWeather;
            break;
        case 3:
            buzzer.reset(); // 重新设置倒计时，把蜂鸣器状态重置一下
            state = DisplayCountdownSet;
            break;
        case 4:
            state = DisplayStopWatch;
            break;
        default:
            updateOledRefreshInterval(0);
            state = DisplayGame;
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
    menuIndex = (menuIndex + 1) % menuSize;
}

void StateManager::decreaseMenuIndex() {
    menuIndex = (menuIndex + menuSize - 1) % menuSize;
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
