#include "Tetris.h"

//========================================================================
void TetrisClass::Run()
{
    GO.Lcd.fillScreen(BLACK); // CLEAR SCREEN
    //----------------------------// Make Blocks ----------------------------
    make_block(0, BLACK);  // Type No, Color
    make_block(1, 0x00F0); // DDDD     RED
    make_block(2, 0xFBE4); // DD,DD    PUPLE
    make_block(3, 0xFF00); // D__,DDD  BLUE
    make_block(4, 0xFF87); // DD_,_DD  GREEN
    make_block(5, 0x87FF); // __D,DDD  YELLO
    make_block(6, 0xF00F); // _DD,DD_  LIGHT GREEN
    make_block(7, 0xF8FC); // _D_,DDD  PINK
    //----------------------------------------------------------------------
    GO.Lcd.drawJpg((uint8_t *)tetris_img, (sizeof(tetris_img) / sizeof(tetris_img[0])));
    GO.Lcd.setFreeFont(FFS9B);
    PutStartPos(); // Start Position
    for (int i = 0; i < 4; ++i)
    {
        screen[pos.X + block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = block.color;
    }
    Draw(); // Draw block
    GO.Lcd.setTextColor(BLUE);
    GO.Lcd.drawCentreString("Score", 50, 150, 1);
    GO.Lcd.drawCentreString("Max Score", 50, 190, 1);
    GO.Lcd.drawCentreString(String(max_score), 50, 210, 1);

    while (1)
    {
        if (gameover)
        {
            GO.update();
            GO.Lcd.setFreeFont(FSS24);
            GO.Lcd.setTextColor(RED);
            GO.Lcd.drawCentreString("GAME OVER", 160, 110, 1);

            while (!GO.BtnB.wasPressed())
            {
                GO.update();
            }
            return;
        }
        Point next_pos;
        int next_rot = rot;
        GetNextPosRot(&next_pos, &next_rot);
        ReviseScreen(next_pos, next_rot);
        GO.update();
        
        if (GO.BtnB.wasPressed()) 
        {
            return;
        }
        
        delay(game_speed); // SPEED ADJUST
    }
}
//========================================================================
void TetrisClass::Draw()
{ 
    for (int i = 0; i < Width; ++i)
    {
        for (int j = 0; j < Height; ++j)
        {
            for (int k = 0; k < Length; ++k)
            {
                for (int l = 0; l < Length; ++l)
                {
                    backBuffer[j * Length + l][i * Length + k] = BlockImage[screen[i][j]][k][l];
                }
            }
        }
    }
    GO.Lcd.drawBitmap(100, 0, 120, 240, (uint8_t *)backBuffer);
}
//========================================================================
void TetrisClass::PutStartPos()
{
    pos.X = 4;
    pos.Y = 1;
    block = blocks[random(7)];
    rot = random(block.numRotate);
    GO.Lcd.setTextColor(3608);
    GO.Lcd.drawCentreString(String(score), 50, 170, 1);
    GO.Lcd.setTextColor(RED);
    score++;
    GO.Lcd.drawCentreString(String(score), 50, 170, 1);

    if (score > max_score)
    {
        GO.Lcd.setTextColor(3608);
        GO.Lcd.drawCentreString(String(max_score), 50, 210, 1);
        GO.Lcd.setTextColor(RED);
        max_score = score;
        GO.Lcd.drawCentreString(String(max_score), 50, 210, 1);
    }
}
//========================================================================
bool TetrisClass::GetSquares(Block block, Point pos, int rot, Point *squares)
{
    bool overlap = false;
    for (int i = 0; i < 4; ++i)
    {
        Point p;
        p.X = pos.X + block.square[rot][i].X;
        p.Y = pos.Y + block.square[rot][i].Y;
        overlap |= p.X < 0 || p.X >= Width || p.Y < 0 || p.Y >= Height || screen[p.X][p.Y] != 0;
        squares[i] = p;
    }
    return !overlap;
}
//========================================================================
void TetrisClass::GameOver()
{
    for (int i = 0; i < Width; ++i)
    {
        for (int j = 0; j < Height; ++j)
            if (screen[i][j] != 0)
            {
                screen[i][j] = 4;
            }
    }
    gameover = true;
}
//========================================================================
void TetrisClass::ClearKeys()
{
    but_A = false;
    but_LEFT = false;
    but_RIGHT = false;
}
//========================================================================
bool TetrisClass::KeyPadLoop()
{
    if (GO.JOY_X.wasAxisPressed() == 2)
    {
        ClearKeys();
        but_LEFT = true;
        return true;
    }
    if (GO.JOY_X.wasAxisPressed() == 1)
    {
        ClearKeys();
        but_RIGHT = true;
        return true;
    }
    if (GO.BtnA.wasPressed())
    {
        ClearKeys();
        but_A = true;
        return true;
    }
    return false;
}
//========================================================================
void TetrisClass::GetNextPosRot(Point *pnext_pos, int *pnext_rot)
{
    bool received = KeyPadLoop();
    if (but_A)
    {
        started = true;
    }
    if (!started)
    {
        return;
    }
    pnext_pos->X = pos.X;
    pnext_pos->Y = pos.Y;
    if ((fall_cnt = (fall_cnt + 1) % 10) == 0)
    {
        pnext_pos->Y += 1;
    }
    else if (received)
    {
        if (but_LEFT)
        {
            but_LEFT = false;
            pnext_pos->X -= 1;
        }
        else if (but_RIGHT)
        {
            but_RIGHT = false;
            pnext_pos->X += 1;
        }
        else if (but_A)
        {
            but_A = false;
            *pnext_rot = (*pnext_rot + block.numRotate - 1) % block.numRotate;
        }
    }
}
//========================================================================
void TetrisClass::DeleteLine()
{
    for (int j = 0; j < Height; ++j)
    {
        bool Delete = true;
        for (int i = 0; i < Width; ++i)
        {
            if (screen[i][j] == 0)
            {
                Delete = false;
            }
        }
        if (Delete)
        {
            for (int k = j; k >= 1; --k)
            {
                for (int i = 0; i < Width; ++i)
                {
                    screen[i][k] = screen[i][k - 1];
                }
            }
        }
    }
}
//========================================================================
void TetrisClass::ReviseScreen(Point next_pos, int next_rot)
{
    if (!started)
    {
        return;
    }
    Point next_squares[4];
    for (int i = 0; i < 4; ++i)
    {
        screen[pos.X + block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = 0;
    }
    if (GetSquares(block, next_pos, next_rot, next_squares))
    {
        for (int i = 0; i < 4; ++i)
        {
            screen[next_squares[i].X][next_squares[i].Y] = block.color;
        }
        pos = next_pos;
        rot = next_rot;
    }
    else
    {
        for (int i = 0; i < 4; ++i)
        {
            screen[pos.X + block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = block.color;
        }
        if (next_pos.Y == pos.Y + 1)
        {
            DeleteLine();
            PutStartPos();
            if (!GetSquares(block, pos, rot, next_squares))
            {
                for (int i = 0; i < 4; ++i)
                {
                    screen[pos.X + block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = block.color;
                }
                GameOver();
            }
        }
    }
    Draw();
}
//========================================================================
void TetrisClass::make_block(int n, uint16_t color)
{ 
    for (int i = 0; i < 12; i++)
        for (int j = 0; j < 12; j++)
        {
            BlockImage[n][i][j] = color; // Block color
            if (i == 0 || j == 0)
            {
                BlockImage[n][i][j] = 0; // BLACK Line
            }
        }
}
//========================================================================

TetrisClass::TetrisClass()
{
    preferences.begin("T_Score", false);
    this->max_score = preferences.getInt("t_sc", 0);
    preferences.end();
}

TetrisClass::~TetrisClass()
{
    preferences.begin("T_Score", false);
    preferences.putInt("t_sc", max_score);
    preferences.end();


    GO.Lcd.fillScreen(0);
    GO.Lcd.setTextSize(1);
	GO.Lcd.setTextFont(1);
    GO.drawAppMenu(F("GAMES"), F("ESC"), F("SELECT"), F("LIST"));
    GO.showList();
}