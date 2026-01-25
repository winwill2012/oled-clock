#include "StateManager.h"
#include "WeatherQuery.h"

int StateManager::menuItemIndex = 1;
int StateManager::countdownIndicator = 1;
int StateManager::countdownTime = 500;
int StateManager::countdownTimeInSeconds = 300;
unsigned long StateManager::countdownStartMillis;
GlobalState StateManager::state = DisplayClock;
uint16_t StateManager::calendarYear;
uint8_t StateManager::calendarMonth;
const char *StateManager::weatherCityName = "昆明";
WeatherInfo StateManager::weatherInfo = {weatherCityName, "0", "1", "晴"};

extern WeatherQuery weatherQuery;

EventGroupHandle_t StateManager::eventGroup = xEventGroupCreate();

int StateManager::getCountdownTimeInSeconds() {
    return countdownTimeInSeconds;
}

GlobalState StateManager::getState() {
    return state;
}

void StateManager::setCalendarMonth(uint8_t m) {
    calendarMonth = m;
}

void StateManager::setCalendarYear(uint16_t y) {
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
    // 每30s调用心知天气获取一次当前实时天气
    xTaskCreate([](void *ptr) {
        while (true) {
            WeatherInfo result = weatherQuery.getRealtimeWeatherInfo();
            updateWeather(result);
            vTaskDelay(pdMS_TO_TICKS(30000));
        }
    }, "weather-query-task", 8192, nullptr, 1, nullptr);
}

void StateManager::updateState(GlobalState newState) {
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
    switch (menuItemIndex) {
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
    menuItemIndex++;
    if (menuItemIndex == 5) {
        menuItemIndex = 1;
    }
}

void StateManager::decreaseMenuIndex() {
    menuItemIndex--;
    if (menuItemIndex == 0) {
        menuItemIndex = 4;
    }
}

int StateManager::getMenuItemIndex() {
    return menuItemIndex;
}

void StateManager::increaseCountdownIndicator() {
    countdownIndicator++;
    if (countdownIndicator == 5) {
        countdownIndicator = 1;
    }
}

void StateManager::decreaseCountdownIndicator() {
    countdownIndicator--;
    if (countdownIndicator == 0) {
        countdownIndicator = 4;
    }
}

int StateManager::getCountdownIndicator() {
    return countdownIndicator;
}

void StateManager::increaseCountdownTime() {
    switch (countdownIndicator) {
        case 1:
            countdownTime += 1000;
            if (countdownTime / 1000 > 9) {
                countdownTime -= 10000;
            }
            break;
        case 2:
            countdownTime += 100;
            if (countdownTime / 100 % 10 == 0) {
                countdownTime -= 1000;
            }
            break;
        case 3:
            countdownTime += 10;
            if (countdownTime / 10 % 10 == 0) {
                countdownTime -= 100;
            }
            break;
        default:
            countdownTime++;
            if (countdownTime % 10 == 0) {
                countdownTime -= 10;
            }
    }
}

int StateManager::getCountdownTime() {
    return countdownTime;
}

unsigned long StateManager::getCountdownStartMillis() {
    return countdownStartMillis;
}

void StateManager::updateCountdownTimeInSeconds() {
    countdownTimeInSeconds = (countdownTime / 1000 * 10 + countdownTime / 100 % 10) * 60 + (
                                 countdownTime / 10 % 10 * 10 + countdownTime % 10);
    countdownStartMillis = millis();
}
