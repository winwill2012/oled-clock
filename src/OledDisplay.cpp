#include "OledDisplay.h"
#include "StateManager.h"
#include "bitmap.h"
#include "Buzzer.h"
#include "CommonUtils.h"
#include "NTPClient.h"
#include "WeatherQuery.h"

String clockWeekDays[7] = {"周日", "周一", "周二", "周三", "周四", "周五", "周六"};
String calendarWeekDays[7] = {"日", "一", "二", "三", "四", "五", "六"};
String menuItemNames[] = {"时钟", "日历", "天气", "计时", "秒表"};
uint8_t menuSize = sizeof(menuItemNames) / sizeof(menuItemNames[0]);
extern NTPClient timeClient;
extern tm timeInfo;
extern Buzzer buzzer;

OledDisplay::OledDisplay(const uint8_t sda, const uint8_t scl) : u8g2(U8G2_R0, scl, sda) {
}

void OledDisplay::begin() {
    u8g2.begin();
    u8g2.enableUTF8Print();
}

constexpr int PARTICLE_COUNT = 96;
constexpr int AREA_X = 10;
constexpr int AREA_Y = 16;
constexpr int AREA_W = 48;
constexpr int AREA_H = 48;

// 粒子结构体
struct Particle {
    int x;
    int y;
    int dx; // X方向速度
    int dy; // Y方向速度
};

Particle particles[PARTICLE_COUNT];

void OledDisplay::loop() {
    // 设置随机种子
    randomSeed(analogRead(0));
    // 初始化天气页面左侧的所有粒子的初始位置
    for (auto &particle: particles) {
        particle.x = AREA_X + random(AREA_W);
        particle.y = AREA_Y + random(AREA_H);
        particle.dx = (random(2) == 0) ? 1 : -1;
        particle.dy = (random(2) == 0) ? 1 : -1;
    }
    timeClient.begin();
    while (!timeClient.update()) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    unsigned long localEpochTime = timeClient.getEpochTime();
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
                case DisplayStopWatch:
                    display->displayStopWatch();
                    break;
                case DisplayClock:
                    display->displayClock();
                    break;
                case DisplayCountdownSet:
                    display->displayCountdownSet();
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
                case DisplayGame:
                    display->displayGame();
                    break;
                default:
                    break;
            }
            if (StateManager::getOledRefreshInterval() > 0) {
                vTaskDelay(pdMS_TO_TICKS(StateManager::getOledRefreshInterval()));
            }
        }
    }, "oled-display", 8192, this, 1, nullptr);
}

const uint8_t *getBitmapByDigit(const int value) {
    switch (value) {
        case 0: return IMAGE_0;
        case 1: return IMAGE_1;
        case 2: return IMAGE_2;
        case 3: return IMAGE_3;
        case 4: return IMAGE_4;
        case 5: return IMAGE_5;
        case 6: return IMAGE_6;
        case 7: return IMAGE_7;
        case 8: return IMAGE_8;
        default: return IMAGE_9;
    }
}

const uint8_t *getBitmapBySmallDigit(const int value) {
    switch (value) {
        case 0: return IMAGE_MINI_0;
        case 1: return IMAGE_MINI_1;
        case 2: return IMAGE_MINI_2;
        case 3: return IMAGE_MINI_3;
        case 4: return IMAGE_MINI_4;
        case 5: return IMAGE_MINI_5;
        case 6: return IMAGE_MINI_6;
        case 7: return IMAGE_MINI_7;
        case 8: return IMAGE_MINI_8;
        default: return IMAGE_MINI_9;
    }
}

