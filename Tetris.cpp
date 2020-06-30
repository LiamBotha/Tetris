#include "Tetris.h"

void Tetris::Game()
{
    srand(time(0));

    RenderWindow window(VideoMode(1600, 900), "The Game!");
    sf::View view(sf::FloatRect(0, 0, 1600, 900));
    window.setView(view);

    Texture t1, t2, t3;
    t1.loadFromFile("images/tiles.png");
    t2.loadFromFile("images/background.png");
    t3.loadFromFile("images/frame.png");

    Sprite s(t1), background(t2), frame(t3);
    float timer = 0, delay = 1;

    // add tracker to how many lines cleared in a row

    Clock deltaTime, matchTime;

    curBlockType = rand() % 7; // chooses new blocks type
    nextBlockType = rand() % 7;

    SpawnNextBlock();

    while (window.isOpen())
    {
        RunGameLoop(deltaTime, timer, window, delay, matchTime);
    }

    delete spawnedTetromino;
}

void Tetris::RunGameLoop(sf::Clock& clock, float& timer, sf::RenderWindow& window, float& delay, Clock& matchTime)
{
    int dx = 0; bool rotate = 0; // dx is the direction to rotate
    speedLevel = linesClearedInAGame / 10;

    delay = std::max(0.01, 1  - ((speedLevel * 5.0) / 100));

    timer += clock.getElapsedTime().asSeconds();
    clock.restart();

    GetPlayerInput(window, rotate, dx, timer, delay, matchTime);

    if (!bGameOver)
    {
        if (bSpawnNextBlock)
            SpawnNextBlock();

        if (Keyboard::isKeyPressed(Keyboard::Down) && !bGoTillLocked && spawnedTetromino != NULL) // add to score for every line the block gets soft dropped
        {
            bIsSoftDropping = true;
            delay = std::min(0.06f, delay);
        }

        if (!bStopControls)
        {
            // Move
            Move(dx);

            // Rotate
            Rotate(rotate);
        }

        // Tick
        Tick(timer, delay);

        // Check Lines
        CheckLines(window);
    }

    // Draw
    Draw(window, matchTime);

}

void Tetris::GetPlayerInput(sf::RenderWindow& window, bool& rotate, int& dx, float& timer, float& delay, Clock& matchTime)
{
    Event e;
    while (window.pollEvent(e))
    {
        // catch the resize events
        if (e.type == sf::Event::Resized)
        {
            // update the view to the new size of the window
            sf::FloatRect visibleArea(0, 0, e.size.width, e.size.height);
            window.setView(sf::View(visibleArea));
        }

        if (e.type == Event::Closed)
            window.close();

        if (e.type == Event::KeyReleased)
        {
            if (e.key.code == Keyboard::Space)
            {
                bGoTillLocked = true;
                bStopControls = false;
            }
            else if (e.key.code == Keyboard::Enter && bGameOver)
            {
                RestartGame(dx, rotate, delay, timer, matchTime);
            }
        }
        else if (e.type == Event::KeyPressed && !bGoTillLocked)
        {
            if (e.key.code == Keyboard::Space)
            {
                bStopControls = true;
            }
            else if (e.key.code == Keyboard::Up)
                rotate = true;
            else if (e.key.code == Keyboard::Left)
                dx = -1;
            else if (e.key.code == Keyboard::Right)
                dx = 1;
            else if (e.key.code == Keyboard::Z && !bSwappedThisTurn)
                bSwapWithHoldBlock = true;
        }
    }
}

