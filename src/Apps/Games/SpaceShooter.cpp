//======================== intro =======================================
//      Space Shooter, basically knock-off Space Invaders
//             && also maybe a bit of Galaga
//   Written by Tyler Edwards for the Hackerbox #0020 badge
//  But should work on any ESP32 && Adafruit ILI9341 display
//        I am sorry for the messy code, you'll just
//                  have to live with it
//      Tyler on GitHub: https://github.com/HailTheBDFL/
//          Hackerboxes: http://hackerboxes.com/
//=========================== setup ===================================
// Space Shooter with M5STACK : 2018.01.12 Transplant by macsbug
// Controller   : Buttons A = LEFT, B = RIGHT, C = START || SHOOTING
// Github:https://macsbug.wordpress.com/2018/01/12/esp32-spaceshooter-with-m5stack/
//===================================================================
#include "SpaceShooter.h"

//=============================== setup && loop ==================

void SpaceShooterClass::initVars()
{
  //============================= game variables =========================
  offsetM = 0;
  offsetT = 0;
  offsetF = 0;
  offsetB = 0;
  offsetA = 0;
  offsetAF = 0;
  offsetAB = 0;
  offsetS = 0;
  threshold = 40;
  startPrinted = false;
  beginGame = false;
  beginGame2 = true;
  play = false;
  alien_shooter_score = 0;
  alien_shooter_scoreInc = 10;
  level = 1;
  //---------------------Player---------------------------------------
  shipX = 147;
  shipY = 190;
  oldShipX = 0;
  oldShipY = 0;
  changeShipX = 0;
  changeShipY = 0;
  shipSpeed = 50;
  doSplode = false;
  fire = false;
  //--------------------------Aliens----------------------------------
  alienLiveCount = 18;
  alienX = 7;
  alienY = 25;
  oldAlienX = 7;
  oldAlienY = 25;
  changeAlienX = 6;
  changeAlienY = 0;
  alienSpeed = 200;
  oldAlienSpeed = 0;
  chanceOfFire = 2;
}
//==================================================================

