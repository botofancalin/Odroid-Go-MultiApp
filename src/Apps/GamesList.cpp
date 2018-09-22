#include "GamesList.h"

// the game list menu
void GamesListClass::Run()
{
    GO.update();
    GO.drawAppMenu(F("GAMES"), F("ESC"), F("SELECT"), F("LIST"));
    GO.clearList();
    GO.setListCaption("GAMES");

    // The list items Add new items to list to be displayed
    GO.addList("TETRIS");
    GO.addList("FLAPPY BIRD");
    GO.addList("ALIEN SHOOTER");
    GO.showList();

    while (!GO.BtnB.wasPressed())
    {
        if (GO.JOY_Y.wasAxisPressed()==1)
        {
            GO.nextList();
        }
        if (GO.JOY_Y.wasAxisPressed()==2)
        {
            GO.previousList();
        }
        if (GO.BtnA.wasPressed())
        {
            if (GO.getListString() == "TETRIS")
            {
                GO.update();
                TetrisClass *TetrisObj = new TetrisClass();
                TetrisObj->Run();
                TetrisClass::DestroyInstance(TetrisObj);
            }
            if (GO.getListString() == "FLAPPY BIRD")
            {
                GO.update();
                FlappyBirdClass FlappyBirdObj;
                FlappyBirdObj.Run();
            }
            if (GO.getListString() == "ALIEN SHOOTER")
            {
                GO.update();
                // The game object with member call
                SpaceShooterClass SpaceShooterObj;
                SpaceShooterObj.Run();
            }
        }
        GO.update();
    }
}

GamesListClass::GamesListClass()
{
}

GamesListClass::~GamesListClass()
{
    GO.show();
}