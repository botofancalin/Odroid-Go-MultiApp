#include "Wrappers.h"
#include "Resources.h"

void setup()
{
    // put your setup code here, to run once:
    GO.begin();
    preferences.begin("WiFi", false);
	GO.WiFi_Mode = preferences.getInt("mode", 0);
	preferences.end();
	WiFi.mode(wifi_mode_t(GO.WiFi_Mode));
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
	GO.addMenuItem(0, "SLEEP/CHARGING", "<", "OK", ">", -1, Sleep, appSleep);

	//GO.addMenuItem(1, "OSCILLOSCOPE", "<", "OK", ">", -1, Oscilloscope, appOscilloscope);
	//GO.addMenuItem(1, "WEBRADIO", "<", "OK", ">", -1, WebRadio, appWebRadio);
	//GO.addMenuItem(1, "WEATHER STATION", "<", "OK", ">", -1, WeatherStation, appWeatherStation);
	//GO.addMenuItem(1, "WEBSERVER", "<", "OK", ">", -1, Webserver, appWebServer);
	//GO.addMenuItem(1, "SD BROWSER", "<", "OK", ">", -1, Browser, appSdBrowser);
	//GO.addMenuItem(1, "TOOLS", "<", "OK", ">", -1, Tools, appListTools);
	//GO.addMenuItem(1, "GAMES", "<", "OK", ">", -1, Games, appGamesList);
	GO.addMenuItem(1, "RETURN", "<", "OK", ">", 0, Return, appReturn);

	GO.addMenuItem(2, "SYSTEM INFORMATIONS", "<", "OK", ">", -1, Sysinfo, appSysInfo);
	//GO.addMenuItem(2, "WIFI CONNECTION", "<", "OK", ">", -1, WifiConn, appWiFiSetup);
	GO.addMenuItem(2, "DISPLAY BACKLIGHT", "<", "OK", ">", -1, Backlight, appCfgbrightness);
	GO.addMenuItem(2, "RETURN", "<", "OK", ">", 0, Return, appReturn);

	GO.show();
}

void displayButtons()
{
    GO.Lcd.clear();
    GO.Lcd.setCursor(0, 0);

    GO.Lcd.println("/* Direction pad */");
    GO.Lcd.printf("Joy-Y-Up: %s \n", (GO.JOY_Y.isAxisPressed() == 2) ? "Pressed" : " ");
    GO.Lcd.printf("Joy-Y-Down: %s \n", (GO.JOY_Y.isAxisPressed() == 1) ? "Pressed" : " ");
    GO.Lcd.printf("Joy-X-Left: %s \n", (GO.JOY_X.isAxisPressed() == 2) ? "Pressed" : " ");
    GO.Lcd.printf("Joy-X-Right: %s \n", (GO.JOY_X.isAxisPressed() == 1) ? "Pressed" : " ");
    GO.Lcd.println("");
    GO.Lcd.println("/* Function key */");
    GO.Lcd.printf("Menu: %s \n", (GO.BtnMenu.isPressed() == 1) ? "Pressed" : " ");
    GO.Lcd.printf("Volume: %s \n", (GO.BtnVolume.isPressed() == 1) ? "Pressed" : " ");
    GO.Lcd.printf("Select: %s \n", (GO.BtnSelect.isPressed() == 1) ? "Pressed" : " ");
    GO.Lcd.printf("Start: %s \n", (GO.BtnStart.isPressed() == 1) ? "Pressed" : " ");
    GO.Lcd.println("");
    GO.Lcd.println("/* Actions */");
    GO.Lcd.printf("B: %s \n", (GO.BtnB.isPressed() == 1) ? "Pressed" : " ");
    GO.Lcd.printf("A: %s \n", (GO.BtnA.isPressed() == 1) ? "Pressed" : " ");
}

void loop()
{
    // put your main code here, to run repeatedly:
    GO.update();
	if (GO.JOY_Y.wasAxisPressed() == 2)
	{
		GO.up();
	}
	if (GO.JOY_Y.wasAxisPressed() == 1)
	{
		GO.down();
	}
	if (GO.BtnA.wasPressed() == 1)
	{
		GO.execute();
	}
}