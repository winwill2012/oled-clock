#include "OledDisplay.h"
#include "StateManager.h"
#include "bitmap.h"
#include "CommonUtils.h"

tm timeInfo{};
String clockWeekDays[7] = {"周日", "周一", "周二", "周三", "周四", "周五", "周六"};
String calendarWeekDays[7] = {"日", "一", "二", "三", "四", "五", "六"};
String menuItemNames[6] = {"计时", "时钟", "日历", "天气", "计时", "时钟"};

OledDisplay::OledDisplay(const uint8_t sda, const uint8_t scl) : u8g2(U8G2_R0, scl, sda) {
}

void OledDisplay::begin() {
    u8g2.begin();
    u8g2.enableUTF8Print();
}

void OledDisplay::loop() {
    StateManager::getTimeClient().begin();
    while (!StateManager::getTimeClient().update()) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    unsigned long localEpochTime = StateManager::getTimeClient().getEpochTime();
    gmtime_r(reinterpret_cast<time_t *>(&localEpochTime), &timeInfo);
    StateManager::setCalendarYear(timeInfo.tm_year + 1900);
    StateManager::setCalendarMonth(timeInfo.tm_mon + 1);
    xTaskCreate([](void *p) {
        auto *display = static_cast<OledDisplay *>(p);
        while (true) {
            switch (StateManager::getState()) {
                case DisplayMainMenu:
                    display->displayMainMenu();
                    break;
                case DisplayClock:
                    display->displayClock();
                    break;
                case DisplayCountdown:
                    display->displayCountdown();
                    break;
                case DisplayCalendar:
                    display->displayCalendar();
                    break;
                case DisplayWeather:
                    display->displayWeather();
                    break;
                default:
                    break;
            }
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }, "oled-display", 8192, this, 1, nullptr);
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
    u8g2.firstPage();
    do {
        drawWiFiAndBattery("菜单选择");
        u8g2.setFont(u8g2_font_wqy16_t_gb2312);
        u8g2.drawButtonUTF8(21, 40,U8G2_BTN_HCENTER | U8G2_BTN_INV | U8G2_BTN_BW1, 0, 2, 4,
                            menuItemNames[StateManager::getMenuItemIndex() - 1].c_str());
        u8g2.drawButtonUTF8(64, 40,U8G2_BTN_HCENTER | U8G2_BTN_INV | U8G2_BTN_BW1 | U8G2_BTN_SHADOW1, 0, 2, 6,
                            menuItemNames[StateManager::getMenuItemIndex()].c_str());
        u8g2.drawButtonUTF8(107, 40,U8G2_BTN_HCENTER | U8G2_BTN_INV | U8G2_BTN_BW1, 0, 2, 4,
                            menuItemNames[StateManager::getMenuItemIndex() + 1].c_str());
    } while (u8g2.nextPage());
}

void OledDisplay::displayConnectWifiTips() {
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_streamline_all_t);
        u8g2.drawGlyph(54, 32, 0x0091);
        u8g2.setFont(u8g2_font_wqy16_t_gb2312);
        u8g2.setFontPosTop();
        u8g2.drawUTF8((u8g2.getDisplayWidth() - u8g2.getUTF8Width("等待联网")) / 2,
                      45, "等待联网");
        u8g2.setFontPosBaseline();
    } while (u8g2.nextPage());
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
    // 获取当前时间
    StateManager::getTimeClient().update();
    unsigned long localEpochTime = StateManager::getTimeClient().getEpochTime();
    gmtime_r(reinterpret_cast<time_t *>(&localEpochTime), &timeInfo);
    // 渲染时间
    u8g2.firstPage();
    do {
        char date[15];
        snprintf(date, sizeof(date), "%d%d/%d%d %s", (timeInfo.tm_mon + 1) / 10,
                 (timeInfo.tm_mon + 1) % 10,
                 timeInfo.tm_mday / 10, timeInfo.tm_mday % 10,
                 clockWeekDays[timeInfo.tm_wday].c_str());
        drawWiFiAndBattery(date);
        u8g2.setFont(u8g2_font_wqy12_t_gb2312);
        u8g2.drawUTF8((u8g2.getDisplayWidth() - u8g2.getUTF8Width(date)) / 2,
                      u8g2.getAscent() + 2, date);
        u8g2.drawBitmap(4, 20, 3, 32, getBitmapByDigit(timeInfo.tm_hour / 10, 0));
        u8g2.drawBitmap(28, 20, 3, 32, getBitmapByDigit(timeInfo.tm_hour % 10, 0));
        u8g2.drawBitmap(52, 20, 1, 32, IMAGE_DOT);
        u8g2.drawBitmap(60, 20, 3, 32, getBitmapByDigit(timeInfo.tm_min / 10, 0));
        u8g2.drawBitmap(84, 20, 3, 32, getBitmapByDigit(timeInfo.tm_min % 10, 0));
        u8g2.drawBitmap(108, 40, 1, 12, getBitmapBySmallDigit(timeInfo.tm_sec / 10, 0));
        u8g2.drawBitmap(116, 40, 1, 12, getBitmapBySmallDigit(timeInfo.tm_sec % 10, 0));
        // drawPageIndicator(2);
        u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
        u8g2.setFontPosTop();
        // u8g2.drawGlyph(108, 18, 0x0040);  // 天阴
        // u8g2.drawGlyph(108, 18, 0x0041);  // 多云
        // u8g2.drawGlyph(108, 18, 0x0042);  // 月亮
        // u8g2.drawGlyph(108, 18, 0x0043);  // 下雨
        // u8g2.drawGlyph(108, 18, 0x0044);  // 星星
        u8g2.drawGlyph(108, 18, 0x0045); // 天晴
        u8g2.setFontPosBaseline();
    } while (u8g2.nextPage());
}

