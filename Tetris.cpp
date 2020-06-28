#include "Tetris.h"

void Tetris::Game()
{
    srand(time(0));

    RenderWindow window(VideoMode(SCREENWIDTH, SCREENHEIGHT), "The Game!");
    sf::View view;
    view.reset(sf::FloatRect(0, 0, SCREENWIDTH, SCREENHEIGHT));
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

void Tetris::RunGameLoop(sf::Clock& clock, float& timer, sf::RenderWindow& window, float& delay, Clock matchTime)
{
    int dx = 0; bool rotate = 0; // dx is the direction to rotate

    timer += clock.getElapsedTime().asSeconds();
    clock.restart();

    GetPlayerInput(window, rotate, dx, timer, delay);

    if (!bGameOver)
    {
        if (bSpawnNextBlock)
            SpawnNextBlock();

        if (Keyboard::isKeyPressed(Keyboard::Down) && !bGoTillLocked) // add to score for every line the block gets soft dropped
            delay = 0.05;

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

        dx = 0; rotate = 0; delay = 0.6;
    }

    // Draw
    Draw(window, matchTime);

}

void Tetris::GetPlayerInput(sf::RenderWindow& window, bool& rotate, int& dx, float& timer, float& delay)
{
    Event e;
    while (window.pollEvent(e))
    {
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
                bGameOver = false;
                dx = 0;
                rotate = 0;
                delay = 0.3;
                score = 0;
                timer = 0;

                for (int i = 0; i < HEIGHT; i++)
                {
                    for (int j = 0; j < WIDTH; j++)
                    {
                        field[i][j] = 0;
                    }
                }
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

void Tetris::Rotate(bool& rotate)
{
    if (rotate && spawnedTetromino != NULL)
    {
        Point p = spawnedTetromino->blocks[1]; //center of rotation // TODO - rework rotation point on individual basis
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
        }

        PlaceBlockInField();

        bSwappedThisTurn = false;

        //SpawnNextBlock();

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

            //SpawnNextBlock(true);
        }
    }
    else if (timer > delay && spawnedTetromino != NULL)
    {
        backupTetromino = *spawnedTetromino;

        for (int i = 0; i < 4; i++)
        {
            spawnedTetromino->blocks[i].y += 1;
        }

        if (!Check(spawnedTetromino))
        {
            PlaceBlockInField();
            
            bSwappedThisTurn = false;
            bSpawnNextBlock = true;

            //SpawnNextBlock();
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

    //Timer gets reset everytime a new line is added. keeping it as a feature so that if you are fast enough you can stack line clears for large numbers
    if (waitTime.getElapsedTime().asSeconds() > 0.4 && completedLines.size() > 0) // time completes lines can stay on screen for
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
    RectangleShape back(Vector2f(WIDTH * CELL_SIZE, (HEIGHT - 1) * CELL_SIZE));
    window.clear(Color(100,100,100));

    back.setFillColor(Color::Black);
    //back.setOrigin(back.getLocalBounds().width / 2, back.getLocalBounds().height / 2);
    //back.setPosition(window.getSize().x / 2, window.getSize().y / 2);
    back.move(CELL_SIZE, CELL_SIZE * 1.8);
    window.draw(back);
    
    if (!bGameOver && spawnedTetromino != NULL)
    {
        RectangleShape previewBlock(Vector2f(CELL_SIZE, CELL_SIZE));
        Tetromino previewPosA = *spawnedTetromino;
        Tetromino previewPosB = *spawnedTetromino;
        while (Check(&previewPosA))
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
            previewBlock.setPosition(previewPosB.blocks[i].x * CELL_SIZE, previewPosB.blocks[i].y * CELL_SIZE);
            previewBlock.move(CELL_SIZE, 28);
            window.draw(previewBlock);
        }


        for (int i = 0; i < 4; i++) // Draw Current Block
        {
            cell.setFillColor(colorMap[spawnedTetromino->colorNum]);
            cell.setPosition(spawnedTetromino->blocks[i].x * CELL_SIZE, spawnedTetromino->blocks[i].y * CELL_SIZE);
            cell.move(CELL_SIZE, 28);
            window.draw(cell);

            //s.setTextureRect(IntRect(colorNum * 18, 0, 18, 18));
            //s.setPosition(a[i].x * 18, a[i].y * 18);
            //s.move(28, 31); //offset
            //window.draw(s);
        }
    }
    
    DrawGridLines(window, back);

    for (int i = 0; i < HEIGHT; i++) // Draw World
    {
        for (int j = 0; j < WIDTH; j++)
        {
            if (field[i][j] == 0)
                continue;

            cell.setFillColor(colorMap[(field[i][j] - 1) % 7]);
            cell.setPosition(Vector2f(j * CELL_SIZE, i * CELL_SIZE));
            cell.move(CELL_SIZE, 28);
            window.draw(cell);

            //s.setTextureRect(IntRect(field[i][j] * 18, 0, 18, 18));
            //s.setPosition(j * 18, i * 18);
            //s.move(28, 31); //offset
            //window.draw(s);
        }
    }

    DrawNextBlockPreview(window); // Maybe split code into GUI Draw and Game Draw?
    DrawHoldBlock(window);

    std::stringstream precisionValue; // Shows Game Time
    precisionValue << std::fixed << std::setprecision(0);
    precisionValue << "Time: " << matchTime.getElapsedTime().asSeconds() << std::endl;
    DrawUIText(window, precisionValue.str(), Vector2f(SCREENWIDTH - 125, CELL_SIZE * 2), 23, 0);

    String textString = "Lines Cleared: " + std::to_string(linesClearedInAGame);
    DrawUIText(window, textString, Vector2f(10, CELL_SIZE / 2), 23, 0);

    textString = "Score: " + std::to_string(score);
    DrawUIText(window, textString, Vector2f(SCREENWIDTH - 125, CELL_SIZE * 3), 23, 0);

    if(bGameOver)
        DrawUIText(window, "Game Over", Vector2f(SCREENWIDTH / 2, SCREENHEIGHT / 2), 50, 2);

    //window.draw(frame);
    window.display();
}

void Tetris::DrawNextBlockPreview(sf::RenderWindow& window)
{
    if (true) // Draws the preview for next block 
    {
        float previewPanelXPos = 132;
        float previewPanelYPos = 200;
        Color previewColor = Color::White;
        RectangleShape previewPanel(Vector2f(3.5 * CELL_SIZE, 4.5 * CELL_SIZE));
        RectangleShape previewCell(Vector2f(CELL_SIZE, CELL_SIZE));

        previewPanel.setFillColor(Color::Black);
        previewPanel.setPosition(SCREENWIDTH - previewPanelXPos, previewPanelYPos);
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

void Tetris::DrawHoldBlock(sf::RenderWindow& window)
{
    if (true) // Draws the preview for next block 
    {
        float holdPanelXPos = 132;
        float holdPanelYPos = 400;
        Color holdColor = Color::White;
        RectangleShape holdPanel(Vector2f(3.5 * CELL_SIZE, 4.5 * CELL_SIZE));
        RectangleShape holdCell(Vector2f(CELL_SIZE, CELL_SIZE));

        holdPanel.setFillColor(Color::Black);
        holdPanel.setPosition(SCREENWIDTH - holdPanelXPos, holdPanelYPos);
        holdPanel.setOutlineThickness(1);
        holdPanel.setOutlineColor(Color::White);
        window.draw(holdPanel);

        if (holdBlockType != -1)
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
    auto topLeft = back.getTransform().transformPoint(back.getPoint(0));
    auto topRight = back.getTransform().transformPoint(back.getPoint(1));
    auto botLeft = back.getTransform().transformPoint(back.getPoint(2));
    auto botRight = back.getTransform().transformPoint(back.getPoint(3));

    for (int i = 0; i < WIDTH + 1; i++)
    {
        sf::Vertex line[]
        {
            Vector2f(i * CELL_SIZE + CELL_SIZE, topRight.y),
            Vector2f(i * CELL_SIZE + CELL_SIZE, botRight.y)
        };

        line[0].color = sf::Color(255, 255, 255, 50); // sets grid lines color
        line[1].color = sf::Color(255, 255, 255, 50);

        window.draw(line, 2, sf::Lines);
    }

    for (int i = 0; i < HEIGHT; i++)
    {
        sf::Vertex line[]
        {
            Vector2f(topLeft.x, i * CELL_SIZE + (CELL_SIZE * 1.8)),
            Vector2f(topRight.x, i * CELL_SIZE + (CELL_SIZE * 1.8))
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
