#pragma once
#include "odroid_go.h"
#include "WeatherIcons.h"
#include "JsonListener.h"
#include "WundergroundConditions.h"
#include "WundergroundForecast.h"
#include "WundergroundAstronomy.h"
#include "simpleDSTadjust.h"

#define NTP_SERVERS "0.ch.pool.ntp.org", "1.ch.pool.ntp.org", "2.ch.pool.ntp.org"

class WeatherStationClass
{
  private:
    WGConditions conditions;
    WGForecast forecasts[12];
    WGAstronomy astronomy;

    bool IS_METRIC = true;
    bool IS_STYLE_12HR = false;
    bool drawn = false;
    bool dateDrawn = false;
    const uint8_t labelX = 110;
    const uint8_t valueX = 245;
    int fy, screen = 0, Utc_Offset;
    long lastDownloadUpdate;
    String WUNDERGROUND_CITY;
    String WUNDERGRROUND_API_KEY;
    String WUNDERGRROUND_LANGUAGE; // as per https://www.wunderground.com/weather/api/d/docs?d=resources/country-to-iso-matching
    String WUNDERGROUND_COUNTRY;
    const String degreeSign = "C";
    String date = "", oldDate = "", day, temp, WeatherParams, name, usabledata;
    char time_str[11];
    char *dstAbbrev;



    time_t now;
    struct tm *timeinfo;

    void updateData(bool msg);
    void drawTime();
    void drawCurrentWeather();
    void drawForecast();
    void drawForecastDetail(uint16_t x, uint16_t y, uint8_t dayIndex);
    void drawCurrentWeatherDetail();
    void drawLabelValue(uint8_t line, String label, String value);
    void drawForecastTable(uint8_t start);
    const char *getMeteoconIconFromProgmem(String iconText);
    const char *getMiniMeteoconIconFromProgmem(String iconText);
    const String getShortText(String iconText);
    bool GetParams(fs::FS &fs, const char *path);
    String GetTextData(String *source, String strname);

  public:
    void Run();
    WeatherStationClass();
    ~WeatherStationClass();
};
