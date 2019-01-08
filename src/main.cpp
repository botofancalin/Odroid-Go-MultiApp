#include "Wrappers.h"
#include "Resources.h"

unsigned long lastcheck = 0;
int SignalStrength = 0;

void setup()
{
	GO.begin(115200);
	preferences.begin("WiFi", false);
	GO.WiFi_Mode = preferences.getInt("mode", 0);
	preferences.end();
	WiFi.mode((wifi_mode_t)GO.WiFi_Mode);
	if (GO.WiFi_Mode > 0)
	{
		WiFi.begin();
	}

	preferences.begin("Brightness", false);
	GO.Lcd.setBrightness(preferences.getUShort("light", 95));
	preferences.end();

	//The main menu. Add main menu items here
	GO.addMenuItem(0, "APPLICATIONS", "<", "OK", ">", 1, Apps, appReturn);
	GO.addMenuItem(0, "SYSTEM", "<", "OK", ">", 2, System, appReturn);
	GO.addMenuItem(0, "ABOUT", "<", "OK", ">", -1, About, appAbout);

	GO.addMenuItem(1, "OSCILLOSCOPE", "<", "OK", ">", -1, Oscilloscope, appOscilloscope);
	GO.addMenuItem(1, "WEBRADIO", "<", "OK", ">", -1, WebRadio, appWebRadio);
	GO.addMenuItem(1, "WEATHER STATION", "<", "OK", ">", -1, WeatherStation, appWeatherStation);
	GO.addMenuItem(1, "WEBSERVER", "<", "OK", ">", -1, Webserver, appWebServer);
	GO.addMenuItem(1, "SD BROWSER", "<", "OK", ">", -1, Browser, appSdBrowser);
	//GO.addMenuItem(1, "TOOLS", "<", "OK", ">", -1, Tools, appListTools);
	GO.addMenuItem(1, "GAMES", "<", "OK", ">", -1, Games, appGamesList);

	GO.addMenuItem(2, "SYSTEM INFORMATIONS", "<", "OK", ">", -1, Sysinfo, appSysInfo);
	GO.addMenuItem(2, "WIFI CONNECTION", "<", "OK", ">", -1, WifiConn, appWiFiSetup);
	GO.addMenuItem(2, "DISPLAY BACKLIGHT", "<", "OK", ">", -1, Backlight, appCfgbrightness);

	GO.show();
}

void loop()
{
	unsigned long now = millis();
	if (now - lastcheck >= 1000)
	{
		GO.Lcd.setTextColor(WHITE, 15);
		GO.Lcd.drawString("Batt: " + String(GO.battery.getPercentage()) + " %", 10, 5, 2);
		GO.WiFi_Mode = WiFi.getMode();
		if (GO.WiFi_Mode == WIFI_MODE_STA && WiFi.isConnected())
		{
			SignalStrength = map(100 + WiFi.RSSI(), 5, 90, 0, 100);
			GO.Lcd.drawRightString("WiFi: " + String(SignalStrength) + " %", 310, 5, 2);
		}
		else if (GO.WiFi_Mode == WIFI_MODE_APSTA)
		{
			GO.Lcd.drawRightString("Clients: " + String(WiFi.softAPgetStationNum()), 300, 5, 2);
		}
		else if (GO.WiFi_Mode == WIFI_MODE_NULL)
		{
			GO.Lcd.drawRightString("Wifi OFF", 310, 5, 2);
		}
		lastcheck = now;
	}
	GO.update();
	if (GO.JOY_Y.wasAxisPressed() == 1)
	{
		GO.up();
	}
	if (GO.JOY_Y.wasAxisPressed() == 2)
	{
		GO.down();
	}
	if (GO.BtnA.wasPressed())
	{
		GO.execute();
	}
	if (GO.BtnB.wasPressed())
	{
		GO.GoToLevel(0);
	}
}