void OledDisplay::displayMainMenu() {
    u8g2.firstPage();
    do {
        drawWiFiAndBattery("菜单选择");
        u8g2.setFont(u8g2_font_wqy16_t_gb2312);
        u8g2.drawButtonUTF8(21, 40,U8G2_BTN_HCENTER | U8G2_BTN_INV | U8G2_BTN_BW1, 0, 2, 4,
                            menuItemNames[(StateManager::getMenuItemIndex() + menuSize - 1) % menuSize].c_str());
        u8g2.drawButtonUTF8(64, 40,U8G2_BTN_HCENTER | U8G2_BTN_INV | U8G2_BTN_BW1 | U8G2_BTN_SHADOW1, 0, 2, 6,
                            menuItemNames[StateManager::getMenuItemIndex()].c_str());
        u8g2.drawButtonUTF8(107, 40,U8G2_BTN_HCENTER | U8G2_BTN_INV | U8G2_BTN_BW1, 0, 2, 4,
                            menuItemNames[(StateManager::getMenuItemIndex() + 1) % menuSize].c_str());
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

void OledDisplay::displayClock() {
    // 获取当前时间
    timeClient.update();
    unsigned long localEpochTime = timeClient.getEpochTime();
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
        u8g2.drawBitmap(4, 20, 3, 32, getBitmapByDigit(timeInfo.tm_hour / 10));
        u8g2.drawBitmap(28, 20, 3, 32, getBitmapByDigit(timeInfo.tm_hour % 10));
        u8g2.drawBitmap(52, 20, 1, 32, IMAGE_DOT);
        u8g2.drawBitmap(60, 20, 3, 32, getBitmapByDigit(timeInfo.tm_min / 10));
        u8g2.drawBitmap(84, 20, 3, 32, getBitmapByDigit(timeInfo.tm_min % 10));
        u8g2.drawBitmap(108, 40, 1, 12, getBitmapBySmallDigit(timeInfo.tm_sec / 10));
        u8g2.drawBitmap(116, 40, 1, 12, getBitmapBySmallDigit(timeInfo.tm_sec % 10));
    } while (u8g2.nextPage());
}

void OledDisplay::drawCountdownIndicator(const uint8_t numberIndex) {
    constexpr uint8_t line1Y = 60;
    constexpr uint8_t line2Y = line1Y + 1;
    constexpr uint8_t line3Y = line2Y + 1;
    uint8_t centerX = 0;
    // 计算每一位数字的指示箭头的横坐标
    switch (numberIndex) {
        case 0:
            centerX = 24;
            break;
        case 1:
            centerX = 48;
            break;
        case 2:
            centerX = 80;
            break;
        default:
            centerX = 104;
            break;
    }
    u8g2.drawPixel(centerX, line1Y);
    u8g2.drawPixel(centerX - 1, line2Y);
    u8g2.drawPixel(centerX, line2Y);
    u8g2.drawPixel(centerX + 1, line2Y);
    u8g2.drawPixel(centerX - 2, line3Y);
    u8g2.drawPixel(centerX - 1, line3Y);
    u8g2.drawPixel(centerX, line3Y);
    u8g2.drawPixel(centerX + 1, line3Y);
    u8g2.drawPixel(centerX + 2, line3Y);
}

void OledDisplay::displayCountdownSet() {
    auto countdownInfo = StateManager::getCountdownInfo();
    u8g2.firstPage();
    do {
        drawWiFiAndBattery("倒计时设置");
        u8g2.drawBitmap(12, 20, 3, 32, getBitmapByDigit(countdownInfo.number1));
        u8g2.drawBitmap(36, 20, 3, 32, getBitmapByDigit(countdownInfo.number2));
        u8g2.drawBitmap(60, 20, 1, 32, IMAGE_DOT);
        u8g2.drawBitmap(68, 20, 3, 32, getBitmapByDigit(countdownInfo.number3));
        u8g2.drawBitmap(92, 20, 3, 32, getBitmapByDigit(countdownInfo.number4));
        drawCountdownIndicator(countdownInfo.countdownIndicator);
    } while (u8g2.nextPage());
}

void OledDisplay::displayCountdown() {
    int time = StateManager::getCountdownInfo().countdownTimeInSeconds - static_cast<int>(
                   (millis() - StateManager::getCountdownInfo().countdownStartMillis) / 1000);
    // 倒计时结束，时间不再变小
    if (time <= 0) {
        time = 0;
        buzzer.beep();
    }
    u8g2.firstPage();
    do {
        drawWiFiAndBattery("倒计时");
        u8g2.drawBitmap(12, 20, 3, 32, getBitmapByDigit(time / 60 / 10));
        u8g2.drawBitmap(36, 20, 3, 32, getBitmapByDigit(time / 60 % 10));
        u8g2.drawBitmap(60, 20, 1, 32, IMAGE_DOT);
        u8g2.drawBitmap(68, 20, 3, 32, getBitmapByDigit(time % 60 / 10));
        u8g2.drawBitmap(92, 20, 3, 32, getBitmapByDigit(time % 60 % 10));
    } while (u8g2.nextPage());
}

void OledDisplay::displayStopWatch() {
    const int stopWatchMills = StateManager::getStopWatchMillis();
    u8g2.firstPage();
    do {
        drawWiFiAndBattery("秒表");
        u8g2.drawBitmap(12, 20, 3, 32, getBitmapByDigit(stopWatchMills / 1000 / 1000));
        u8g2.drawBitmap(36, 20, 3, 32, getBitmapByDigit(stopWatchMills / 1000 % 100 / 10));
        u8g2.drawBitmap(60, 20, 3, 32, getBitmapByDigit(stopWatchMills / 1000 % 10));
        u8g2.drawBitmap(92, 40, 1, 12, getBitmapBySmallDigit(stopWatchMills % 1000 / 100));
        u8g2.drawBitmap(100, 40, 1, 12, getBitmapBySmallDigit(stopWatchMills % 1000 % 100 / 10));
        u8g2.drawBitmap(108, 40, 1, 12, getBitmapBySmallDigit(stopWatchMills % 1000 % 10));
        u8g2.setFont(u8g2_font_wqy12_t_gb2312);
        u8g2.drawUTF8(92, 30, "毫秒");
    } while (u8g2.nextPage());
}


void OledDisplay::displayCalendar() {
    u8g2.firstPage();
    u8g2.setFontPosTop();
    do {
        u8g2.setFont(u8g2_font_wqy12_t_gb2312);
        constexpr int x = 2, y = 2, width = 15, height = 10;
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
    // 计算坐标随机粒子的坐标
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i].x += particles[i].dx;
        particles[i].y += particles[i].dy;
        // 碰到边界反弹
        if (particles[i].x <= AREA_X || particles[i].x >= AREA_X + AREA_W - 1) {
            particles[i].dx *= -1;
        }
        if (particles[i].y <= AREA_Y || particles[i].y >= AREA_Y + AREA_H - 1) {
            particles[i].dy *= -1;
        }
    }
    // 获取当前天气情况
    auto weatherInfo = StateManager::getWeatherInfo();
    char temp[15];
    snprintf(temp, sizeof(temp), "%s℃", weatherInfo.temperature.c_str());

    // 开始渲染画面
    u8g2.firstPage();
    do {
        drawWiFiAndBattery("当前天气");
        u8g2.setFont(u8g2_font_wqy12_t_gb2312);
        u8g2.drawButtonUTF8(93, 28,
                            U8G2_BTN_HCENTER | U8G2_BTN_INV | U8G2_BTN_BW1 | U8G2_BTN_SHADOW1, 0, 1, 2,
                            weatherInfo.cityName.c_str());
        u8g2.drawUTF8(66, 56, temp);
        u8g2.drawUTF8(90, 56, "|");
        u8g2.drawUTF8(97, 56, WeatherQuery::getChineseDescription(weatherInfo.weatherCode.c_str()));
        // 绘制所有粒子
        for (const auto &particle: particles) {
            u8g2.drawPixel(particle.x, particle.y);
        }
        // u8g2.drawBitmap(5, 16, 6, 48, IMAGE_EARTH);
    } while (u8g2.nextPage());
}

