#pragma once

#include <SFML/Graphics.hpp>
#include <time.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "Source.h"
using namespace sf;

class Tetris
{
    const Color colorMap[6] = 
    {
        Color::Green, Color::Blue, Color::Red,
        Color::Yellow, Color::Magenta, Color::Cyan
    };

    static const int HEIGHT = 25;
    static const int WIDTH = 15;
    const int CELL_SIZE = 35;
    const int SCREENWIDTH = (WIDTH + 5) * CELL_SIZE;
    const int SCREENHEIGHT = (HEIGHT + 1) * CELL_SIZE;

    int score = 0;
    int linesClearedInAGame = 0;
    int previousBlockType = -1, curBlockType = 0, nextBlockType = 0, holdBlockType = -1;

    bool bGameOver = false;
    bool bGoTillLocked = false;
    bool bSwapWithHoldBlock = false;
    bool bSwappedThisTurn = false;
    bool bStopControls = false;
    bool bSpawnNextBlock = false;

    struct Point { int x, y; } a[4], b[4], hold[4] = { NULL };
    
    struct Tetromino
    {
        Point blocks[4];
        sf::Color color;
        int blockType = -1;
    };

    int field[HEIGHT][WIDTH] = { 0 };

    int figures[7][4] =
    {
        1,3,5,7, // I
        2,4,5,7, // Z
        3,5,4,6, // S
        3,5,4,7, // T
        2,3,5,7, // L
        3,5,7,6, // J
        2,3,4,5, // O
    };

public:
    void Game();
    
private:
    void RunGameLoop(sf::Clock& clock, float& timer, sf::RenderWindow& window, bool& rotate, int& dx, float& delay, int& colorNum, sf::Sprite& background, sf::Sprite& s, sf::Sprite& frame, Clock matchTime);
    void GetPlayerInput(sf::RenderWindow& window, bool& rotate, int& dx, int& colorNum, float& timer, float& delay);
    bool Check(Point val[4]);
    void Move(int& dx);
    void Rotate(bool& rotate);
    void Tick(float& timer, float& delay, int& colorNum);
    void SpawnNextBlock(int& colorNum, bool bBlockSwapped = false);
    void CheckLines(sf::RenderWindow& window);
    void ClearFinishedLines(std::vector<int>& completedLines);
    void Draw(sf::RenderWindow& window, sf::Sprite& background, sf::Sprite& s, int& colorNum, sf::Sprite& frame, Clock matchTime);
    void DrawNextBlockPreview(sf::RenderWindow& window);
    void DrawHoldBlock(sf::RenderWindow& window);
    void DrawGridLines(sf::RenderWindow& window, RectangleShape back);
    void DrawUIText(sf::RenderWindow& window, const String textString, Vector2f textPos, int textSize, int drawDirection);
};

