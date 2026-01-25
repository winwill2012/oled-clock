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
            return {locationName, "0", "0", "晴"};
        }
        const auto results = doc["results"].as<JsonArray>();
        const auto now = results[0]["now"].as<JsonObject>();
        return {
            locationName,
            now["temperature"].as<String>(),
            now["code"].as<String>(),
            now["text"].as<String>()
        };
    }
    Serial.printf("调用心知天气获取天气情况失败: %d\n", httpCode);
    return {locationName, "0", "0", "晴"};
}

// 通过天气代码获取天气描述（和官方有略微改动，最多保留两个字）
// 官网代码：https://www.doubao.com/chat/36419258016813826
const char *WeatherQuery::getChineseDescription(const char *code) {
    const int codeInt = strtol(code, nullptr, 10);
    switch (codeInt) {
        case 0:
        case 1:
        case 2:
        case 3: return "晴朗";
        case 4:
        case 5:
        case 6:
        case 7:
        case 8: return "多云";
        case 9: return "阴";
        case 10: return "阵雨";
        case 11:
        case 12: return "雷雨";
        case 13: return "小雨";
        case 14: return "中雨";
        case 15: return "大雨";
        case 16:
        case 17:
        case 18: return "暴雨";
        case 19: return "冻雨";
        case 20: return "雨雪";
        case 21: return "阵雪";
        case 22: return "小雪";
        case 23: return "中雪";
        case 24: return "大雪";
        case 25: return "暴雪";
        case 26: return "浮尘";
        case 27: return "扬沙";
        case 28:
        case 29: return "沙尘";
        case 30: return "雾";
        case 31: return "霾";
        case 32: return "风";
        case 33: return "大风";
        case 34: return "飓风";
        case 35: return "风暴";
        case 36: return "狂风";
        case 37: return "冷";
        case 38: return "热";
        default: return "未知"; // 处理不在列表中的数字
    }
}