void Tetris::RestartGame(int& dx, bool& rotate, float& delay, float& timer, sf::Clock& matchTime)
{
    bGameOver = false;
    bGoTillLocked = false;
    bSwapWithHoldBlock = false;
    bSwappedThisTurn = false;
    bStopControls = false;
    bSpawnNextBlock = false;
    bIsSoftDropping = false;

    linesClearedInAGame = 0;

    previousBlockType = -1;
    curBlockType = rand() % 7; // chooses new blocks type
    nextBlockType = rand() % 7;

    if (spawnedTetromino != NULL)
    {
        delete spawnedTetromino;
        spawnedTetromino = NULL;

        completedLines.clear();
    }

    for (int i = 0; i < 4; i++)
    {
        holdTetromino.blocks[i] = { NULL };
    }

    holdTetromino.blockType = -1;
    holdTetromino.colorNum = -1;
    holdTetromino.rotation = -1;

    dx = 0;
    rotate = 0;
    delay = 0.3;
    score = 0;
    timer = 0;

    matchTime.restart();
    waitTime.restart();

    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            field[i][j] = 0;
        }
    }
}

bool Tetris::Check(const Tetromino* tetrominoToCheck)
{
    if (spawnedTetromino == NULL)
        return 0;

    auto val = tetrominoToCheck->blocks;

    for (int i = 0; i < 4; i++)
    {
        if (val[i].x < 0 || val[i].x >= WIDTH || val[i].y >= HEIGHT)
            return 0;
        else if (field[val[i].y][val[i].x])
            return 0;
    }

    return 1;
}

void Tetris::Move(int& dx)
{
    if (spawnedTetromino != NULL)
    {
        backupTetromino = *spawnedTetromino;

        for (int i = 0; i < 4; i++)
        {
            spawnedTetromino->blocks[i].x += dx;
        }

        if (!Check(spawnedTetromino))
        {
            for (int i = 0; i < 4; i++)
            {
                spawnedTetromino->blocks[i] = backupTetromino.blocks[i];
            }
        }
    }
}

void Tetris::Rotate(bool& rotate) // TODO - Break into more chunks

{
    if (rotate && spawnedTetromino != NULL)
    {
        Point p = spawnedTetromino->blocks[1]; //center of rotation // TODO - rework rotation point on individual basis

        switch (spawnedTetromino->blockType) // For each Block Type. Returns if block is square or doesn't exist
        {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            return;
        }

        if (spawnedTetromino->blockType == 0 && spawnedTetromino->rotation == 2)
            p = spawnedTetromino->blocks[2];

        for (int i = 0; i < 4; i++)
        {
            int x = spawnedTetromino->blocks[i].y - p.y;
            int y = spawnedTetromino->blocks[i].x - p.x;

            spawnedTetromino->blocks[i].x = p.x - x;
            spawnedTetromino->blocks[i].y = p.y + y;
        }

        int pushDir = 0;
        bool bReset = false;

        // Handles Wall Clipping 
        for (int i = 0; i < 4; i++)
        {
            if (curBlockType == 0)
            {
                if (spawnedTetromino->blocks[i].x < 0)
                    pushDir = 2;
                else if (spawnedTetromino->blocks[i].x >= WIDTH)
                    pushDir = -2;
            }
            else
            {
                if (spawnedTetromino->blocks[i].x < 0)
                    pushDir = 1;
                else if (spawnedTetromino->blocks[i].x >= WIDTH)
                    pushDir = -1;
            }

            if (spawnedTetromino->blocks[i].y >= HEIGHT || field[spawnedTetromino->blocks[i].y][spawnedTetromino->blocks[i].x])
                bReset = true;
        }

        for (int i = 0; i < 4; i++)
        {
            if (bReset)
            {
                spawnedTetromino->blocks[i] = backupTetromino.blocks[i];
            }
            else
            {
                spawnedTetromino->blocks[i].x += pushDir;
            }
        }

        if (!bReset)
            spawnedTetromino->rotation = (spawnedTetromino->rotation + 1 % 4);
    }
}

