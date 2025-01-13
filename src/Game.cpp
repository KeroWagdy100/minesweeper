// Game.cpp
#include <Game.h>

namespace game
{
    // Default Constructor (You have to call init to initialize game)
    Game::Game()
    {

    }
    // Convert Dimensions (1D to 2D)
    sf::Vector2u convertDim1To2(uint16_t oneDim, uint16_t width)
    {
        return {(unsigned int)(oneDim / width), (unsigned int)(oneDim % width)};
    }
    // Convert Dimensions (2D to 1D)
    uint16_t convertDim2To1(const sf::Vector2u& twoDim, uint16_t width)
    {
        return twoDim.y + twoDim.x * width;
    }

    // Init The Game (should be called before run)
    bool Game::init(const std::filesystem::path& _tilesetPath, uint16_t tileSize)
    {
        // Setup Game Fields
        gameFinished = false;
        gameStarted = false; // game starts only when player open his first tile
        flags = 0;

        // Setup Window
        window.create(sf::VideoMode({(unsigned int)(width * tileSize), (unsigned int)(height * tileSize)}), "Minesweeper" );
        window.setFramerateLimit(60);
        window.setIcon(sf::Image("res/png/mine-icon-256.png"));

        // Setup UI (Timer && N_MINES)
        if (!font.openFromFile("res/fonts/DS-DIGI.TTF"))
            return false;
        clock.reset();

        unsigned int textSize = window.getSize().x / 20;
        timerText.setFont(font);
        timerText.setCharacterSize(textSize);
        timerText.setPosition({10u, 5u});
        timerText.setFillColor(sf::Color::Black);
        timerText.setString(std::to_string(clock.getElapsedTime().asMilliseconds() * 1000));

        minesText.setFont(font);
        minesText.setCharacterSize(textSize);
        minesText.setPosition({10u, 35u});
        minesText.setFillColor(sf::Color::Red);
        minesText.setString("mines: " + std::to_string(mines - flags));
        ///////////////////////////////////////////

        // Setup Tiles
        tilesetPath = _tilesetPath;
        Tile tempTile;
        for (uint16_t i = 0; i < width * height; ++i)
        {
            tiles[i] = tempTile;
            mapIndices[i] = tempTile.getMapIndex();
        }
        this->tileSize = tileSize;

        
        // Generate level and update mapIndices 
        // moved to first tile click (in handleEvent) to gurantee that first click is not mine

        // generateLevel();
        // Generate Tilemap
        return tilemap.load(tilesetPath, {tileSize, tileSize}, mapIndices, width, height);
        // return true;
    }

    /**
     * @brief Game/Main Loop
     * 
     * @return true if game finishes before player closing window
     * @return false if player closes window
     */
    bool Game::run()
    {
        while (window.isOpen())
        {
            // handle events
            while (const std::optional event = window.pollEvent())
                handleEvent(event);

            // window drawing
            window.clear();
            window.draw(tilemap);
            // update UI 
            if (!gameFinished && clock.isRunning())
                timerText.setString(std::to_string(clock.getElapsedTime().asMilliseconds() / 1000));
            
            // close game after delay 3.5s from game finish
            // note that clock is restarted at endGame()
            else if (gameFinished && clock.getElapsedTime().asMilliseconds() >= 3500)
            {
                window.close();
                return true;
            }
            window.draw(timerText);
            window.draw(minesText);
            window.display();
        }
        // if user closed window before game finish
        return false;
        
    }

    /**
     * @brief puts mines in random tiles and updating their neighbours counter
     * 
     */
    void Game::generateLevel(uint16_t firstClickTileIndex)
    {
        srand(time(0));
        uint16_t size = width * height;

        // filling tiles with mines in random positions
        for (uint16_t mine = 0; mine < mines; ++mine)
        {
            uint16_t mineIndex;
            do
            {
                mineIndex = rand() % size;
                // find another position if position is on firstClicked tile or mine already exist
            } while (firstClickTileIndex == mineIndex|| tiles[mineIndex].m_isMine); 

            tiles[mineIndex].m_isMine = true;
            // Update neighbours' counter
            Tile* neighbours[8];
            auto counter = getNeighbours8(mineIndex, neighbours);
            for (uint16_t i = 0; i < counter; i++)
                neighbours[i]->m_mineCounter++;
        }
        
        // map tiles to their indices on tilemap
        for (uint16_t i = 0; i < width * height; i++)
            mapIndices[i] = tiles[i].getMapIndex();
    }

