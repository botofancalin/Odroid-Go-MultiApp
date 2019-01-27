#include "WiFiSettings.h"

void WifiSettingsClass::AP_Mode()
{
    WiFi.disconnect();
    vTaskDelay(200 / portTICK_PERIOD_MS);
    WiFi.mode(WIFI_MODE_AP);
    GO.WiFi_Mode = WIFI_MODE_AP;
    WiFi.softAP("OrdoidGO");
    GO.Lcd.drawString("AP Mode Started", 5, 50, 2);
    GO.Lcd.drawString("Host Name: OdroidGo", 5, 70, 2);
    GO.Lcd.drawString("IP Address: " + WiFi.softAPIP().toString(), 5, 90, 2);
}

void WifiSettingsClass::STA_Mode()
{
    WiFi.disconnect();
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin();
    GO.WiFi_Mode = WIFI_MODE_STA;
    GO.Lcd.drawString("STA Mode Started", 5, 50, 2);
    GO.Lcd.drawString("Will Connect to stored SSID", 5, 70, 2);
}

void WifiSettingsClass::APSTA_Mode()
{
    WiFi.disconnect();
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.begin();
    GO.WiFi_Mode = WIFI_MODE_APSTA;
    GO.Lcd.drawString("AP + STA Mode Started", 5, 50, 2);
    GO.Lcd.drawString("Will use the stored SSID", 5, 70, 2);
}

void WifiSettingsClass::SmartConfig()
{
    int i = 0;
    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();
    GO.Lcd.drawString("Waiting for SmartConfig", 5, 30, 2);
    while (!WiFi.smartConfigDone())
    {
        GO.Lcd.setTextColor(WHITE);
        GO.Lcd.drawNumber(i, 5, 50, 2);
        delay(500);
        GO.Lcd.setTextColor(BLACK);
        GO.Lcd.drawNumber(i, 5, 50, 2);
        if (i == 119)
        {
            GO.Lcd.setTextColor(WHITE);
            GO.Lcd.drawString("SmartConfig NOT received!", 5, 70, 2);
            STA_Mode();
            return;
        }
        i++;
    }
    GO.Lcd.setTextColor(WHITE);
    GO.Lcd.drawString("SmartConfig received", 5, 70, 2);
    GO.Lcd.drawString("Waiting for WiFi", 5, 90, 2);
    i = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        GO.Lcd.setTextColor(WHITE);
        GO.Lcd.drawNumber(i, 5, 110, 2);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        GO.Lcd.setTextColor(BLACK);
        GO.Lcd.drawNumber(i, 5, 110, 2);
        if (i == 59)
        {
            STA_Mode();
            GO.Lcd.setTextColor(WHITE);
            GO.Lcd.drawString("Wifi Not Found!", 5, 130, 2);
            return;
        }
        i++;
    }
    GO.Lcd.setTextColor(WHITE);
    GO.Lcd.drawString("WiFi Connected", 5, 130, 2);
    GO.Lcd.drawString("IP: " + WiFi.localIP().toString(), 5, 150, 2);
    GO.WiFi_Mode = WIFI_MODE_STA;
}

void WifiSettingsClass::Run()
{
    GO.clearList();
    GO.setListCaption("WiFi");
    GO.addList("WiFi SmartConfig");
    GO.addList("Connect by WPS Button");
    GO.addList("Connect by WPS Pin Code");
    GO.addList("WiFi STA");
    GO.addList("WiFi AP");
    GO.addList("WiFi OFF");
    GO.showList();

    while (!GO.BtnB.wasPressed())
    {
        if (GO.JOY_Y.wasAxisPressed() == 1)
        {
            GO.nextList();
        }
        if (GO.BtnA.wasPressed())
        {
            if (GO.getListString() == "WiFi STA")
            {
                GO.windowClr();
                STA_Mode();
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                GO.windowClr();
                GO.drawAppMenu(F("WiFi"), F("ESC"), F("SELECT"), F("LIST"));
                GO.showList();
            }
            if (GO.getListString() == "WiFi SmartConfig")
            {
                GO.windowClr();
                SmartConfig();
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                GO.windowClr();
                GO.drawAppMenu(F("WiFi"), F("ESC"), F("SELECT"), F("LIST"));
                GO.showList();
            }
            if (GO.getListString() == "WiFi AP")
            {
                GO.windowClr();
                AP_Mode();
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                GO.windowClr();
                GO.drawAppMenu(F("WiFi"), F("ESC"), F("SELECT"), F("LIST"));
                GO.showList();
            }
            if (GO.getListString() == "Connect by WPS Button")
            {
                GO.windowClr();
                vTaskDelay(200 / portTICK_PERIOD_MS);
                Wps_run(true);
                GO.WiFi_Mode = WIFI_MODE_STA;
            }
            if (GO.getListString() == "Connect by WPS Pin Code")
            {
                GO.windowClr();
                vTaskDelay(200 / portTICK_PERIOD_MS);
                Wps_run(false);
                GO.WiFi_Mode = WIFI_MODE_STA;
            }
            if (GO.getListString() == "WiFi OFF")
            {
                GO.windowClr();
                vTaskDelay(200 / portTICK_PERIOD_MS);
                WiFi.disconnect();
                WiFi.mode(WIFI_MODE_NULL);
                GO.WiFi_Mode = WIFI_MODE_NULL;
                GO.Lcd.drawString("WiFi Turned OFF", 5, 50, 2);
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                GO.drawAppMenu(F("WiFi"), F("ESC"), F("SELECT"), F("LIST"));
                GO.showList();
            }
        }
        GO.update();
    }
    preferences.begin("WiFi", false);
    preferences.putInt("mode", (int)WiFi.getMode());
    preferences.end();
}

WifiSettingsClass::WifiSettingsClass()
{
    GO.drawAppMenu(F("WiFi"), F("ESC"), F("SELECT"), F("LIST"));
    GO.update();
}

WifiSettingsClass::~WifiSettingsClass()
{
    GO.show();
}