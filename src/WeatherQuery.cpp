#include "WeatherQuery.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

JsonDocument doc;

WeatherQuery::WeatherQuery(const char *apiKey) {
    this->apiKey = apiKey;
}

void WeatherQuery::setLocation(const char *location) {
    this->locationName = location;
}

const char *WeatherQuery::getLocation() const {
    return this->locationName;
}

WeatherInfo WeatherQuery::getRealtimeWeatherInfo() {
    char url[300];
    snprintf(url, sizeof(url),
             "https://api.seniverse.com/v3/weather/now.json?key=%s&location=%s&language=zh-Hans&unit=c", apiKey,
             locationName);
    httpClient.begin(url);
    int httpCode = httpClient.GET();
    if (httpCode == 200) {
        auto response = httpClient.getString();
        Serial.printf("调用心知天气获取天气情况成功: %s\n", response.c_str());
        const auto error = deserializeJson(doc, response);
        if (error) {
            Serial.print("获取天气数据失败: ");
            Serial.println(error.c_str());
            return {locationName, "未知", "未知", "未知"};
        }
        const auto result = doc["results"].as<JsonObject>();
        const auto now = result["now"].as<JsonObject>();
        return {
            locationName,
            now["temperature"].as<String>().c_str(),
            now["code"].as<String>().c_str(),
            now["text"].as<String>().c_str()
        };
    }
    Serial.printf("调用心知天气获取天气情况失败: %d\n", httpCode);
    return {locationName, "未知", "未知", "未知"};
}
