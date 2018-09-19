#include "VideoPlayer.h"

void VideoPlayerClass::Play(const char *fileName)
{
    GO.update();
    GO.Lcd.fillScreen(BLACK);
    file = My_SD.open(fileName);
    GO.Lcd.setSwapBytes(true);
    while(!GO.BtnB.wasPressed() && file.read(videoBuffer, 115200))
    {
        GO.Lcd.pushImage(0,30,320,180,(uint16_t*)videoBuffer);
        GO.update();
    }
    file.close();
    return;
}

VideoPlayerClass::VideoPlayerClass()
{
    videoBuffer = (uint8_t*)heap_caps_malloc(115200,MALLOC_CAP_DEFAULT);
}

VideoPlayerClass::~VideoPlayerClass()
{
    free(videoBuffer);
    GO.drawAppMenu(F("SD BROWSER"), F("EXIT"), F("OPEN"), F(">"));
    GO.showList();
}