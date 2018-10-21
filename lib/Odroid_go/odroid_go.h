#ifndef _ODROID_GO_H_
#define _ODROID_GO_H_

#if defined(ESP32)

#define MENU_TITLE_MAX_SIZE 24
#define BTN_TITLE_MAX_SIZE 6
#define MAX_SUBMENUS 3

#define LIST_MAX_LABEL_SIZE 32
#define LIST_PAGE_LABELS 6

#define FSS9 &FreeSans9pt7b
#define FSS12 &FreeSans12pt7b
#define FSS18 &FreeSans18pt7b
#define FSS24 &FreeSans24pt7b
#define FFS9B &FreeSansBold9pt7b

#include <Arduino.h>
#include <vector>
#include <SPI.h>
#include <WiFi.h>
#include "SPIFFS.h"
#include "SD.h"
#include "utility/Config.h"
#include "utility/Display.h"
#include "utility/Button.h"
#include "utility/battery.h"

#include "AudioFileSourceSD.h"
#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

#define My_SD SD

#include "Preferences.h"

extern "C"
{
#include "esp_sleep.h"
}

class ODROID_GO
{
  public:
    ODROID_GO();
    ~ODROID_GO();

    void begin(unsigned long baud = 115200);
    void update();
    void up();
    void down();
    void execute();
    void windowClr();
    void setColorSchema(unsigned int inmenucolor, unsigned int inwindowcolor, unsigned int intextcolor);
    void drawAppMenu(String inmenuttl, String inbtnAttl, String inbtnBttl, String inbtnCttl);
    void GoToLevel(uint32_t inlevel);
    unsigned int getrgb(uint8_t inred, uint8_t ingrn, uint8_t inblue);
    void addMenuItem(uint32_t levelID, const char *menu_title, const char *btnA_title, const char *btnB_title, const char *btnC_title,
                     signed char goto_level, const char *Menu_Img, void (*function)());
    void show();
    void showList();
    void clearList();
    unsigned int getListID();
    String getListString();
    void nextList();
    void previousList();
    void addList(String inLabel);
    void setListCaption(String inCaption);
    void btnRestore();
    String lastBtnTittle[3];

#define DEBOUNCE_MS 20
    Button JOY_Y = Button(BUTTON_JOY_Y, true, DEBOUNCE_MS);
    Button JOY_X = Button(BUTTON_JOY_X, true, DEBOUNCE_MS);
    Button BtnA = Button(BUTTON_A_PIN, true, DEBOUNCE_MS);
    Button BtnB = Button(BUTTON_B_PIN, true, DEBOUNCE_MS);
    Button BtnMenu = Button(BUTTON_MENU, true, DEBOUNCE_MS);
    Button BtnVolume = Button(BUTTON_VOLUME, true, DEBOUNCE_MS);
    Button BtnSelect = Button(BUTTON_SELECT, true, DEBOUNCE_MS);
    Button BtnStart = Button(BUTTON_START, true, DEBOUNCE_MS);

    int WiFi_Mode;
    int vol, old_vol;

    // LCD
    ILI9341 Lcd = ILI9341();
    Battery battery;

  private:
    uint8_t _wakeupPin;
    String listCaption;
    void drawListItem(uint32_t inIDX, uint32_t postIDX);
    void drawMenu(String inmenuttl, String inbtnAttl, String inbtnBttl, String inbtnCttl, unsigned int inmenucolor,
                  unsigned int inwindowcolor, const char *iMenuImg, unsigned int intxtcolor);
    struct MenuCommandCallback
    {
        char title[MENU_TITLE_MAX_SIZE + 1];
        char btnAtitle[BTN_TITLE_MAX_SIZE + 1];
        char btnBtitle[BTN_TITLE_MAX_SIZE + 1];
        char btnCtitle[BTN_TITLE_MAX_SIZE + 1];
        signed char gotoLevel;
        const char *MenuImg;
        void (*function)();
    };
    std::vector<String> list_labels;
    uint32_t list_lastpagelines;
    uint32_t list_count;
    uint32_t list_pages;
    uint32_t list_page;
    unsigned int list_idx;
    uint32_t list_lines;

    MenuCommandCallback *menuList[MAX_SUBMENUS];
    uint32_t menuIDX;
    uint32_t levelIDX;
    uint32_t menuCount[MAX_SUBMENUS];
    unsigned int menucolor;
    unsigned int windowcolor;
    unsigned int menutextcolor;
};

extern Preferences preferences;
extern ODROID_GO GO;
#define go GO

#else
#error “This library only supports boards with ESP32 processor.”
#endif

#endif
