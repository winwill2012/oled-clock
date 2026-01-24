#ifndef OLED_CLOCK_WEATHERQUERY_H
#define OLED_CLOCK_WEATHERQUERY_H
#include <HTTPClient.h>

struct WeatherInfo {
    const char *cityName; // 城市
    const char *temperature; // 当前温度
    const char *weatherCode; // 天气情况代码
    const char *weather; // 天气情况
    WeatherInfo(const char *cityName, const char *temperature, const char *weatherCode, const char *weather) {
        this->cityName = cityName;
        this->temperature = temperature;
        this->weatherCode = weatherCode;
        this->weather = weather;
    }
};

class WeatherQuery {
public:
    WeatherQuery(const char *apiKey);

    void setLocation(const char *location);

    const char *getLocation() const;

    WeatherInfo getRealtimeWeatherInfo();

private:
    HTTPClient httpClient;
    const char *apiKey;
    const char *locationName = "昆明";
};


#endif //OLED_CLOCK_WEATHERQUERY_H
