#include "About.h"

void AboutClass::Run()
{
    GO.windowClr();
    GO.Lcd.drawCentreString(F("Made by"), GO.Lcd.width() / 2, (GO.Lcd.height() / 2) - 50, 4);
    GO.Lcd.drawCentreString(F("Botofan Calin"), GO.Lcd.width() / 2, (GO.Lcd.height() / 2) - 15, 4);
    GO.Lcd.drawCentreString(F("botofancalin@gmail.com"), GO.Lcd.width() / 2, (GO.Lcd.height() / 2) + 20, 4);
    while (!GO.BtnB.wasPressed())
    {
        GO.update();
    }
}

AboutClass::AboutClass()
{
    GO.drawAppMenu(F("ABOUT"), F(""), F("ESC"), F(""));
    GO.update();
}

AboutClass::~AboutClass()
{
    GO.show();
}