#ifndef OLED_CLOCK_STATEMANAGER_H
#define OLED_CLOCK_STATEMANAGER_H
#include <Arduino.h>

enum GlobalState {
    Init,
    DisplayMainMenu,
    DisplayClock,
    DisplayCountdown,
    DisplayCalender,
};

class StateManager {
public:
    static void updateState(GlobalState state);

    static GlobalState getState();

    static EventGroupHandle_t getEventGroup();

private:
    static GlobalState state;
    static EventGroupHandle_t eventGroup;
};


#endif //OLED_CLOCK_STATEMANAGER_H
