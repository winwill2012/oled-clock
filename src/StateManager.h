#ifndef OLED_CLOCK_STATEMANAGER_H
#define OLED_CLOCK_STATEMANAGER_H
#include <Arduino.h>
#include <WiFiUdp.h>

#include "NTPClient.h"
#include "WeatherQuery.h"

#define EVENT_STATE_CHANGED (1<<0)

enum GlobalState {
    Init,
    DisplayMainMenu,
    DisplayClock,
    DisplayCountdown,
    DisplayCalendar,
    DisplayWeather,
};

class StateManager {
public:
    static void startWeatherQuery();

    static void updateState(GlobalState state);

    static void updateWeather(const WeatherInfo &info);

    static WeatherInfo getWeatherInfo();

    static void updateStateByMenuItemIndex();

    static void increaseCalendarMonth();

    static void decreaseCalendarMonth();

    static void increaseMenuIndex();

    static void decreaseMenuIndex();

    static int getMenuItemIndex();

    static NTPClient &getTimeClient();

    static GlobalState getState();

    static void setCalendarMonth(uint8_t m);

    static void setCalendarYear(uint16_t y);

    static uint16_t getCalendarYear();

    static uint8_t getCalendarMonth();

    static EventGroupHandle_t getEventGroup();

    static uint16_t getWeatherCityId();

    static const char *getWeatherCityName();

private:
    static int menuItemIndex;
    static WiFiUDP wifiUdp;
    static NTPClient timeClient;
    static GlobalState state;
    static EventGroupHandle_t eventGroup;
    static uint16_t calendarYear;
    static uint8_t calendarMonth;
    static uint16_t weatherCityId;
    static const char *weatherCityName;
    static WeatherInfo weatherInfo;
};


#endif //OLED_CLOCK_STATEMANAGER_H
