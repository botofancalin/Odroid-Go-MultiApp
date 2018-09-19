#include "SdBrowser.h"

void SdBrowserClass::listDir(fs::FS &fs, String dirName, int levels)
{
    File root = fs.open(dirName);
    if (!root)
    {
        return;
    }
    if (!root.isDirectory())
    {
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (!strstr((file.name()), ignored))
        {
            if (file.isDirectory())
            {
                if (levels)
                {
                    listDir(fs, file.name(), levels - 1);
                }
            }
            else
            {
                fileVector.push_back(fileinfo);
                String fileName = file.name();
                fileVector[appsCount].fileName = fileName;
                fileVector[appsCount].fileSize = file.size();
                appsCount++;
            }
        }
        file = root.openNextFile();
    }
    file.close();
}

void SdBrowserClass::aSortFiles()
{
    bool swapped;
    String name1, name2;
    do
    {
        swapped = false;
        for (int i = 0; i < appsCount - 1; i++)
        {
            name1 = String(fileVector[i].fileName[0]);
            name2 = String(fileVector[i + 1].fileName[0]);
            if (name1 == name2)
            {
                name1 = String(fileVector[i].fileName[1]);
                name2 = String(fileVector[i + 1].fileName[1]);
                if (name1 == name2)
                {
                    name1 = String(fileVector[i].fileName[2]);
                    name2 = String(fileVector[i + 1].fileName[2]);
                }
            }

            if (name1 > name2)
            {
                fileinfo = fileVector[i];
                fileVector[i] = fileVector[i + 1];
                fileVector[i + 1] = fileinfo;
                swapped = true;
            }
        }
    } while (swapped);
}

void SdBrowserClass::buildMyMenu()
{
    GO.clearList();
    GO.setListCaption("Files");
    for (int i = 0; i < appsCount; i++)
    {
        String shortName = fileVector[i].fileName.substring(1);
        GO.addList(shortName);
    }
}

void SdBrowserClass::Run()
{
    appsCount = 0;
    listDir(My_SD, "/", levels);
    aSortFiles();
    buildMyMenu();
    GO.showList();
    while (!GO.BtnB.wasPressed())
    {
        if (GO.JOY_Y.wasAxisPressed()==1)
        {
            if (repaint)
            {
                GO.Lcd.clear();
                GO.drawAppMenu(F("SD BROWSER"), F("EXIT"), F("OPEN"), F(">"));
                GO.btnRestore();
            }
            repaint = false;
            GO.nextList();
        }
        if (GO.JOY_Y.wasAxisPressed()==2)
        {
            if (repaint)
            {
                GO.Lcd.clear();
                GO.drawAppMenu(F("SD BROWSER"), F("EXIT"), F("OPEN"), F(">"));
                GO.btnRestore();
            }
            repaint = false;
            GO.previousList();
        }
        int MenuID = GO.getListID();

        if (GO.BtnA.wasPressed())
        {
            String FileName = fileVector[MenuID].fileName;
            if (FileName.endsWith(".jpg"))
            {
                GO.Lcd.clear();
                if (fileVector[MenuID].fileSize > 100000UL)
                {
                    GO.Lcd.drawJpgFile(My_SD, FileName.c_str(), 0, 0, 0, 0, 0, 0, JPEG_DIV_8);
                }
                else if (fileVector[MenuID].fileSize > 70000UL)
                {
                    GO.Lcd.drawJpgFile(My_SD, FileName.c_str(), 0, 0, 0, 0, 0, 0, JPEG_DIV_4);
                }
                else if (fileVector[MenuID].fileSize > 50000)
                {
                    GO.Lcd.drawJpgFile(My_SD, FileName.c_str(), 0, 0, 0, 0, 0, 0, JPEG_DIV_2);
                }
                else
                {
                    GO.Lcd.drawJpgFile(My_SD, FileName.c_str());
                }
                repaint = true;
                GO.btnRestore();
            }
            else if (FileName.endsWith(".mp3"))
            {
                Mp3PlayerClass Mp3PlayerObj;
                Mp3PlayerObj.Play(&FileName);
            }
            else if (FileName.endsWith(".mov"))
            {
                VideoPlayerClass VideoPlayerObj;
                VideoPlayerObj.Play(FileName.c_str());
            }
            else if (!inmenu)
            {
                inmenu = true;
                GO.windowClr();
                GO.Lcd.drawCentreString("File Name: " + fileVector[MenuID].fileName, GO.Lcd.width() / 2, (GO.Lcd.height() / 2) - 10, 2);
                GO.Lcd.drawCentreString("File Size: " + String(fileVector[MenuID].fileSize), GO.Lcd.width() / 2, (GO.Lcd.height() / 2) + 10, 2);
            }
            else
            {
                inmenu = false;
                GO.showList();
            }
        }
        GO.update();
    }
}

SdBrowserClass::SdBrowserClass()
{
    GO.update();
    GO.drawAppMenu(F("SD BROWSER"), F("EXIT"), F("OPEN"), F(">"));
}

SdBrowserClass::~SdBrowserClass()
{
    fileVector.clear();
    fileVector.shrink_to_fit();
    GO.clearList();
    GO.show();
}