#include "FlappyBird.h"

// ---------------
// game loop
// ---------------
void FlappyBirdClass::game_loop()
{
    // ===============
    // prepare game variables
    // draw floor
    // ===============
    // instead of calculating the distance of the floor from the screen height each time store it in a variable
    unsigned char GAMEH = TFTH - FLOORH;
    // draw the floor once, we will not overwrite on this area in-game
    // black line
    GO.Lcd.drawFastHLine(0, GAMEH, TFTW, BLACK);
    // grass and stripe
    GO.Lcd.fillRect(0, GAMEH + 1, TFTW2, GRASSH, GRASSCOL);
    GO.Lcd.fillRect(TFTW2, GAMEH + 1, TFTW2, GRASSH, GRASSCOL2);
    // black line
    GO.Lcd.drawFastHLine(0, GAMEH + GRASSH, TFTW, BLACK);
    // mud
    GO.Lcd.fillRect(0, GAMEH + GRASSH + 1, TFTW, FLOORH - GRASSH, FLOORCOL);
    // grass x position (for stripe animation)
    long grassx = TFTW;
    // game loop time variables
    double delta, old_time, next_game_tick, current_time;
    next_game_tick = current_time = millis();
    int loops;
    // passed pipe flag to count flappy_bird_score
    bool passed_pipe = false;
    // temp var for setAddrWindow
    unsigned char px;

    while (1)
    {
        loops = 0;
        while (millis() > next_game_tick && loops < MAX_FRAMESKIP)
        {
            // ===============
            // input
            // ===============
            if (GO.BtnA.wasPressed())
            {
                // if the bird is not too close to the top of the screen apply jump force
                if (bird.y > BIRDH2 * 0.5)
                {
                    bird.vel_y = -JUMP_FORCE;
                }
                // else zero velocity
                else
                {
                    bird.vel_y = 0;
                }
            }
            GO.update();

            // ===============
            // update
            // ===============
            // calculate delta time
            // ---------------
            old_time = current_time;
            current_time = millis();
            delta = (current_time - old_time) / 1000;

            // bird
            // ---------------
            bird.vel_y += GRAVITY * delta;
            bird.y += bird.vel_y;

            // pipe
            // ---------------
            pipes.x -= SPEED;
            // if pipe reached edge of the screen reset its position and gap
            if (pipes.x < -PIPEW)
            {
                pipes.x = TFTW;
                pipes.gap_y = random(10, GAMEH - (10 + GAPHEIGHT));
            }

            // ---------------
            next_game_tick += SKIP_TICKS;
            loops++;
        }

        // ===============
        // draw
        // ===============
        // pipe
        // ---------------
        // we save cycles if we avoid drawing the pipe when outside the screen
        if (pipes.x >= 0 && pipes.x < TFTW)
        {
            // pipe color
            GO.Lcd.drawFastVLine(pipes.x + 3, 0, pipes.gap_y, PIPECOL);
            GO.Lcd.drawFastVLine(pipes.x + 3, pipes.gap_y + GAPHEIGHT + 1, GAMEH - (pipes.gap_y + GAPHEIGHT + 1), PIPECOL);
            // highlight
            GO.Lcd.drawFastVLine(pipes.x, 0, pipes.gap_y, PIPEHIGHCOL);
            GO.Lcd.drawFastVLine(pipes.x, pipes.gap_y + GAPHEIGHT + 1, GAMEH - (pipes.gap_y + GAPHEIGHT + 1), PIPEHIGHCOL);
            // bottom and top border of pipe
            GO.Lcd.drawPixel(pipes.x, pipes.gap_y, PIPESEAMCOL);
            GO.Lcd.drawPixel(pipes.x, pipes.gap_y + GAPHEIGHT, PIPESEAMCOL);
            // pipe seam
            GO.Lcd.drawPixel(pipes.x, pipes.gap_y - 6, PIPESEAMCOL);
            GO.Lcd.drawPixel(pipes.x, pipes.gap_y + GAPHEIGHT + 6, PIPESEAMCOL);
            GO.Lcd.drawPixel(pipes.x + 3, pipes.gap_y - 6, PIPESEAMCOL);
            GO.Lcd.drawPixel(pipes.x + 3, pipes.gap_y + GAPHEIGHT + 6, PIPESEAMCOL);
        }
        // erase behind pipe
        if (pipes.x <= TFTW)
        {
            GO.Lcd.drawFastVLine(pipes.x + PIPEW, 0, GAMEH, BCKGRDCOL);
        }
        // bird
        // ---------------
        tmpx = BIRDW - 1;
        do
        {
            px = bird.x + tmpx + BIRDW;
            // clear bird at previous position stored in old_y
            // we can't just erase the pixels before and after current position
            // because of the non-linear bird movement (it would leave 'dirty' pixels)
            tmpy = BIRDH - 1;
            do
            {
                GO.Lcd.drawPixel(px, bird.old_y + tmpy, BCKGRDCOL);
            } while (tmpy--);
            // draw bird sprite at new position
            tmpy = BIRDH - 1;
            do
            {
                GO.Lcd.drawPixel(px, bird.y + tmpy, birdcol[tmpx + (tmpy * BIRDW)]);
            } while (tmpy--);
        } while (tmpx--);
        // save position to erase bird on next draw
        bird.old_y = bird.y;
        // grass stripes
        // ---------------
        grassx -= SPEED;
        if (grassx < 0)
        {
            grassx = TFTW;
        }
        GO.Lcd.drawFastVLine(grassx % TFTW, GAMEH + 1, GRASSH - 1, GRASSCOL);
        GO.Lcd.drawFastVLine((grassx + 64) % TFTW, GAMEH + 1, GRASSH - 1, GRASSCOL2);

        // ===============
        // collision
        // ===============
        // if the bird hit the ground game over
        if (bird.y > GAMEH - BIRDH)
        {
            break;
        }
        // checking for bird collision with pipe
        if (bird.x + BIRDW >= pipes.x - BIRDW2 && bird.x <= pipes.x + PIPEW - BIRDW)
        {
            // bird entered a pipe, check for collision
            if (bird.y < pipes.gap_y || bird.y + BIRDH > pipes.gap_y + GAPHEIGHT)
            {
                break;
            }
            else
            {
                passed_pipe = true;
            }
        }
        // if bird has passed the pipe increase flappy_bird_score
        else if (bird.x > pipes.x + PIPEW - BIRDW && passed_pipe)
        {
            passed_pipe = false;
            // erase flappy_bird_score with background color
            GO.Lcd.setTextColor(BCKGRDCOL);
            GO.Lcd.drawNumber(flappy_bird_score, TFTW2, 4);
            // set text color back to white for new flappy_bird_score
            GO.Lcd.setTextColor(WHITE);
            // increase flappy_bird_score since we successfully passed a pipe
            flappy_bird_score++;
        }
        // update flappy_bird_score
        // ---------------
        GO.Lcd.drawNumber(flappy_bird_score, TFTW2, 4);
    }
    // add a small delay to show how the player lost
    vTaskDelay(1200 / portTICK_PERIOD_MS);
}

