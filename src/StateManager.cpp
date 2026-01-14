#include "StateManager.h"

GlobalState StateManager::state = DisplayClock;
EventGroupHandle_t StateManager::eventGroup = xEventGroupCreate();

GlobalState StateManager::getState() {
    return state;
}

EventGroupHandle_t StateManager::getEventGroup() {
    return eventGroup;
}

void StateManager::updateState(GlobalState newState) {
    xEventGroupClearBits(eventGroup, 1 << state);
    state = newState;
    xEventGroupSetBits(eventGroup, 1 << state);
}