void Tetris::Tick(float& timer, float& delay)
{   
    if (bGoTillLocked)
    {
        while (Check(spawnedTetromino))
        {
            backupTetromino = *spawnedTetromino;

            for (int i = 0; i < 4; i++)
            {
                spawnedTetromino->blocks[i].y += 1;
            }

            score += 2;
        }

        PlaceBlockInField();

        bSwappedThisTurn = false;

    }
    else if (bSwapWithHoldBlock)
    {
        bSwapWithHoldBlock = false;
        bSwappedThisTurn = true;

        int tempBlockType = holdBlockType;
        holdBlockType = curBlockType;

        if (holdTetromino.blocks[0].x != NULL)
        {
            curBlockType = tempBlockType;

            Tetromino temp = holdTetromino;
            holdTetromino = *spawnedTetromino;

            spawnedTetromino->blockType = temp.blockType;
            spawnedTetromino->colorNum = temp.colorNum;

            for (int i = 0; i < 4; i++) //swapping current block with hold block
            {
                spawnedTetromino->blocks[i].x = temp.blocks[i].x;
                spawnedTetromino->blocks[i].y = temp.blocks[i].y;

                //TODO figure how to get pivot and set to hold blocks pivot
                spawnedTetromino->blocks[i].x = (figures[curBlockType][i] % 2) + 6; // sets new blocks x pos
                spawnedTetromino->blocks[i].y = (figures[curBlockType][i] / 2);
            }
        }
        else
        {
            holdTetromino = *spawnedTetromino; //swapping current block with hold block

            delete spawnedTetromino;
            spawnedTetromino = NULL;
        }
    }
    else if (timer > delay && spawnedTetromino != NULL)
    {
        if (bIsSoftDropping)
        {
            score++;
            bIsSoftDropping = false;
        }

        backupTetromino = *spawnedTetromino;

        for (int i = 0; i < 4; i++)
        {
            spawnedTetromino->blocks[i].y += 1;
        }

        if (!Check(spawnedTetromino) && spawnedTetromino->collisionTime.getElapsedTime().asSeconds() > lockTime)
        {
            PlaceBlockInField();
            
            bSwappedThisTurn = false;
            bSpawnNextBlock = true;
        }
        else if(!Check(spawnedTetromino))
        {
            std::cout << "Falling" << std::endl;

            for (int i = 0; i < 4; i++) // reset it back so its not clipping wall
            {
                spawnedTetromino->blocks[i].x = backupTetromino.blocks[i].x;
                spawnedTetromino->blocks[i].y = backupTetromino.blocks[i].y;
            }
        }
        else
        {
            spawnedTetromino->collisionTime.restart();
        }

        timer = 0;
    }
}

void Tetris::PlaceBlockInField()
{
    if (spawnedTetromino != NULL)
    {
        for (int i = 0; i < 4; i++)
        {
            field[backupTetromino.blocks[i].y][backupTetromino.blocks[i].x] = backupTetromino.colorNum + 1; // attacked block to world on collison
        }

        delete spawnedTetromino;

        spawnedTetromino = NULL;

        waitTime.restart();
    }
}

void Tetris::SpawnNextBlock()
{
    bSpawnNextBlock = false;
    previousBlockType = curBlockType;
    curBlockType = nextBlockType; // chooses new blocks type
    nextBlockType = rand() % 7;
    bGoTillLocked = false;

    spawnedTetromino = new Tetromino(curBlockType, figures);
    spawnedTetromino->colorNum = rand() % 6;

    for (int i = 0; i < 4; i++)
    {
        spawnedTetromino->blocks[i].x = (figures[curBlockType][i] % 2) + 6; // sets new blocks x pos
        spawnedTetromino->blocks[i].y = (figures[curBlockType][i] / 2);
    }
}

