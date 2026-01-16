#include "StateManager.h"

int StateManager::menuItemIndex = 1;
WiFiUDP StateManager::wifiUdp;
NTPClient StateManager::timeClient = NTPClient(wifiUdp, "time1.aliyun.com", 28800);
GlobalState StateManager::state = DisplayClock;
uint16_t StateManager::calendarYear;
uint8_t StateManager::calendarMonth;
uint16_t StateManager::weatherCityId = 100;
const char *StateManager::weatherCityName = "昆明";

EventGroupHandle_t StateManager::eventGroup = xEventGroupCreate();

NTPClient &StateManager::getTimeClient() {
    return timeClient;
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

uint16_t StateManager::getWeatherCityId() {
    return weatherCityId;
}

const char *StateManager::getWeatherCityName() {
    return weatherCityName;
}


void StateManager::updateState(GlobalState newState) {
    state = newState;
    xEventGroupSetBits(eventGroup, EVENT_STATE_CHANGED); // 状态发生变化时，通知OLED显示任务
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
            state = DisplayCountdown;
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
