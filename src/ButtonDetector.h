#ifndef OLED_CLOCK_BUTTONDETECTOR_H
#define OLED_CLOCK_BUTTONDETECTOR_H
#include "OneButton.h"

#define BUTTON_LEFT_PIN 18
#define BUTTON_CENTER_PIN 19
#define BUTTON_RIGHT_PIN 5

class ButtonDetector {
public:
    static void begin();

private :
    static OneButton leftButton;

    static OneButton centerButton;

    static OneButton rightButton;
};


#endif //OLED_CLOCK_BUTTONDETECTOR_H