    /**
     * @brief update one tile's state
     * 
     * @param tileIndex1D index in 1Dim tiles array
     * @param state tile's new state
     */
    void Game::updateTile(uint16_t tileIndex1D, const TileState& state)
    {
        tiles[tileIndex1D].m_state = state;
        auto tileIndex2D = convertDim1To2(tileIndex1D, width);
        tilemap.updateTile(tileIndex2D.x, tileIndex2D.y, tiles[tileIndex1D].getMapIndex());
    }

    /**
     * @brief update one tile's state
     * 
     * @param tilePtr  tile pointer
     * @param state tile's new state
     */
    void Game::updateTile(Tile* tilePtr, const TileState& state)
    {
        if (tilePtr == nullptr)
            return;
        tilePtr->m_state = state;
        uint16_t tileIndex1D = tilePtr - tiles;
        auto tileIndex2D = convertDim1To2(tileIndex1D, width);
        tilemap.updateTile(tileIndex2D.x, tileIndex2D.y, tiles[tileIndex1D].getMapIndex());
    }


    /**
     * @brief Get array of pointers to neighbours of one tile
     * 
     * @param tileIndex1D index in 1Dim tiles array
     * @param neighbours array of pointers (PLEASE MAKE SURE THAT THIS IS ALLOCATED IN MEMORY)
     * @return uint16_t number of neighbours found (max: 8)
     * Neighbours can be [down left, left, up left, up, up right, right, down right, down]
     */
    uint16_t Game::getNeighbours8(const uint16_t tileIndex1D, Tile** neighbours)
    {
        sf::Vector2u tileIndex2D = convertDim1To2(tileIndex1D, width);
        auto& i = tileIndex2D.x;
        auto& j = tileIndex2D.y;
        uint16_t counter = 0;
        if (i != height - 1)
        {
            neighbours[counter] = &tiles[tileIndex1D + width];
            counter++;
            if (j != width - 1)
            {
                neighbours[counter] = &tiles[tileIndex1D + width + 1]; // down-right
                counter++;
            }
            if (j != 0)
            {
                neighbours[counter] = &tiles[tileIndex1D + width - 1]; // down-left
                counter++;
            }
        }
        if (i != 0)
        {
            neighbours[counter] = &tiles[tileIndex1D - width]; // up
            counter++;
            if (j != 0)
            {
                neighbours[counter] = &tiles[tileIndex1D - width - 1]; // up-left
                counter++;
            }
            if (j != width - 1)
            {
                neighbours[counter] = &tiles[tileIndex1D - width + 1]; // up-right
                counter++;
            }
        }
        if (j != width - 1)
        {
            neighbours[counter] = &tiles[tileIndex1D + 1]; // right
            counter++;
        }
        if (j != 0)
        {
            neighbours[counter] = &tiles[tileIndex1D - 1]; // left
            counter++;
        }
        return counter;
    }

    /**
     * @brief Get array of pointers to hidden-close neighbours of one tile
     * a hidden neighbour: is a tile whose state is TileState::hidden
     * a close neighbour can be [down, left, up, right]
     * @param tileIndex1D index in 1Dim tiles array
     * @param neighbours array of pointers (PLEASE MAKE SURE THAT THIS IS ALLOCATED IN MEMORY)
     * @return uint16_t number of hidden-close neighbours found (max: 4)
     */
    uint16_t Game::getHiddenNeighbours4(const uint16_t tileIndex1D, Tile** neighbours)
    {
        sf::Vector2u tileIndex2D = convertDim1To2(tileIndex1D, width);
        auto& i = tileIndex2D.x;
        auto& j = tileIndex2D.y;
        uint16_t counter = 0;
        if (i != height - 1)
        {
            Tile* neighbour = &tiles[tileIndex1D + width];
            if (neighbour->m_state == TileState::hidden || neighbour->m_state == TileState::peek)
                neighbours[counter++] = neighbour; // down
        }

        if (i != 0)
        {
            Tile* neighbour = &tiles[tileIndex1D - width];
            if (neighbour->m_state == TileState::hidden || neighbour->m_state == TileState::peek)
                neighbours[counter++] = neighbour; // up
        }

        if (j != width - 1)
        {
            Tile* neighbour = &tiles[tileIndex1D + 1];
            if (neighbour->m_state == TileState::hidden || neighbour->m_state == TileState::peek)
                neighbours[counter++] = neighbour; // right
        }
        if (j != 0)
        {
            Tile* neighbour = &tiles[tileIndex1D - 1];
            if (neighbour->m_state == TileState::hidden || neighbour->m_state == TileState::peek)
                neighbours[counter++] = neighbour; // left
        }
        return counter;
    }