void Tetris::CheckLines(sf::RenderWindow& window)
{
    int k = HEIGHT - 1; // Bottom Row
    int linesClearedAtOnce = 0;

    for (int i = HEIGHT - 1; i > 0; i--)
    {
        int count = 0;
        for (int j = 0; j < WIDTH; j++)
        {
            if (field[i][j] && i < 2) // if blocks reach top of grid then end game
            {
                bGameOver = true;
            }
            else if (field[i][j]) // increments count for each block in row
                count++;
        }

        if (count >= WIDTH)
        {
            if (std::find(completedLines.begin(), completedLines.end(), i) == completedLines.end())
            {
                std::cout << "Line: " << i << std::endl;

                completedLines.push_back(i);
                linesClearedAtOnce++; // amount cleared in a single move
                linesClearedInAGame++; // amount cleared over the whole game 
            }
        }
    }

    ClearFinishedLines(completedLines); // Check for T-spins?

    score += pow(linesClearedAtOnce, 2); // TODO - Use 40, 100, 300, 1200 rule -> 40  * (level + 1) or something similar
}

void Tetris::ClearFinishedLines(std::vector<int>& completedLines)
{
    for (int i = HEIGHT - 1; i > 0; i--)
    {
        if (std::find(completedLines.begin(), completedLines.end(), i) != completedLines.end())
        {
            for (int j = 0; j < WIDTH; j++)
            {
                field[i][j] = 7; // sets completed lines to a seperate color
            }
        }
    }

    //Timer gets reset everytime a new line is added.
    if (waitTime.getElapsedTime().asSeconds() > 0.3 && completedLines.size() > 0) // time completes lines can stay on screen for
    {   
        waitTime.restart();

        int newField[HEIGHT][WIDTH] = { 0 };
        for (int i = HEIGHT - 1, m = i; i >= 0 && m >= 0; i--, m--)
        {
            while(std::find(completedLines.begin(), completedLines.end(), m) != completedLines.end())
            {
                m--;
                completedLines.erase(completedLines.begin());
            }

            for (int j = 0; j < WIDTH; j++)
            {
                newField[i][j] = field[m][j]; // removes conpleted lines
            }
        }

        for (int i = HEIGHT - 1; i >= 0; i--)
        {
            for (int j = 0; j < WIDTH; j++)
            {
                field[i][j] = newField[i][j];
            }
        }

        completedLines.clear();
    }

    if (completedLines.size() == 0 && spawnedTetromino == NULL)
        bSpawnNextBlock = true;
}

