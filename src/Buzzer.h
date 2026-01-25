#ifndef OLED_LEARNING_BUZZER_H
#define OLED_LEARNING_BUZZER_H

#define BUZZER_PIN 23

class Buzzer {
public:
    void begin();

    void beep();

    void reset();

private :
    bool isRinging = false;
};

#endif //OLED_LEARNING_BUZZER_H