    void Game::handleEvent(const std::optional<sf::Event>& event)
    {
        static bool wasPeeking = false;
        static uint16_t tilePeekedIndex1D = 0;
        // last hovered tile
        static uint16_t hoveredIndex = 0;

        if (event->is<sf::Event::Closed>())
            window.close();

        // PEEK ON HOVER -- Only For Debugging purposes
        // else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>())
        // {
        //     uint16_t col, row;
        //     {
        //         auto tileIndex2D = tileIndexFromScreenPos(mouse->position);
        //         col = tileIndex2D.y;
        //         row = tileIndex2D.x;
        //     }
        //     // hover on different tile
        //     if (hoveredIndex != col + row * width)
        //     {
        //         // hide prev hovered tile 
        //         auto hoveredIndex2D = convertDim1To2(hoveredIndex, width);
        //         if (tiles[hoveredIndex].m_state == TileState::hidden)
        //             tilemap.updateTile(hoveredIndex2D.x, hoveredIndex2D.y, mapIndex::hidden);
        //         // update current hovered tile
        //         hoveredIndex = col + row * width;
        //         Tile temp = tiles[hoveredIndex];
        //         temp.m_state = TileState::notHidden;
        //         tilemap.updateTile(row, col, temp.getMapIndex());
        //     }
        // }

        else if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>())
        {
            if (wasPeeking)
            {
                Tile* neighborus[8];
                auto counter = getNeighbours8(tilePeekedIndex1D, neighborus);
                for (uint16_t i = 0; i < counter; i++)
                {
                    if (neighborus[i]->m_state == TileState::peek)
                        updateTile(neighborus[i], TileState::hidden);
                }
                wasPeeking = false;
            }
        }

        else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
        {
            // start timer whenever the player presses on any position
            if (!clock.isRunning())
                clock.start();

            
            // which button is pressed
            bool left = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            bool right = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);

            auto tileIndex1D = tileIndexFromScreenPos(mouse->position);

            // Both Buttons Clicked (Peeking Neighbours)
            if (left && right)
            {
                // to peek neighbours of a tile, the tile must be not-hidden
                if (tiles[tileIndex1D].m_state != TileState::notHidden)
                    return;

                Tile* neighbours[8];
                auto counter = getNeighbours8(tileIndex1D, neighbours);

                uint16_t flagCounter = 0;
                bool flagNotOnMine = false;

                // peek on hidden neighbours
                for (uint16_t i = 0; i < counter; i++)
                {
                    if (neighbours[i]->m_state == TileState::hidden)
                        updateTile(neighbours[i], TileState::peek);

                    // check neighbours with flags
                    else if (neighbours[i]->m_state == TileState::flagged)
                    {
                        flagCounter++;
                        // if player puts a flag on a not-mined tile
                        // there's possibility that player loses
                        if (!(neighbours[i]->m_isMine))
                            flagNotOnMine = true;
                    }
                }

                // no. of neighbours with flags >= tile number itself
                // then player is not peeking, but opening all neighbours
                if (flagCounter == tiles[tileIndex1D].m_mineCounter)
                {
                    // a flag was on wrong tile, and the player tries to open
                    // the tile, then player loses.
                    if (flagNotOnMine)
                    {
                        endGame(false);
                        return;
                    }

                    // opening non-mined neighbours
                    // since player guessed the mines by putting flags correct
                    for (uint16_t i = 0; i < counter; i++)
                    {
                        Tile* neighbour = neighbours[i];
                        // if neighbour is empty, unhide all neighbour's neighbours !
                        if (neighbour->m_mineCounter == 0 && !neighbour->m_isMine)
                            unhideEmptyNeighbours(neighbours[i] - tiles);

                        else if (!neighbour->m_isMine)
                            updateTile(neighbours[i], TileState::notHidden);
                    }
                }
                // player was just peeking neighbours (not to open them)
                else
                {
                    wasPeeking = true;
                    tilePeekedIndex1D = tileIndex1D;
                }
            }

