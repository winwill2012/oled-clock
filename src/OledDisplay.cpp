#include "OledDisplay.h"
#include "StateManager.h"
#include "bitmap.h"

tm timeInfo{};
String weekDays[7] = {"周日", "周一", "周二", "周三", "周四", "周五", "周六"};

OledDisplay::OledDisplay(const uint8_t sda, const uint8_t scl) : u8g2(U8G2_R0, scl, sda),
                                                                 timeClient(udp, "time1.aliyun.com", 28800) {
}

void OledDisplay::begin() {
    Serial.printf("当前状态: %d\n", StateManager::getState());
    xEventGroupSetBits(StateManager::getEventGroup(), 1 << StateManager::getState());
    u8g2.begin();
    u8g2.enableUTF8Print();
    displayMainMenu();
    displayClock();
    displayCountdown();
    displayCalender();
}

uint8_t *getBitmapByDigit(const int value, int round, const bool reverse = false) {
    if (round == 0) {
        round = 5;
    }
    static uint8_t buffer[SINGLE_DIGIT_LENGTH];
    int beginIndex;
    if (reverse) {
        beginIndex = (value * SINGLE_DIGIT_LENGTH + (5 - round) * SINGLE_DIGIT_LENGTH / 4) % (
                         SINGLE_DIGIT_LENGTH * 10);
    } else {
        beginIndex = ((value * SINGLE_DIGIT_LENGTH + SINGLE_DIGIT_LENGTH * 10) - (5 - round) *
                      SINGLE_DIGIT_LENGTH / 4) %
                     (SINGLE_DIGIT_LENGTH * 10);
    }
    for (int i = 0; i < SINGLE_DIGIT_LENGTH; i++) {
        buffer[i] = COMBINED_IMAGE[beginIndex++];
    }
    return buffer;
}

uint8_t *getBitmapBySmallDigit(const int value, int round) {
    if (round == 0) {
        round = 5;
    }
    static uint8_t buffer[SINGLE_SMALL_DIGIT_LENGTH];
    int beginIndex = ((value * SINGLE_SMALL_DIGIT_LENGTH + SINGLE_SMALL_DIGIT_LENGTH * 10) - (5 - round) *
                      SINGLE_SMALL_DIGIT_LENGTH /
                      4) %
                     (SINGLE_SMALL_DIGIT_LENGTH * 10);
    for (int i = 0; i < SINGLE_SMALL_DIGIT_LENGTH; i++) {
        buffer[i] = COMBINED_MINI_IMAGE[beginIndex++];
    }
    return buffer;
}