struct GameManPoint {
    int8_t x;
    int8_t y;
    int dy;
};

GameManPoint gameManPoints[] = {
    {10, 0, 0},
    {10, 1, 0},
    {10, 2, 0},
    {10, 3, 0},
};

inline void OledDisplay::drawGameMan() {
    for (int i = 0; i < sizeof(gameManPoints) / sizeof(gameManPoints[0]); i++) {
        u8g2.drawPixel(0, StateManager::getGameManY());
    }
}

void OledDisplay::displayGame() {
    StateManager::updateGameManY();
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_wqy12_t_gb2312);
        u8g2.drawFrame(0, 0, 96, 64);
        u8g2.drawButtonUTF8(114, 12, U8G2_BTN_BW1 | U8G2_BTN_INV | U8G2_BTN_HCENTER, 0, 1, 2, "得分");
        u8g2.drawStr(102, 32, "0");
        u8g2.drawButtonUTF8(114, 48, U8G2_BTN_BW1 | U8G2_BTN_INV | U8G2_BTN_HCENTER, 0, 1, 2, "纪录");
        u8g2.drawStr(102, 64, "9999");
        drawGameMan();
    } while (u8g2.nextPage());
    if (StateManager::getGameManY() == 1 && StateManager::getGameManDY() == -1) {
        StateManager::updateGameManDY(1);
    } else if (StateManager::getGameManY() == 31 && StateManager::getGameManDY() == 1) {
        StateManager::updateGameManDY(0);
    }
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
