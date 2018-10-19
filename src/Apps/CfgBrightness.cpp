#include "CfgBrightness.h"

void CfgBrightnessClass::Run()
{
    preferences.begin("Brightness", false);
    tmp_bright = preferences.getUShort("light", 95);
    tmp_lbright = 0;

    while (!GO.BtnB.wasPressed())
    {
        if ((GO.JOY_X.wasAxisPressed() == 2) && tmp_bright > 16)
        {
            tmp_bright = tmp_bright - 16;
        }
        if ((GO.JOY_X.wasAxisPressed() == 1) && tmp_bright < 255)
        {
            tmp_bright = tmp_bright + 16;
        }
        if (tmp_lbright != tmp_bright)
        {
            tmp_lbright = tmp_bright;
            preferences.putUShort("light", tmp_lbright);
            GO.Lcd.setBrightness(tmp_lbright);
            GO.windowClr();
            val = map(tmp_lbright, 16, 255, 0, 100);
            GO.Lcd.HprogressBar(40, 110, 240, 30, YELLOW, val, true);
        }
        GO.update();
    }
    preferences.end();
}

CfgBrightnessClass::CfgBrightnessClass()
{
    GO.drawAppMenu(F("DISPLAY BRIGHTNESS"), F("-"), F("OK"), F("+"));
    GO.update();
}

CfgBrightnessClass::~CfgBrightnessClass()
{
    GO.show();
}