void Tetris::Draw(sf::RenderWindow& window, Clock matchTime)
{
    RectangleShape cell(Vector2f(CELL_SIZE, CELL_SIZE));
    RectangleShape tetrisBoard(Vector2f(WIDTH * CELL_SIZE, (HEIGHT - 1) * CELL_SIZE)); // Tetris Board That Blocks are on
    RectangleShape tetrisOutline(Vector2f(WIDTH * CELL_SIZE, (HEIGHT - 1) * CELL_SIZE)); // Outline for the Tetris board
    window.clear(Color::Black);

    sf::Vertex background[] // draws the gradient background
    {
        sf::Vertex(Vector2f(0, 0), sf::Color(0, 22, 74)),// top
        sf::Vertex(Vector2f(window.getSize().x, 0), sf::Color(0, 22, 74)),

        sf::Vertex(Vector2f(window.getSize().x, window.getSize().y / 2), sf::Color(0, 88, 181)),
        sf::Vertex(Vector2f(0, window.getSize().y / 2), sf::Color(0, 88, 181)),// top middle

        sf::Vertex(Vector2f(0, window.getSize().y / 2), sf::Color(0, 88, 181)), //bot middle
        sf::Vertex(Vector2f(window.getSize().x, window.getSize().y / 2), sf::Color(0, 88, 181)),

        sf::Vertex(Vector2f(window.getSize().x, window.getSize().y), sf::Color(0, 22, 74)),
        sf::Vertex(Vector2f(0, window.getSize().y), sf::Color(0, 22, 74)), // bot
    };

    window.draw(background, 8, sf::Quads);

    tetrisBoard.setFillColor(Color::Black);
    tetrisBoard.move((window.getSize().x / 2) - (tetrisBoard.getGlobalBounds().width / 2), (window.getSize().y / 2) - (tetrisBoard.getGlobalBounds().height / 2));
    tetrisOutline.move((window.getSize().x / 2) - (tetrisBoard.getGlobalBounds().width / 2), (window.getSize().y / 2) - (tetrisBoard.getGlobalBounds().height / 2));

    tetrisOutline.setOutlineThickness(2);
    window.draw(tetrisOutline);

    window.draw(tetrisBoard);
    
    if (!bGameOver && spawnedTetromino != NULL)
    {
        RectangleShape previewBlock(Vector2f(CELL_SIZE, CELL_SIZE));
        Tetromino previewPosA = *spawnedTetromino;
        Tetromino previewPosB = *spawnedTetromino;

        while (Check(&previewPosA)) //Moves the preview to where it will lock in at the bottom
        {
            previewPosB = previewPosA;

            for (int i = 0; i < 4; i++)
            {
                previewPosA.blocks[i].y += 1;
            }
        }

        for (int i = 0; i < 4; i++) // Draw Preview Block
        {
            previewBlock.setFillColor(Color(255,255,255,100));
            previewBlock.setPosition(tetrisBoard.getGlobalBounds().left + (previewPosB.blocks[i].x) * CELL_SIZE, tetrisBoard.getGlobalBounds().top + (previewPosB.blocks[i].y - 1) * CELL_SIZE);
            window.draw(previewBlock);
        }


        for (int i = 0; i < 4; i++) // Draw Current Block
        {
            cell.setFillColor(colorMap[spawnedTetromino->colorNum]);
            cell.setPosition(tetrisBoard.getGlobalBounds().left + (spawnedTetromino->blocks[i].x) * CELL_SIZE, tetrisBoard.getGlobalBounds().top + (spawnedTetromino->blocks[i].y - 1) * CELL_SIZE);
            window.draw(cell);
        }
    }
    
    DrawGridLines(window, tetrisBoard);

    for (int i = 0; i < HEIGHT; i++) // Draw World
    {
        for (int j = 0; j < WIDTH; j++)
        {
            if (field[i][j] == 0)
                continue;

            cell.setFillColor(colorMap[(field[i][j] - 1) % 7]);
            cell.setPosition(Vector2f(tetrisBoard.getGlobalBounds().left + (j * CELL_SIZE), tetrisBoard.getGlobalBounds().top + (i - 1) * CELL_SIZE));
            window.draw(cell);
        }
    }

    DrawNextBlockPreview(window, tetrisBoard); // Maybe split code into GUI Draw and Game Draw?
    DrawHoldBlock(window, tetrisBoard);

    float UIRightXPos = tetrisBoard.getGlobalBounds().left + tetrisBoard.getGlobalBounds().width + (CELL_SIZE / 2);

    std::stringstream precisionValue; // Shows Game Time
    precisionValue << "Time: " << std::fixed << std::setprecision(0) << matchTime.getElapsedTime().asSeconds() << std::endl;
    DrawUIText(window, precisionValue.str(), Vector2f(UIRightXPos, tetrisBoard.getGlobalBounds().top + (CELL_SIZE * 6)), 23, 0);

    String textString = "Lines Cleared: " + std::to_string(linesClearedInAGame);
    DrawUIText(window, textString, Vector2f(tetrisBoard.getGlobalBounds().left, tetrisBoard.getGlobalBounds().top - (CELL_SIZE)), 23, 0);

    textString = "Score: " + std::to_string(score);
    DrawUIText(window, textString, Vector2f(UIRightXPos, tetrisBoard.getGlobalBounds().top + (CELL_SIZE * 7)), 23, 0);

    textString = "Level: " + std::to_string(speedLevel + 1);
    DrawUIText(window, textString, Vector2f(tetrisBoard.getGlobalBounds().left - CELL_SIZE , tetrisBoard.getGlobalBounds().top + (CELL_SIZE * 7)), 23, 1);

    if(bGameOver)
        DrawUIText(window, "Game Over", Vector2f(tetrisBoard.getGlobalBounds().left + (tetrisBoard.getGlobalBounds().width / 2), SCREENHEIGHT / 2), 50, 2);

    //window.draw(frame);
    window.display();
}

