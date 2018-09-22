#include "WeatherStation.h"

String WeatherStationClass::GetTextData(String *source, String strname)
{
    int t = ((source->indexOf(strname)) + (strname.length()));
    int r = source->indexOf(';', t);
    usabledata = source->substring(t, r);

    return usabledata;
}

bool WeatherStationClass::GetParams(fs::FS &fs, const char *path)
{
    File sdfile = fs.open(path);
    if (!sdfile)
    {
        return false;
    }

    if (sdfile.available())
    {
        WeatherParams = sdfile.readStringUntil(EOF);
        WUNDERGROUND_CITY = GetTextData(&WeatherParams, String("CITY="));
        WUNDERGROUND_COUNTRY = GetTextData(&WeatherParams, String("COUNTRY="));
        WUNDERGRROUND_LANGUAGE = GetTextData(&WeatherParams, String("LANGUAGE="));
        WUNDERGRROUND_API_KEY = GetTextData(&WeatherParams, String("APIKEY="));
        Utc_Offset = atoi((GetTextData(&WeatherParams, String("TIME_OFFSET="))).c_str());
    }

    else
    {
        return false;
    }

    sdfile.close();
    return true;
}

void WeatherStationClass::updateData(bool msg = false)
{

    if (msg)
    {
        GO.Lcd.fillScreen(BLACK);
        GO.Lcd.setTextColor(WHITE);
        GO.Lcd.setFreeFont(FSS12);
        GO.Lcd.drawCentreString("Reading Weather Info...", 160, 120, 1);
    }

    configTime(Utc_Offset * 3600, 0, NTP_SERVERS);
    WundergroundConditions *conditionsClient = new WundergroundConditions(IS_METRIC);
    conditionsClient->updateConditions(&conditions, WUNDERGRROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
    delete conditionsClient;
    conditionsClient = nullptr;

    WundergroundForecast *forecastClient = new WundergroundForecast(IS_METRIC);
    forecastClient->updateForecast(forecasts, 12, WUNDERGRROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
    delete forecastClient;
    forecastClient = nullptr;

    WundergroundAstronomy *astronomyClient = new WundergroundAstronomy(IS_STYLE_12HR);
    astronomyClient->updateAstronomy(&astronomy, WUNDERGRROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
    delete astronomyClient;
    astronomyClient = nullptr;
}

dstRule StartRule = {"CEST", Last, Sun, Mar, 2, 3600}; // Central European Summer Time = UTC/GMT +2 hours
dstRule EndRule = {"CET", Last, Sun, Oct, 2, 0};       // Central European Time = UTC/GMT +1 hour
simpleDSTadjust dstAdjusted(StartRule, EndRule);

void WeatherStationClass::drawTime()
{
    GO.Lcd.setTextColor(BLACK);
    GO.Lcd.setFreeFont(FSS18);
    GO.Lcd.drawRightString(time_str, 315, 5, 1);
    now = dstAdjusted.time(&dstAbbrev);
    timeinfo = localtime(&now);
    date = ctime(&now);
    date = date.substring(0, 11) + String(1900 + timeinfo->tm_year);
    if (!dateDrawn || (timeinfo->tm_min == 0 && timeinfo->tm_sec == 1))
    {
        GO.Lcd.setTextColor(BLACK);
        GO.Lcd.setFreeFont(FSS9);
        GO.Lcd.drawString(oldDate, 5, 18, 1);
        GO.Lcd.setTextColor(WHITE);
        GO.Lcd.setFreeFont(FSS9);
        GO.Lcd.drawString(date, 5, 18, 1);
        oldDate = date;
        dateDrawn = true;
    }

    if (drawn && ((timeinfo->tm_min) % 10 == 0) && timeinfo->tm_sec == 5)
    {
        updateData(false);
        drawn = false;
    }

    sprintf(time_str, "%02d:%02d:%02d\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    GO.Lcd.setTextColor(WHITE);
    GO.Lcd.setFreeFont(FSS18);
    GO.Lcd.drawRightString(time_str, 315, 5, 1);
}

void WeatherStationClass::drawCurrentWeather()
{
    GO.Lcd.drawFastHLine(0, 40, 320, WHITE);
    GO.Lcd.drawJpg((uint8_t *)getMeteoconIconFromProgmem(conditions.weatherIcon),
                    sizeof(getMeteoconIconFromProgmem(conditions.weatherIcon)) / getMeteoconIconFromProgmem(conditions.weatherIcon)[0], 100, 45);
    GO.Lcd.setTextColor(BLUE);
    GO.Lcd.setFreeFont(FSS12);
    GO.Lcd.drawRightString(WUNDERGROUND_CITY, 315, 50, 1);
    GO.Lcd.setTextColor(WHITE);

    String temp = conditions.currentTemp + degreeSign;
    GO.Lcd.setFreeFont(FSS24);
    GO.Lcd.drawRightString(temp, 315, 75, 1);
    GO.Lcd.setTextColor(ORANGE);
    GO.Lcd.setFreeFont(FSS9);
    GO.Lcd.drawRightString(conditions.weatherText, 315, 117, 1);
    GO.Lcd.drawFastHLine(0, 140, 320, WHITE);
    GO.Lcd.setTextColor(CYAN);
    GO.Lcd.drawCentreString("Hum: " + conditions.humidity, 40, 50, 1);
    GO.Lcd.drawCentreString("Precip: " + forecasts[0].PoP + "%", 40, 117, 1);
    GO.Lcd.setTextColor(YELLOW);
    GO.Lcd.drawCentreString("Wind: " + conditions.windDir, 40, 75, 1);
    GO.Lcd.drawCentreString(conditions.windSpeed, 40, 95, 1);
}

void WeatherStationClass::drawForecastDetail(uint16_t x, uint16_t y, uint8_t dayIndex)
{
    day = forecasts[dayIndex].forecastTitle.substring(0, 3);
    day.toUpperCase();
    GO.Lcd.setTextColor(YELLOW);
    GO.Lcd.setFreeFont(FSS9);
    GO.Lcd.drawCentreString(day, x + 25, y - 15, 2);
    GO.Lcd.setTextColor(WHITE);
    GO.Lcd.drawCentreString(forecasts[dayIndex].forecastLowTemp + "|" + forecasts[dayIndex].forecastHighTemp, x + 25, y, 2);
    GO.Lcd.drawJpg((uint8_t *)getMiniMeteoconIconFromProgmem(forecasts[dayIndex].forecastIcon),
                    sizeof(getMiniMeteoconIconFromProgmem(forecasts[dayIndex].forecastIcon)) / getMiniMeteoconIconFromProgmem(forecasts[dayIndex].forecastIcon)[0], x, y + 15);
    GO.Lcd.setTextColor(BLUE);
    GO.Lcd.setFreeFont(FSS9);
    GO.Lcd.drawCentreString(forecasts[dayIndex].PoP + "%", x + 25, y + 60, 2);
}

void WeatherStationClass::drawCurrentWeatherDetail()
{
    GO.Lcd.drawFastHLine(0, 20, 320, WHITE);
    GO.Lcd.drawFastVLine(100, 20, 210, WHITE);
    GO.Lcd.drawFastHLine(0, 230, 320, WHITE);
    GO.Lcd.drawJpg((uint8_t *)getMeteoconIconFromProgmem(conditions.weatherIcon),
                    sizeof(getMeteoconIconFromProgmem(conditions.weatherIcon)) / getMeteoconIconFromProgmem(conditions.weatherIcon)[0], 0, 25);
    GO.Lcd.setTextColor(WHITE);
    GO.Lcd.setFreeFont(FSS9);
    GO.Lcd.drawCentreString("Current Conditions", 160, 2, 1);
    GO.Lcd.setTextColor(CYAN);
    GO.Lcd.setFreeFont(FSS12);
    GO.Lcd.drawCentreString(conditions.weatherText, 220, 25, 1);
    drawLabelValue(0, "Temperature:", conditions.currentTemp + degreeSign);
    drawLabelValue(1, "Humidity:", conditions.humidity);
    drawLabelValue(2, "Feels Like:", conditions.feelslike + degreeSign);
    drawLabelValue(3, "Dew Point:", conditions.dewPoint + degreeSign);
    drawLabelValue(4, "Wind Speed:", conditions.windSpeed);
    drawLabelValue(5, "Wind Dir:", conditions.windDir);
    drawLabelValue(6, "Pressure:", conditions.pressure);
    drawLabelValue(7, "Precipitation:", conditions.precipitationToday);
    drawLabelValue(8, "UV Index:", conditions.UV);

    GO.Lcd.setTextColor(YELLOW);
    GO.Lcd.drawCentreString("SunRise:", 45, 130, 1);
    GO.Lcd.drawCentreString("SunSet:", 45, 170, 1);
    GO.Lcd.setTextColor(WHITE);
    GO.Lcd.drawCentreString(astronomy.sunriseTime, 45, 150, 1);
    GO.Lcd.drawCentreString(astronomy.sunsetTime, 45, 190, 1);
}

void WeatherStationClass::drawLabelValue(uint8_t line, String label, String value)
{
    GO.Lcd.setFreeFont(FSS9);
    GO.Lcd.setTextColor(ORANGE);
    GO.Lcd.drawString(label, labelX, 52 + line * 20, 1);
    GO.Lcd.setTextColor(WHITE);
    GO.Lcd.drawString(value, valueX, 52 + line * 20, 1);
}

void WeatherStationClass::drawForecastTable(uint8_t start)
{
    GO.Lcd.drawFastHLine(0, 20, 320, WHITE);
    GO.Lcd.drawFastVLine(60, 20, 210, WHITE);
    GO.Lcd.setTextColor(WHITE);
    GO.Lcd.setFreeFont(FSS9);
    GO.Lcd.drawCentreString("Forecasts", 160, 2, 1);
    fy = 0;
    for (uint8_t i = start; i < start + 4; i++)
    {
        fy = 32 + (i - start) * 52;
        if (fy > 240)
        {
            break;
        }
        GO.Lcd.drawJpg((uint8_t *)getMiniMeteoconIconFromProgmem(forecasts[i].forecastIcon),
                        sizeof(getMiniMeteoconIconFromProgmem(forecasts[i].forecastIcon)) / getMiniMeteoconIconFromProgmem(forecasts[i].forecastIcon)[0], 0, fy - 10);
        GO.Lcd.setTextColor(ORANGE);
        GO.Lcd.setFreeFont(FSS9);
        GO.Lcd.drawString(forecasts[i].forecastTitle, 70, (fy - 4), 1);
        GO.Lcd.setTextColor(WHITE);
        GO.Lcd.drawString(getShortText(forecasts[i].forecastIcon), 70, fy + 18, 1);
        GO.Lcd.setTextColor(WHITE);
        if (i % 2 == 0)
        {
            temp = forecasts[i].forecastHighTemp;
        }
        else
        {
            temp = forecasts[i - 1].forecastLowTemp;
        }
        GO.Lcd.setFreeFont(FSS9);
        GO.Lcd.drawRightString("Temp: " + temp + degreeSign, 315, (fy - 4), 1);
        GO.Lcd.setTextColor(BLUE);
        GO.Lcd.drawRightString("Precip: " + forecasts[i].PoP + "%", 315, fy + 18, 1);
        GO.Lcd.drawFastHLine(0, fy + 40, 320, WHITE);
    }
}

const char *WeatherStationClass::getMeteoconIconFromProgmem(String iconText)
{

    if (iconText == "chanceflurries")
        return chanceflurries;
    if (iconText == "chancerain")
        return chancerain;
    if (iconText == "chancesleet")
        return chancesleet;
    if (iconText == "chancesnow")
        return chancesnow;
    if (iconText == "chancetstorms")
        return chancestorms;
    if (iconText == "clear")
        return clear;
    if (iconText == "cloudy")
        return cloudy;
    if (iconText == "flurries")
        return flurries;
    if (iconText == "fog")
        return fog;
    if (iconText == "hazy")
        return hazy;
    if (iconText == "mostlycloudy")
        return mostlycloudy;
    if (iconText == "mostlysunny")
        return mostlysunny;
    if (iconText == "partlycloudy")
        return partlycloudy;
    if (iconText == "partlysunny")
        return partlysunny;
    if (iconText == "sleet")
        return sleet;
    if (iconText == "rain")
        return rain;
    if (iconText == "snow")
        return snow;
    if (iconText == "sunny")
        return sunny;
    if (iconText == "tstorms")
        return tstorms;

    return unknown;
}
const char *WeatherStationClass::getMiniMeteoconIconFromProgmem(String iconText)
{
    if (iconText == "chanceflurries")
        return minichanceflurries;
    if (iconText == "chancerain")
        return minichancerain;
    if (iconText == "chancesleet")
        return minichancesleet;
    if (iconText == "chancesnow")
        return minichancesnow;
    if (iconText == "chancetstorms")
        return minichancestorms;
    if (iconText == "clear")
        return miniclear;
    if (iconText == "cloudy")
        return minicloudy;
    if (iconText == "flurries")
        return miniflurries;
    if (iconText == "fog")
        return minifog;
    if (iconText == "hazy")
        return minihazy;
    if (iconText == "mostlycloudy")
        return minimostlycloudy;
    if (iconText == "mostlysunny")
        return minimostlysunny;
    if (iconText == "partlycloudy")
        return minipartlycloudy;
    if (iconText == "partlysunny")
        return minipartlysunny;
    if (iconText == "sleet")
        return minisleet;
    if (iconText == "rain")
        return minirain;
    if (iconText == "snow")
        return minisnow;
    if (iconText == "sunny")
        return minisunny;
    if (iconText == "tstorms")
        return minitstorms;

    return miniunknown;
}

const String WeatherStationClass::getShortText(String iconText)
{

    if (iconText == "chanceflurries")
        return "Chance of Flurries";
    if (iconText == "chancerain")
        return "Chance of Rain";
    if (iconText == "chancesleet")
        return "Chance of Sleet";
    if (iconText == "chancesnow")
        return "Chance of Snow";
    if (iconText == "chancetstorms")
        return "Chance of Storms";
    if (iconText == "clear")
        return "Clear";
    if (iconText == "cloudy")
        return "Cloudy";
    if (iconText == "flurries")
        return "Flurries";
    if (iconText == "fog")
        return "Fog";
    if (iconText == "hazy")
        return "Hazy";
    if (iconText == "mostlycloudy")
        return "Mostly Cloudy";
    if (iconText == "mostlysunny")
        return "Mostly Sunny";
    if (iconText == "partlycloudy")
        return "Partly Couldy";
    if (iconText == "partlysunny")
        return "Partly Sunny";
    if (iconText == "sleet")
        return "Sleet";
    if (iconText == "rain")
        return "Rain";
    if (iconText == "snow")
        return "Snow";
    if (iconText == "sunny")
        return "Sunny";
    if (iconText == "tstorms")
        return "Storms";

    return "-";
}

void WeatherStationClass::Run()
{
    GO.update();
    if (WiFi.isConnected())
    {
        if (GetParams(My_SD, "/WeatherParams.txt"))
        {
            updateData(true);
            lastDownloadUpdate = 0;

            while (!GO.BtnB.wasPressed())
            {
                GO.update();
                if (GO.JOY_Y.wasAxisPressed()==1)
                {
                    (screen > 0) ? screen-- : screen = 2;
                    drawn = false;
                }
                if (GO.JOY_Y.wasAxisPressed()==2)
                {
                    (screen < 2) ? screen++ : screen = 0;
                    drawn = false;
                }

                if ((screen == 0) && (millis() - lastDownloadUpdate >= 1000))
                {
                    drawTime();
                    lastDownloadUpdate = millis();
                }

                if (!drawn)
                {
                    switch (screen)
                    {
                    case 0:
                        GO.Lcd.fillScreen(BLACK);
                        dateDrawn = false;
                        drawCurrentWeather();
                        drawForecastDetail(8, 160, 2);
                        drawForecastDetail(70, 160, 4);
                        drawForecastDetail(135, 160, 6);
                        drawForecastDetail(200, 160, 8);
                        drawForecastDetail(260, 160, 10);
                        break;
                    case 1:
                        GO.Lcd.fillScreen(BLACK);
                        drawCurrentWeatherDetail();
                        break;
                    case 2:
                        GO.Lcd.fillScreen(BLACK);
                        drawForecastTable(1);
                        break;
                    default:
                        break;
                    }
                    drawn = true;
                }
            }
        }

        else
        {
            GO.Lcd.fillScreen(BLACK);
            GO.Lcd.setTextColor(WHITE);
            GO.Lcd.setFreeFont(FSS9);
            GO.Lcd.drawCentreString("WeatherParams.txt not found on SD!", 160, 60, 1);
            delay(3000);
            return;
        }
    }
    else
    {
        GO.Lcd.fillScreen(BLACK);
        GO.Lcd.setTextColor(WHITE);
        GO.Lcd.setFreeFont(FSS12);
        GO.Lcd.drawCentreString("Wifi Not Connected!", 160, 60, 1);
        delay(3000);
    }
}

WeatherStationClass::WeatherStationClass()
{
}

WeatherStationClass::~WeatherStationClass()
{
    GO.Lcd.fillScreen(0);
    GO.Lcd.setTextSize(1);
	GO.Lcd.setTextFont(1);
    GO.show();
}