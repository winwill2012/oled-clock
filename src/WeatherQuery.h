#ifndef OLED_CLOCK_WEATHERQUERY_H
#define OLED_CLOCK_WEATHERQUERY_H

struct WeatherInfo {
    const char *temp; // 当前温度
    const char *humidity; // 当前湿度
    const char *weather; // 天气情况
};

class WeatherQuery {
public:
    WeatherQuery();

    void setApiHost(const char *apiHost);

    void setLocation(const char *id, const char *locationName);

    WeatherInfo getRealtimeWeatherInfo();

private:
    const char *apiHost;
    const char *locationId = nullptr;
    const char *locationName = nullptr;
};


#endif //OLED_CLOCK_WEATHERQUERY_H