            // Left Button Clicked (opening/unhiding tile)
            else if (left)
            {
                // Player opens first tile
                if (!gameStarted)
                {
                    generateLevel(tileIndex1D);
                    if (!tilemap.load(tilesetPath, {tileSize, tileSize}, mapIndices, width, height))
                    {
                        gameFinished = true;
                        return;
                    }
                    gameStarted = true;
                }
                // ignore clicking on already-openned tiles
                else if (tiles[tileIndex1D].m_state == TileState::notHidden)
                    return;
                // player opens a mine -> Loses
                else if (tiles[tileIndex1D].m_isMine)
                {
                    updateTile(tileIndex1D, TileState::mineClicked);
                    endGame(false);
                    return;
                }
                // player opens a non-mined tile
                updateTile(tileIndex1D, TileState::notHidden);
                // player opens an empty tile
                if (tiles[tileIndex1D].m_mineCounter == 0)
                    unhideEmptyNeighbours(tileIndex1D);
            }

            // Right Button Clicked (setting/unsetting flag)
            else if (right)
            {
                // ignore right-clicking on openned tile
                if (tiles[tileIndex1D].m_state == TileState::notHidden)
                    return;

                // unsetting a flag
                if (tiles[tileIndex1D].m_state == TileState::flagged)
                {
                    updateTile(tileIndex1D, TileState::hidden);
                    flags--;
                    minesText.setString("mines: " + std::to_string(mines - flags));
                    return;
                }

                // setting a flag
                updateTile(tileIndex1D, TileState::flagged);
                flags++;
                minesText.setString("mines: " + std::to_string(mines - flags));

                // if player uses all their flags - endGame
                // if all flags on all mines, then win, else lose
                if (flags == mines)
                    endGame(checkWin());
            }
        }
    }

    /**
     * @brief opens/unhides non-mined neighbours of a given tile
     * 
     * @param index1D index in 1Dim tiles array
     */
    void Game::unhideEmptyNeighbours(uint16_t index1D)
    {
        Tile* neighbours[4];
        uint16_t count = getHiddenNeighbours4(index1D, neighbours);
        
        for (uint16_t i = 0; i < count; ++i)
        {
            // don't open mined neighbours
            if (neighbours[i]->m_isMine)
                continue;
            
            updateTile(neighbours[i], TileState::notHidden);

            // recursively unhide empty neighbours if current
            // negihbour is empty (has no mined neighbour)
            if (neighbours[i]->m_mineCounter == 0)
                unhideEmptyNeighbours(neighbours[i] - tiles);
        }
    }

    /**
     * @brief check if player win 
     * 
     * @return false if any tile with a flag is not mined or vice versa
     * @return true when all mined tiles are flagged and vice versa
     */
    bool Game::checkWin()
    {
        for (uint16_t i = 0; i < width * height; ++i)
        {
            bool isMined = tiles[i].m_isMine;
            bool isFlagged = tiles[i].m_state == TileState::flagged;
            if (isMined && !isFlagged || !isMined && isFlagged)
                return false;
        }
        return true;
    }

    /**
     * @brief Ends the game, opens all hidden-mines and resets clock/timer
     * 
     * @param userWon whether user won or not
     */
    void Game::endGame(bool userWon)
    {
        // resetting timer and game state
        gameFinished = true;
        clock.restart();

        std::cout << (userWon ? "win" : "lose") << "\n";

        // open all mines to let player know where were the mines
        for (uint16_t i = 0; i < width * height; ++i)
        {
            if (tiles[i].m_state != TileState::mineClicked && tiles[i].m_isMine)
                updateTile(i, TileState::notHidden);
        }
    }

    /**
     * @brief returns tileIndex in 1-D array of given position on screen
     * 
     * @param screenPos screen position (according to sfml)
     * @return uint16_t index of tile in 1D array 
     */
    uint16_t Game::tileIndexFromScreenPos(const sf::Vector2i& screenPos)
    {
        const auto& x = screenPos.x;
        const auto& y = screenPos.y;
        const auto windowSize = window.getSize();
        if (x < 0 || y < 0 || x >= windowSize.x || y >= windowSize.y)
            return {};
        const auto currTileSize = sf::Vector2u(windowSize.y / width, windowSize.x / height);
        sf::Vector2u index2d = sf::Vector2u(y / currTileSize.x, x / currTileSize.y);
        return convertDim2To1(index2d, width);
    }

};