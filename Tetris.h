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
    const Color colorMap[7] =
    {
        Color::Green, Color(255, 113, 5) /*Orange*/, Color::Red,
        Color::Yellow, Color::Magenta, Color::Cyan, Color(74, 64, 112) /*Lineclear Color*/
    };

    static const int HEIGHT = 25;
    static const int WIDTH = 15;
    const int CELL_SIZE = 34;
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

    struct Point { int x, y; };
  
    struct Tetromino
    {
        Point blocks[4] = { NULL };
        int colorNum = -1;
        int blockType = -1;

        Tetromino(int _blockType, int figures[7][4])
        {
            blockType = _blockType;

            for (int i = 0; i < 4; i++)
            {
                blocks[i].x = (figures[_blockType][i] % 2) + 6; // sets new blocks x pos
                blocks[i].y = figures[_blockType][i] / 2;
            }

            colorNum = rand() % 6;
        }

        Tetromino()
        {
            
        }
    };

    Tetromino* spawnedTetromino = nullptr, backupTetromino, holdTetromino;

    std::vector<int> completedLines;

    int field[HEIGHT][WIDTH] = { 0 }; //TODO - Create Lines Class that holds each line's cells and has bCleared and timer var for better control

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

    Clock waitTime;

public:
    void Game();
    
private:
    void RunGameLoop(sf::Clock& clock, float& timer, sf::RenderWindow& window, float& delay, Clock matchTime);
    void GetPlayerInput(sf::RenderWindow& window, bool& rotate, int& dx, float& timer, float& delay);
    bool Check(const Tetromino* tetrominoToCheck);
    void Move(int& dx);
    void Rotate(bool& rotate);
    void Tick(float& timer, float& delay);
    void PlaceBlockInField();
    void SpawnNextBlock();
    void CheckLines(sf::RenderWindow& window);
    void ClearFinishedLines(std::vector<int>& completedLines);
    void Draw(sf::RenderWindow& window, Clock matchTime);
    void DrawNextBlockPreview(sf::RenderWindow& window, sf::RectangleShape tetrisBoard);
    void DrawHoldBlock(sf::RenderWindow& window, sf::RectangleShape tetrisBoard);
    void DrawGridLines(sf::RenderWindow& window, RectangleShape back);
    void DrawUIText(sf::RenderWindow& window, const String textString, Vector2f textPos, int textSize, int drawDirection);
};

