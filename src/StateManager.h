#ifndef OLED_CLOCK_STATEMANAGER_H
#define OLED_CLOCK_STATEMANAGER_H
#include <Arduino.h>
#include "WeatherQuery.h"

#define EVENT_STATE_CHANGED (1<<0)

enum GlobalState {
    Init,
    DisplayMainMenu,
    DisplayClock,
    DisplayCountdown,
    DisplayCountdownSet, // 倒计时设置状态
    DisplayCalendar,
    DisplayWeather,
};

struct CountdownInfo {
    uint8_t number1 = 0;
    uint8_t number2 = 0;
    uint8_t number3 = 0;
    uint8_t number4 = 0;
    unsigned long countdownStartMillis;
    int countdownTimeInSeconds;
    uint8_t countdownIndicator = 0;
};

class StateManager {
public:
    static void startWeatherQueryTask();

    static void updateState(GlobalState newState);

    static void updateWeather(const WeatherInfo &info);

    static WeatherInfo getWeatherInfo();

    static void updateStateByMenuItemIndex();

    static void increaseCalendarMonth();

    static void decreaseCalendarMonth();

    static void increaseMenuIndex();

    static void decreaseMenuIndex();

    static int getMenuItemIndex();

    static CountdownInfo &getCountdownInfo();

    static void increaseCountdownTime();

    static GlobalState getState();

    static void setCalendarMonth(uint8_t m);

    static void setCalendarYear(uint16_t y);

    static uint16_t getCalendarYear();

    static uint8_t getCalendarMonth();

    static EventGroupHandle_t getEventGroup();

    static const char *getWeatherCityName();

private:
    static int menuIndex;
    static CountdownInfo countdownInfo; // 开始倒计时时的单片机时间，单位毫秒
    static GlobalState state;
    static EventGroupHandle_t eventGroup;
    static uint16_t calendarYear;
    static uint8_t calendarMonth;
    static const char *weatherCityName;
    static WeatherInfo weatherInfo;
};


#endif //OLED_CLOCK_STATEMANAGER_H