void FlappyBirdClass::game_init()
{
    // clear screen
    GO.Lcd.fillScreen(BCKGRDCOL);
    // reset flappy_bird_score
    flappy_bird_score = 0;
    // init bird
    bird.x = 144;
    bird.y = bird.old_y = TFTH2 - BIRDH;
    bird.vel_y = -JUMP_FORCE;
    tmpx = tmpy = 0;
    // generate new random seed for the pipe gape
    randomSeed(analogRead(0));
    // init pipe
    pipes.x = 0;
    pipes.gap_y = random(20, TFTH - 60);
}

// ---------------
// game start
// ---------------
void FlappyBirdClass::game_start()
{
    GO.Lcd.fillScreen(BLACK);
    GO.Lcd.fillRect(10, TFTH2 - 20, TFTW - 20, 1, WHITE);
    GO.Lcd.fillRect(10, TFTH2 + 32, TFTW - 20, 1, WHITE);
    GO.Lcd.setTextColor(WHITE);
    GO.Lcd.setTextSize(3);
    // half width - num char * char width in pixels
    GO.Lcd.drawString("FLAPPY", TFTW2 - (6 * 9), TFTH2 - 16);
    GO.Lcd.setTextSize(3);
    GO.Lcd.drawString("-BIRD-", TFTW2 - (6 * 9), TFTH2 + 8);
    GO.Lcd.setTextSize(2);
    GO.Lcd.drawString("OdroidGO", 10, TFTH2 - 36);
    GO.Lcd.drawString("Mod by Calin", TFTW2 - (17 * 9), TFTH2 + 36);
    while (1)
    {
        // wait for push button
        if (GO.BtnA.wasPressed())
        {
            break;
        }
        GO.update();
    }
    // init game settings
    game_init();
}

void FlappyBirdClass::resetMaxflappy_bird_score()
{
    preferences.begin("Fpsc", false);
    preferences.putInt("sc", flappy_bird_score);
    preferences.end();
}

// ---------------
// game over
// ---------------
void FlappyBirdClass::game_over()
{
    GO.Lcd.fillScreen(BLACK);
    preferences.begin("Fpsc", false);
    maxflappy_bird_score = preferences.getInt("sc", 0);

    if (flappy_bird_score > maxflappy_bird_score)
    {
        preferences.putInt("sc", flappy_bird_score);
        maxflappy_bird_score = flappy_bird_score;
        GO.Lcd.setTextColor(RED);
        GO.Lcd.setTextSize(2);
        //GO.Lcd.setCursor(TFTW2 - (13 * 6), TFTH2 - 26);
        GO.Lcd.drawString("NEW HIGHSCORE", TFTW2 - (13 * 6), TFTH2 - 26);
    }
    preferences.end();

    GO.Lcd.setTextColor(WHITE);
    GO.Lcd.setTextSize(3);
    // half width - num char * char width in pixels
    GO.Lcd.drawString("GAME OVER", TFTW2 - (9 * 9), TFTH2 - 6);
    GO.Lcd.setTextSize(2);
    GO.Lcd.drawString("Score: " + String(flappy_bird_score), 10, 10);
    GO.Lcd.drawString("press button", TFTW2 - (12 * 6), TFTH2 + 18);
    GO.Lcd.drawString("Max Score:" + String(maxflappy_bird_score), 10, 28);
    GO.Lcd.drawString("ScoreR", 30, 221);
    GO.Lcd.drawString("Start", 132, 221);
    GO.Lcd.drawString("Exit", 232, 221);
    while (1)
    {
        if (GO.BtnStart.wasPressed())
        {
            resetMaxflappy_bird_score();
        }
        if (GO.BtnA.wasPressed())
        {
            Run();
        }
        if (GO.BtnB.wasPressed())
        {
            return;
        }
        GO.update();
    }
}

void FlappyBirdClass::Run()
{
    // put your main code here
    game_start();
    game_loop();
    game_over();
}

FlappyBirdClass::FlappyBirdClass()
{
}

FlappyBirdClass::~FlappyBirdClass()
{
    GO.Lcd.fillScreen(0);
    GO.Lcd.setTextSize(1);
	GO.Lcd.setTextFont(1);
    GO.drawAppMenu(F("GAMES"), F("ESC"), F("SELECT"), F("LIST"));
    GO.showList();
}