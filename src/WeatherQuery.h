#ifndef OLED_CLOCK_WEATHERQUERY_H
#define OLED_CLOCK_WEATHERQUERY_H
#include <HTTPClient.h>

struct WeatherInfo {
    String cityName; // 城市
    String temperature; // 当前温度
    String weatherCode; // 天气情况代码
    String weather; // 天气情况
    WeatherInfo(const String &cityName, const String &temperature,
                const String &weatherCode,
                const String &weather) {
        this->cityName = cityName;
        this->temperature = temperature;
        this->weatherCode = weatherCode;
        this->weather = weather;
    }

    String toString() const {
        return String("城市: ") + cityName +
               ", 天气代码：" + weatherCode +
               ", 天气情况: " + weather +
               ", 当前温度: " + temperature;
    }
};

class WeatherQuery {
public:
    WeatherQuery(const char *apiKey);

    void setLocation(const char *location);

    const char *getLocation() const;

    WeatherInfo getRealtimeWeatherInfo();

    static const char *getChineseDescription(const char *code);

private:
    HTTPClient httpClient;
    const char *apiKey;
    const char *locationName = "昆明";
};


#endif //OLED_CLOCK_WEATHERQUERY_H
