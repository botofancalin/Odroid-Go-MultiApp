#include "Sysinfo.h"

String SysinfoClass::getWiFiMac()
{
    esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
    sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
    return String(baseMacChr);
}

void SysinfoClass::page_0()
{
    GO.Lcd.drawString(F("CPU FREQ:"), 10, 40, 2);
    GO.Lcd.drawString(String(ESP.getCpuFreqMHz()) + " Mhz", 120, 40, 2);

    GO.Lcd.drawString(F("CPU CORES:"), 10, 60, 2);
    GO.Lcd.drawNumber(2, 120, 60, 2);

    GO.Lcd.drawString(F("CHIP REV:"), 10, 80, 2);
    GO.Lcd.drawNumber(ESP.getChipRevision(), 120, 80, 2);

    GO.Lcd.drawString(F("FLASH SPEED:"), 10, 100, 2);
    GO.Lcd.drawString(String(ESP.getFlashChipSpeed() / 1000000) + " Mhz", 120, 100, 2);

    GO.Lcd.drawString(F("FREE RAM:"), 10, 120, 2);
    GO.Lcd.drawString(String(ESP.getFreeHeap()) + " Bytes", 120, 120, 2);

    GO.Lcd.drawString(F("WIFI STA MAC:"), 10, 140, 2);
    GO.Lcd.drawString(getWiFiMac(), 120, 140, 2);

    GO.Lcd.drawString(F("IP ADDRESS:"), 10, 160, 2);
    GO.Lcd.drawString((WiFi.localIP().toString()), 120, 160, 2);

    GO.Lcd.drawString(F("WIFI MODE:"), 10, 180, 2);
    GO.Lcd.drawString(wifi_m_mode[WiFi.getMode()], 120, 180, 2);
}

void SysinfoClass::page_1()
{
    type = My_SD.cardType();

    GO.Lcd.drawString(F("MIN FREE HEAP:"), 10, 40, 2);
    GO.Lcd.drawString(String(esp_get_minimum_free_heap_size()) + " Bytes", 120, 40, 2);

    GO.Lcd.drawString(F("FLASH SIZE:"), 10, 60, 2);
    GO.Lcd.drawString(String(ESP.getFlashChipSize() / 1048576) + " Mb", 120, 60, 2);

    GO.Lcd.drawString(F("SPIFFS SIZE:"), 10, 80, 2);
    GO.Lcd.drawString(String(SPIFFS.totalBytes()) + " Bytes", 120, 80, 2);

    GO.Lcd.drawString(F("SPIFFS USED:"), 10, 100, 2);
    GO.Lcd.drawString(String(SPIFFS.usedBytes()) + " Bytes", 120, 100, 2);

    GO.Lcd.drawString(F("SD CARD TYPE:"), 10, 120, 2);
    GO.Lcd.drawString(SD_Type[type], 120, 120, 2);

    if (type != 0)
    {
        sdSize = My_SD.cardSize();
        sdUsed = My_SD.usedBytes();
        GO.Lcd.drawString(F("SD CARD SIZE:"), 10, 140, 2);
        GO.Lcd.drawString(String((int)(sdSize / 1048576)) + " Mb", 120, 140, 2);
        //GO.Lcd.drawNumber(sdSize, 120, 140, 2);
        GO.Lcd.drawString(F("SD BYTES USED:"), 10, 160, 2);
        GO.Lcd.drawString(String((int)(sdUsed / 1048576)) + " Mb", 120, 160, 2);
    }
}

void SysinfoClass::drawpage(int page)
{
    switch (page)
    {
    case 0:
        page_0();
        break;
    case 1:
        page_1();
        break;
    }
}

void SysinfoClass::Run()
{
    while (!GO.BtnB.wasPressed())
    {
        if (GO.JOY_Y.wasAxisPressed() == 1)
        {
            if (page < PAGEMAX)
            {
                page++;
            }
            else
            {
                page = 0;
            }
            done = false;
        }

        if (GO.JOY_Y.wasAxisPressed() == 2)
        {
            if (page > 0)
            {
                page--;
            }
            else
            {
                page = PAGEMAX;
            }
            done = false;
        }

        if (!done)
        {
            GO.windowClr();
            drawpage(page);
            done = true;
        }
        GO.update();
    }
    done = false;
}

SysinfoClass::SysinfoClass()
{
    GO.update();
    GO.drawAppMenu(F("M5 SYSTEM INFO"), F("<"), F("ESC"), F(">"));
}

SysinfoClass::~SysinfoClass()
{
    GO.show();
}