void Tetris::DrawNextBlockPreview(sf::RenderWindow& window, sf::RectangleShape tetrisBoard) // TODO - Break out into UI Class
{
    if (true) // Draws the preview for next block 
    {
        float previewPanelXPos = tetrisBoard.getGlobalBounds().left + tetrisBoard.getGlobalBounds().width + (CELL_SIZE / 2.5);
        float previewPanelYPos = tetrisBoard.getGlobalBounds().top + 30;
        Color previewColor = Color::White;
        RectangleShape previewPanel(Vector2f(3.5 * CELL_SIZE, 4.5 * CELL_SIZE));
        RectangleShape previewCell(Vector2f(CELL_SIZE, CELL_SIZE));

        previewPanel.setFillColor(Color::Black);
        previewPanel.setPosition(previewPanelXPos, previewPanelYPos);
        previewPanel.setOutlineThickness(1);
        previewPanel.setOutlineColor(Color::White);
        window.draw(previewPanel);

        float previewBlockXPos = 0;
        float previewBlockYPos = 0;
        switch (nextBlockType)
        {
        case 0: // I
        {
            previewBlockXPos = 10;
            previewBlockYPos = 9;
            break;
        }
        case 1: // S
        {
            previewBlockXPos = 25;
            previewBlockYPos = -7;
            break;
        }
        case 2: // Z
        {
            previewBlockXPos = 26;
            previewBlockYPos = -8;
            break;
        }
        case 3: // -|
        case 4: //  7
        case 5: // _|
        {
            previewBlockXPos = 25;
            previewBlockYPos = -7;
            break;
        }
        case 6: // O
        {
            previewBlockXPos = 27;
            previewBlockYPos = 7;
            break;
        }
        default:
            break;
        }

        for (int i = 0; i < 4; i++) // Draw Preview Block
        {
            int xPos = figures[nextBlockType][i] % 2 * CELL_SIZE;
            int yPos = figures[nextBlockType][i] / 2 * CELL_SIZE;

            previewCell.setFillColor(previewColor);
            previewCell.setPosition(previewPanel.getPosition().x + previewBlockXPos + xPos, previewPanel.getPosition().y + previewBlockYPos + yPos); // Displays the preview of the next block type
            window.draw(previewCell);
        }

        String textString = "Next Block:";
        DrawUIText(window, textString, Vector2f(previewPanel.getPosition().x + 10, previewPanel.getPosition().y - 25), 18, 0);
    }
}