void SpaceShooterClass::drawBitmap(char img[], int imgW, int imgH, int x, int y, int scale)
{
  uint16_t cellColor = 0;
  char curPix;
  for (int i = 0; i < imgW * imgH; i++)
  {
    curPix = img[i];
    if (curPix == 'W')
    {
      cellColor = WHITE;
    }
    else if (curPix == 'Y')
    {
      cellColor = YELLOW;
    }
    else if (curPix == 'B')
    {
      cellColor = BLUE;
    }
    else if (curPix == 'R')
    {
      cellColor = RED;
    }
    else if (curPix == 'G')
    {
      cellColor = 0x5E85;
    }
    if (curPix != 'Z' && scale == 1)
    {
      GO.Lcd.drawPixel(x + i % imgW, y + i / imgW, cellColor);
    }
    else if (curPix != 'Z' && scale > 1)
    {
      GO.Lcd.fillRect(x + scale * (i % imgW), y + scale * (i / imgW), scale, scale, cellColor);
    }
  }
}
//==================================================================
void SpaceShooterClass::drawalien_shooter_score(bool win)
{
  GO.Lcd.setTextColor(WHITE);
  GO.Lcd.setTextSize(2);
  if (win)
  {
    GO.Lcd.drawString("LEVEL UP!", 53, 40);
  }
  else
  {
    GO.Lcd.drawString("GAME OVER", 53, 40);
  }
  for (; millis() - offsetM <= 1000;)
    GO.Lcd.drawString("Score: " + String(alien_shooter_score), 80, 89);
  offsetM = millis();
  for (; millis() - offsetM <= 1000;)
  {
  }
  GO.Lcd.drawString("Level: " + String(level), 80, 128);
}
// functions =======================================================
void SpaceShooterClass::gameOver()
{
  play = false;
  if (doSplode)
  {
    drawBitmap(splodedImg, splodedImgW, splodedImgH, shipX, shipY, 2);
  }
  GO.Lcd.fillScreen(BLACK);
  drawalien_shooter_score(false);
  delay(1000);
  GO.Lcd.drawString("Start", 132, 221);
  GO.Lcd.drawString("Exit", 232, 221);
  while (1)
  {
    // wait for push button
    if (GO.BtnA.wasPressed())
    {
      break;
    }
    if (GO.BtnB.wasPressed())
    {
      return;
    }
    GO.update();
  }
}
//==================================================================
void SpaceShooterClass::levelUp()
{
  play = false;
  memset(alienLive, true, 18);
  memset(aFireX, 0, 5);
  memset(aFireY, 0, 5);
  memset(aFireAge, 0, 5);
  alienX = 7;
  alienY = 25;
  oldAlienX = 7;
  oldAlienY = 25;
  alienSpeed = oldAlienSpeed;
  if (alienSpeed > 100)
  {
    alienSpeed -= 10;
    chanceOfFire -= 10;
  }
  else if (alienSpeed > 50)
  {
    alienSpeed -= 10;
    chanceOfFire -= 5;
  }
  else if (alienSpeed > 25)
  {
    alienSpeed -= 5;
    chanceOfFire -= 1;
  }
  alien_shooter_score += 50;
  alien_shooter_scoreInc += 5;
  changeShipX = 0;
  changeShipY = 0;
  for (unsigned long i = millis(); millis() - i <= 1600;)
  {
    if (millis() - offsetM >= 20)
    {
      GO.Lcd.fillRect(oldShipX, oldShipY, 28, 44, BLACK);
      drawBitmap(shipImg, shipImgW, shipImgH, shipX, shipY, 2);
      drawBitmap(flamesImg, flamesImgW, flamesImgH, shipX + 1,
                 shipY + 32, 2);
      oldShipX = shipX;
      oldShipY = shipY;
      shipY -= 6;
      offsetM = millis();
    }
  }
  GO.Lcd.fillRect(oldShipX, 0, 28, 44, BLACK);
  drawalien_shooter_score(true);
  level += 1;
  shipX = 147;
  shipY = 190;
  for (; millis() - offsetM <= 4000;)
  {
  }
  GO.Lcd.fillScreen(BLACK);
  offsetM = millis();
  play = true;
}
//==================================================================
int SpaceShooterClass::findAlienX(int num) { return alienX + 42 * (num % 6); }
//==================================================================
int SpaceShooterClass::findAlienY(int num) { return alienY + 33 * (num / 6); }
//==================================================================
int SpaceShooterClass::findOldAlienX(int num) { return oldAlienX + 42 * (num % 6); }
//==================================================================
int SpaceShooterClass::findOldAlienY(int num) { return oldAlienY + 33 * (num / 6); }
//==================================================================
bool SpaceShooterClass::alienShot(int num)
{
  for (int i = 0; i < 5; i++)
  {
    if (fFireAge[i] < 20 && fFireAge[i] > 0)
    {
      if (fFireX[i] > findAlienX(num) - 4 && fFireX[i] < findAlienX(num) + 28 && fFireY[i] < findAlienY(num) + 22 && fFireY[i] > findAlienY(num) + 4)
      {
        fFireAge[i] = 20;
        return true;
      }
    }
  }
  return false;
}
//==================================================================
bool SpaceShooterClass::onPlayer(int num)
{
  if (findAlienX(num) - shipX < 24 && findAlienX(num) - shipX > -28 && findAlienY(num) - shipY < 32 &&
      findAlienY(num) - shipY > -22)
  {
    doSplode = true;
    return true;
  }
  else
  {
    return false;
  }
}
//==================================================================
bool SpaceShooterClass::exceedBoundary(int num)
{
  if (findAlienY(num) > 218)
  {
    return true;
  }
  else
  {
    return false;
  }
}
//==================================================================
void SpaceShooterClass::moveAliens()
{
  for (int i = 0; i < 18; i++)
  {
    if (alienLive[i])
    {
      GO.Lcd.fillRect(findOldAlienX(i), findOldAlienY(i), 28, 22, BLACK);
      drawBitmap(alienImg, alienImgW, alienImgH, findAlienX(i),
                 findAlienY(i), 2);
    }
  }
  oldAlienX = alienX;
  oldAlienY = alienY;
  alienX += changeAlienX;
  alienY += changeAlienY;
  if (changeAlienY != 0)
  {
    changeAlienY = 0;
  }
}
//---------------------------Player---------------------------------
void SpaceShooterClass::fireDaLazer()
{
  int bulletNo = -1;
  for (int i = 0; i < 4; i++)
  {
    if (fFireAge[i] == 0)
    {
      bulletNo = i;
    }
  }
  if (bulletNo != -1)
  {
    fFireAge[bulletNo] = 1;
    fFireX[bulletNo] = shipX + 13;
    fFireY[bulletNo] = shipY - 4;
    GO.Lcd.fillRect(fFireX[bulletNo], fFireY[bulletNo], 4, 3, MAGENTA);
  }
  fire = false;
}
//==================================================================
void SpaceShooterClass::keepFirinDaLazer(int bulletNo)
{
  GO.Lcd.fillRect(fFireX[bulletNo], fFireY[bulletNo], 4, 4, BLACK);
  fFireY[bulletNo] -= 8;
  GO.Lcd.fillRect(fFireX[bulletNo], fFireY[bulletNo], 4, 4, MAGENTA);
  fFireAge[bulletNo] += 1;
}
//==================================================================
void SpaceShooterClass::stopFirinDaLazer(int bulletNo)
{
  GO.Lcd.fillRect(fFireX[bulletNo], fFireY[bulletNo], 4, 4, BLACK);
  fFireAge[bulletNo] = 0;
}
//==================================================================
void SpaceShooterClass::moveShip()
{
  if (shipX + changeShipX < 288 && shipX + changeShipX > 6 && changeShipX != 0)
  {
    shipX += changeShipX;
  }
  if (shipY + changeShipY > 24 && shipY + changeShipY < 192 && changeShipY != 0)
  {
    shipY += changeShipY;
  }
  if (oldShipX != shipX || oldShipY != shipY)
  {
    GO.Lcd.fillRect(oldShipX, oldShipY, 28, 44, BLACK);
    oldShipX = shipX;
    oldShipY = shipY;
    drawBitmap(shipImg, shipImgW, shipImgH, shipX, shipY, 2);
  }
}
//=========================== button functions =====================
void SpaceShooterClass::up()
{
  if (millis() - offsetT >= 50 && play)
  {
    changeShipX = 0;
    changeShipY = -6;
    offsetT = millis();
  }
}
//==================================================================
void SpaceShooterClass::down()
{
  if (millis() - offsetT >= 50 && play)
  {
    changeShipX = 0;
    changeShipY = 6;
    offsetT = millis();
  }
}
//==================================================================
void SpaceShooterClass::left()
{
  if (millis() - offsetT >= 50 && play)
  {
    changeShipX = -6;
    changeShipY = 0;
    offsetT = millis();
  }
}
//==================================================================
void SpaceShooterClass::right()
{
  if (millis() - offsetT >= 50 && play)
  {
    changeShipX = 6;
    changeShipY = 0;
    offsetT = millis();
  }
}
//==================================================================
void SpaceShooterClass::select()
{
  if (millis() - offsetF >= 500 && play)
  {
    fire = true;
    offsetF = millis();
  }
  if (!beginGame)
  {
    beginGame = true;
  }
}