void OledDisplay::displayMainMenu() {
    xTaskCreate([](void *p) {
        auto *display = static_cast<OledDisplay *>(p);
        while (true) {
            xEventGroupWaitBits(StateManager::getEventGroup(), 1 << DisplayMainMenu, false, true, portMAX_DELAY);
            Serial.println("显示主菜单");
            display->u8g2.firstPage();
            do {
                display->u8g2.setFont(u8g2_font_wqy16_t_gb2312);
                display->u8g2.drawUTF8(5, 20, "主菜单页面");
            } while (display->u8g2.nextPage());
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }, "display-menu", 4096, this, 1, nullptr);
}

void OledDisplay::drawPageIndicator(int item) {
    int pageIndicatorY = 63;
    switch (item) {
        case 1:
            u8g2.drawPixel(60, pageIndicatorY);
            u8g2.drawPixel(61, pageIndicatorY);
            u8g2.drawPixel(64, pageIndicatorY);
            u8g2.drawPixel(67, pageIndicatorY);
            break;
        case 2:
            u8g2.drawPixel(60, pageIndicatorY);
            u8g2.drawPixel(63, pageIndicatorY);
            u8g2.drawPixel(64, pageIndicatorY);
            u8g2.drawPixel(67, pageIndicatorY);
            break;
        case 3:
            u8g2.drawPixel(60, pageIndicatorY);
            u8g2.drawPixel(63, pageIndicatorY);
            u8g2.drawPixel(66, pageIndicatorY);
            u8g2.drawPixel(67, pageIndicatorY);
            break;
        default:
            break;
    }
}

void OledDisplay::displayClock() {
    xTaskCreate([](void *p) {
        auto *display = static_cast<OledDisplay *>(p);
        while (true) {
            xEventGroupWaitBits(StateManager::getEventGroup(), 1 << DisplayClock, false, true, portMAX_DELAY);
            Serial.println("显示时钟");
            // 获取当前时间
            display->timeClient.update(); // update函数内部做了更新间隔检测，并不会每一次都从网络获取时间
            unsigned long localEpochTime = display->timeClient.getEpochTime();
            gmtime_r(reinterpret_cast<time_t *>(&localEpochTime), &timeInfo);
            // 渲染时间
            display->u8g2.firstPage();
            do {
                display->u8g2.setFont(u8g2_font_siji_t_6x10);
                display->u8g2.drawGlyph(5, 8, 0xe1af);
                const uint32_t currentMilliVolts = analogReadMilliVolts(34) * 3;
                if (currentMilliVolts >= 3950) {
                    // 满电
                    display->u8g2.drawGlyph(111, 8, 0xe23D);
                } else if (currentMilliVolts > 3700) {
                    // 中等电量
                    display->u8g2.drawGlyph(111, 8, 0xe23C);
                } else {
                    // 低电量需要充电
                    display->u8g2.drawGlyph(111, 8, 0xe23B);
                }
                char date[15];
                snprintf(date, sizeof(date), "%d%d/%d%d %s", (timeInfo.tm_mon + 1) / 10,
                         (timeInfo.tm_mon + 1) % 10,
                         timeInfo.tm_mday / 10, timeInfo.tm_mday % 10,
                         weekDays[timeInfo.tm_wday].c_str());
                Serial.println(date);
                display->u8g2.setFont(u8g2_font_wqy12_t_gb2312);
                display->u8g2.drawUTF8((display->u8g2.getDisplayWidth() - display->u8g2.getUTF8Width(date)) / 2,
                                       display->u8g2.getAscent() + 2, date);
                display->u8g2.drawBitmap(4, 20, 3, 32, getBitmapByDigit(timeInfo.tm_hour / 10, 0));
                display->u8g2.drawBitmap(28, 20, 3, 32, getBitmapByDigit(timeInfo.tm_hour % 10, 0));
                display->u8g2.drawBitmap(52, 20, 1, 32, IMAGE_DOT);
                display->u8g2.drawBitmap(60, 20, 3, 32, getBitmapByDigit(timeInfo.tm_min / 10, 0));
                display->u8g2.drawBitmap(84, 20, 3, 32, getBitmapByDigit(timeInfo.tm_min % 10, 0));
                display->u8g2.drawBitmap(108, 40, 1, 12, getBitmapBySmallDigit(timeInfo.tm_sec / 10, 0));
                display->u8g2.drawBitmap(116, 40, 1, 12, getBitmapBySmallDigit(timeInfo.tm_sec % 10, 0));
                display->drawPageIndicator(2);
                display->u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
                display->u8g2.setFontPosTop();
                // u8g2.drawGlyph(108, 18, 0x0040);  // 天阴
                // u8g2.drawGlyph(108, 18, 0x0041);  // 多云
                // u8g2.drawGlyph(108, 18, 0x0042);  // 月亮
                // u8g2.drawGlyph(108, 18, 0x0043);  // 下雨
                // u8g2.drawGlyph(108, 18, 0x0044);  // 星星
                display->u8g2.drawGlyph(108, 18, 0x0045); // 天晴
                display->u8g2.setFontPosBaseline();
            } while (display->u8g2.nextPage());
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }, "display-clock", 8092, this, 1, nullptr);
}

void OledDisplay::displayCountdown() {
    xTaskCreate([](void *p) {
        const auto display = static_cast<OledDisplay *>(p);
        while (true) {
            xEventGroupWaitBits(StateManager::getEventGroup(), 1 << DisplayCountdown, false, true, portMAX_DELAY);
            Serial.println("显示倒计时");
            display->u8g2.firstPage();
            do {
                display->u8g2.setFont(u8g2_font_wqy16_t_gb2312);
                display->u8g2.drawUTF8(10, 25, "倒计时");
            } while (display->u8g2.nextPage());
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }, "display-countdown", 8092, this, 1, nullptr);
}

void OledDisplay::displayCalender() {
    xTaskCreate([](void *p) {
        const auto display = static_cast<OledDisplay *>(p);
        while (true) {
            xEventGroupWaitBits(StateManager::getEventGroup(), 1 << DisplayCalender, false, true, portMAX_DELAY);
            Serial.println("显示日历");
            display->u8g2.firstPage();
            do {
                display->u8g2.setFont(u8g2_font_wqy16_t_gb2312);
                display->u8g2.drawUTF8(10, 25, "日历");
            } while (display->u8g2.nextPage());
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }, "display-calender", 8092, this, 1, nullptr);
}