void OledDisplay::displayCountdown() {
    u8g2.firstPage();
    do {
        drawWiFiAndBattery("倒计时");
    } while (u8g2.nextPage());
}


void OledDisplay::displayCalendar() {
    u8g2.firstPage();
    u8g2.setFontPosTop();
    do {
        u8g2.setFont(u8g2_font_wqy12_t_gb2312);
        int x = 2, y = 2, width = 15, height = 10;
        int d = 1;
        char date[3];
        u8g2.drawFrame(0, 0, x + width * 7, 64);
        for (int i = 0; i < 7; i++) {
            u8g2.drawUTF8(x + (i * width), y, calendarWeekDays[i].c_str());
        }
        // 所选月份第一天对应周几
        const int startWeekDay = CommonUtils::calculateWeekday(StateManager::getCalendarYear(),
                                                               StateManager::getCalendarMonth(), 1);
        const int maxDays = CommonUtils::getDaysInMonth(StateManager::getCalendarYear(),
                                                        StateManager::getCalendarMonth());
        for (int i = 1; i < 6; i++) {
            const uint8_t startJ = i == 1 ? startWeekDay : 0;
            for (int j = startJ; j < 7; j++) {
                snprintf(date, sizeof(date), "%02d", d++);
                if (StateManager::getCalendarYear() == timeInfo.tm_year + 1900 &&
                    StateManager::getCalendarMonth() == timeInfo.tm_mon + 1 &&
                    d - 1 == timeInfo.tm_mday) {
                    u8g2.drawButtonUTF8(x + (j * width), y + (i * height), U8G2_BTN_INV | U8G2_BTN_BW0, 0, 2, 1, date);
                } else {
                    u8g2.drawUTF8(x + (j * width), y + (i * height), date);
                }
                if (d > maxDays) {
                    break;
                }
            }
            if (d > maxDays) {
                break;
            }
        }
        char month[3], year[3];
        snprintf(year, sizeof(year), "%02d", StateManager::getCalendarYear() % 100);
        snprintf(month, sizeof(month), "%02d", StateManager::getCalendarMonth());
        u8g2.drawButtonUTF8(117, 3, U8G2_BTN_BW0 | U8G2_BTN_INV | U8G2_BTN_HCENTER, 0, 3, 3, year);
        u8g2.drawButtonUTF8(117, 20, U8G2_BTN_BW0 | U8G2_BTN_HCENTER, 0, 0, 2, "年");
        u8g2.drawButtonUTF8(117, 34, U8G2_BTN_BW0 | U8G2_BTN_INV | U8G2_BTN_HCENTER, 0, 3, 3, month);
        u8g2.drawButtonUTF8(117, u8g2.getDisplayHeight() - (u8g2.getAscent() - u8g2.getDescent()),
                            U8G2_BTN_BW0 | U8G2_BTN_HCENTER, 0, 0, 2, "月");
    } while (u8g2.nextPage());
    u8g2.setFontPosBaseline();
}

/**
 * 显示天气情况
 */
void OledDisplay::displayWeather() {
    u8g2.firstPage();
    do {
        drawWiFiAndBattery("当前天气");
        u8g2.setFont(u8g2_font_wqy12_t_gb2312);
        u8g2.drawButtonUTF8(93, 32,
                            U8G2_BTN_HCENTER | U8G2_BTN_INV | U8G2_BTN_BW1 | U8G2_BTN_SHADOW1, 0, 1, 2,
                            StateManager::getWeatherCityName());
        u8g2.drawUTF8(66, 56, "18℃");
        u8g2.drawUTF8(90, 56, "|");
        u8g2.drawUTF8(97, 56, "24%");
        u8g2.drawVLine(58, 16, u8g2.getDisplayHeight() - 16);
        u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
        u8g2.drawGlyph(5, 64, 0x0045);
    } while (u8g2.nextPage());
}

/**
 * 在屏幕顶部显示WiFi和电池电量
 */
void OledDisplay::drawWiFiAndBattery(const char *title) {
    if (strcmp(title, "") != 0) {
        u8g2.setFont(u8g2_font_wqy12_t_gb2312);
        u8g2.drawUTF8((u8g2.getDisplayWidth() - u8g2.getUTF8Width(title)) / 2,
                      u8g2.getAscent() + 2, title);
    }
    u8g2.setFont(u8g2_font_siji_t_6x10);
    u8g2.drawGlyph(5, 8, 0xe1af);
    const uint32_t currentMilliVolts = analogReadMilliVolts(34) * 3;
    if (currentMilliVolts >= 3950) {
        // 满电
        u8g2.drawGlyph(111, 8, 0xe23D);
    } else if (currentMilliVolts > 3700) {
        // 中等电量
        u8g2.drawGlyph(111, 8, 0xe23C);
    } else {
        // 低电量需要充电
        u8g2.drawGlyph(111, 8, 0xe23B);
    }
}