void Tetris::DrawHoldBlock(sf::RenderWindow& window, sf::RectangleShape tetrisBoard)
{
    if (true) // Draws the preview for next block 
    {
        Color holdColor = Color::White;
        RectangleShape holdPanel(Vector2f(3.5 * CELL_SIZE, 4.5 * CELL_SIZE));
        RectangleShape holdCell(Vector2f(CELL_SIZE, CELL_SIZE));

        float holdPanelXPos = tetrisBoard.getGlobalBounds().left - holdPanel.getGlobalBounds().width - (CELL_SIZE / 2.5);
        float holdPanelYPos = tetrisBoard.getGlobalBounds().top + 30;

        holdPanel.setFillColor(Color::Black);
        holdPanel.setPosition(holdPanelXPos, holdPanelYPos);
        holdPanel.setOutlineThickness(1);
        holdPanel.setOutlineColor(Color::White);
        window.draw(holdPanel);

        if (holdTetromino.blocks[0].x != NULL)
        {
            float holdBlockXPos = 0;
            float holdBlockYPos = 0;
            switch (holdBlockType)
            {
            case 0: // I
            {
                holdBlockXPos = 10;
                holdBlockYPos = 9;
                break;
            }
            case 1: // S
            {
                holdBlockXPos = 25;
                holdBlockYPos = -7;
                break;
            }
            case 2: // Z
            {
                holdBlockXPos = 26;
                holdBlockYPos = -8;
                break;
            }
            case 3: // -|
            case 4: //  7
            case 5: // _|
            {
                holdBlockXPos = 25;
                holdBlockYPos = -7;
                break;
            }
            case 6: // O
            {
                holdBlockXPos = 27;
                holdBlockYPos = 7;
                break;
            }
            default:
                break;
            }

            for (int i = 0; i < 4; i++) // Draw Hold Block
            {
                int xPos = figures[holdBlockType][i] % 2 * CELL_SIZE;
                int yPos = figures[holdBlockType][i] / 2 * CELL_SIZE;

                holdCell.setFillColor(holdColor);
                holdCell.setPosition(holdPanel.getPosition().x + holdBlockXPos + xPos, holdPanel.getPosition().y + holdBlockYPos + yPos); // Displays the preview of the next block type
                window.draw(holdCell);
            }
        }

        String textString = "Hold Block:";
        DrawUIText(window, textString, Vector2f(holdPanel.getPosition().x + 10, holdPanel.getPosition().y - 25), 18, 0);
    }
}

void Tetris::DrawGridLines(sf::RenderWindow& window, RectangleShape back)
{
    auto left = back.getGlobalBounds().left;
    auto right = back.getGlobalBounds().left + back.getGlobalBounds().width;
    auto top = back.getGlobalBounds().top;
    auto bot = back.getGlobalBounds().top + back.getGlobalBounds().height;

    for (int i = 0; i < WIDTH + 1; i++)
    {
        sf::Vertex line[]
        {
            Vector2f(left + (i * CELL_SIZE), top),
            Vector2f(left + (i * CELL_SIZE), bot)
        };

        line[0].color = sf::Color(255, 255, 255, 50); // sets grid lines color
        line[1].color = sf::Color(255, 255, 255, 50);

        window.draw(line, 2, sf::Lines);
    }

    for (int i = 0; i < HEIGHT; i++)
    {
        sf::Vertex line[]
        {
            Vector2f(left, top + (i * CELL_SIZE)),
            Vector2f(right, top + (i * CELL_SIZE))
        };

        line[0].color = sf::Color(255, 255, 255, 50);
        line[1].color = sf::Color(255, 255, 255, 50);

        window.draw(line, 2, sf::Lines);
    }
}

void Tetris::DrawUIText(sf::RenderWindow& window, const String textString, Vector2f textPos, int textSize, int drawDirection = 0)
{
    sf::Font font;
    if (font.loadFromFile("Fonts/LiberationSans.ttf"))
    {
        sf::Text text;
        // select the font
        text.setFont(font); // font is a sf::Font
        // set the string to display
        text.setString(textString);
        // set the character size
        text.setCharacterSize(textSize); // in pixels, not points!
        // set the color
        text.setFillColor(sf::Color::White);
        text.setOutlineColor(sf::Color::Black);
        text.setOutlineThickness(1);
        // set the text style
        text.setStyle(sf::Text::Bold);
        // set pos
        sf:FloatRect boundingBox = text.getGlobalBounds();

        switch (drawDirection)
        {
            case 1: //right
            {
                text.setOrigin(boundingBox.width, 0);
                break;
            }
            case 2: //center
            {
                text.setOrigin(boundingBox.width / 2, 0);
                break;
            }
            default: // left
            {
                break;
            }
        }

        text.setPosition(textPos.x, textPos.y);
        // inside the main loop, between window.clear() and window.display()
        window.draw(text);
    }
}
