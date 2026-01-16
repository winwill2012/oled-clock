#ifndef OLED_CLOCK_OLEDDISPLAY_H
#define OLED_CLOCK_OLEDDISPLAY_H
#include <Arduino.h>
#include "U8g2lib.h"

#define OLED_IIC_DATA_PIN 21
#define OLED_IIC_SCL_PIN 22

#define DIGIT_HEIGHT 32
#define DIGIT_WIDTH 24
#define SINGLE_DIGIT_LENGTH (DIGIT_HEIGHT * DIGIT_WIDTH / 8)

#define SMALL_DIGIT_HEIGHT 12
#define SMALL_DIGIT_WIDTH 8
#define SINGLE_SMALL_DIGIT_LENGTH (SMALL_DIGIT_HEIGHT * SMALL_DIGIT_WIDTH / 8)


class OledDisplay {
public:
    OledDisplay(uint8_t sda, uint8_t scl);

    void begin();

    void loop();

    // 显示主菜单界面
    void displayMainMenu();

    void displayConnectWifiTips();

    void drawPageIndicator(int item);

    // 显示时钟界面
    void displayClock();

    // 显示倒计时界面
    void displayCountdown();

    // 显示日历界面
    void displayCalendar();

    // 显示天气情况
    void displayWeather();

    void drawWiFiAndBattery(const char *title);

private:
    U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;
};


#endif //OLED_CLOCK_OLEDDISPLAY_H