//==================================================================
void SpaceShooterClass::spaceShoot_run()
{
  initVars();
  memset(alienLive, true, 18);
  memset(aFireX, 0, 5);
  memset(aFireY, 0, 5);
  memset(aFireAge, 0, 5);
  GO.Lcd.fillScreen(BLACK);
  GO.Lcd.setTextColor(0x5E85);
  GO.Lcd.setTextSize(3);
  randomSeed(analogRead(2));
  while (1)
  {
    if (GO.JOY_X.isAxisPressed() == 2)
    {
      left();
    }
    if (GO.JOY_X.isAxisPressed() == 1)
    {
      right();
    }
    if (GO.BtnA.isPressed())
    {
      this->select();
    }
    
    if (GO.BtnB.isPressed()) 
    {
      return;
    }
    //-------------Start Screen--------------
    if (millis() - offsetS >= 900 && !beginGame)
    {
      if (!startPrinted)
      {
        GO.Lcd.drawString(">START<", 77, 105);
        startPrinted = true;
        offsetS = millis();
      }
      else
      {
        GO.Lcd.fillRect(77, 105, 244, 32, BLACK);
        startPrinted = false;
        offsetS = millis();
      }
    }
    if (beginGame && beginGame2)
    {
      GO.Lcd.fillRect(77, 105, 244, 32, BLACK);
      beginGame2 = false;
      play = true;
    }
    //-------------Player-----------------------------------------------
    if (millis() - offsetM >= shipSpeed && play)
    {
      moveShip();
      offsetM = millis();
    }
    if (oldShipX != shipX || oldShipY != shipY)
    {
      GO.Lcd.fillRect(oldShipX, oldShipY, 28, 44, BLACK);
      oldShipX = shipX;
      oldShipY = shipY;
      drawBitmap(shipImg, shipImgW, shipImgH, shipX, shipY, 2);
    }
    if (fire && play)
    {
      fireDaLazer();
    }
    if (millis() - offsetB >= 50)
    {
      for (int i = 0; i < 5; i++)
      {
        if (fFireAge[i] < 20 && fFireAge[i] > 0)
        {
          keepFirinDaLazer(i);
        }
        if (fFireAge[i] == 20)
        {
          stopFirinDaLazer(i);
        }
      }
      offsetB = millis();
    }
    if (millis() - offsetT > 50)
    {
      changeShipX = 0;
      changeShipY = 0;
    }
    //---------------Aliens--------------------------------------------
    if (millis() - offsetA >= alienSpeed && play)
    {
      moveAliens();
      offsetA = millis();
    }
    if (findAlienX(5) >= 294)
    {
      changeAlienX = -3;
      changeAlienY = 7;
    }
    if (alienX <= 6)
    {
      changeAlienX = 3;
      changeAlienY = 7;
    }
    alienLiveCount = 0;
    for (int i = 0; i < 18; i++)
    {
      if (alienLive[i])
      {
        alienLiveCount += 1;
        if (alienShot(i))
        {
          GO.Lcd.fillRect(findOldAlienX(i), findOldAlienY(i), 28, 22, BLACK);
          alienLiveCount -= 1;
          alienLive[i] = false;
          alien_shooter_score += alien_shooter_scoreInc;
        }
        if (onPlayer(i) || exceedBoundary(i))
        {
          return;
        }
      }
    }
    if (alienLiveCount == 1)
    {
      oldAlienSpeed = alienSpeed;
      if (alienSpeed > 50)
      {
        alienSpeed -= 10;
      }
      else
      {
        alienSpeed = 20;
      }
    }
    if (alienLiveCount == 0)
    {
      levelUp();
    }
    GO.update();
  }
}

void SpaceShooterClass::Run()
{
  spaceShoot_run();
  gameOver();
}

SpaceShooterClass::SpaceShooterClass()
{
}

SpaceShooterClass::~SpaceShooterClass()
{
  GO.Lcd.fillScreen(0);
  GO.Lcd.setTextSize(1);
  GO.Lcd.setTextFont(1);
  GO.drawAppMenu(F("GAMES"), F("ESC"), F("SELECT"), F("LIST"));
  GO.showList();
}