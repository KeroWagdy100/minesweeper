// Game.h
///////////////////////////////////////////
#pragma once

#include <SFML/Graphics.hpp>
#include <Tile.h>
#include <algorithm>
#include <iostream>

template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    os << "[";
    for (size_t i = 0; i < v.size(); ++i)
        os << v.at(i) << (i == v.size() - 1 ? "" : ", ");
    os << "]";
    return os;
}

namespace game
{
    // Convert Dimensions (1D to 2D)
    sf::Vector2u convertDim1To2(uint16_t oneDim, uint16_t width)
    {
        return {oneDim / width, oneDim % width};
    }
    // Convert Dimensions (2D to 1D)
    uint16_t convertDim2To1(const sf::Vector2u& twoDim, uint16_t width)
    {
        return twoDim.y + twoDim.x * width;
    }

    class Game
    {
    public:
        // Default Constructor (You have to call init to initialize game)
        Game()
        {
        }

        bool init(const std::filesystem::path& tilesetPath)
        {
            // Setup window
            window.setFramerateLimit(60);
            // Setup UI (Timer && N_MINES)
            clock.reset();

            timerText.setPosition({10u, 5u});
            timerText.setFillColor(sf::Color::Red);
            timerText.setString(std::to_string(clock.getElapsedTime().asMilliseconds() * 1000));

            minesText.setPosition({10u, 35u});
            minesText.setFillColor(sf::Color::Red);
            minesText.setString(std::to_string(mines - flags));
            ///////////////////////////////////////////

            // Generate level and update mapIndices
            generateLevel();
            // Generate Tilemap
            return tilemap.load(tilesetPath, {tileSize, tileSize}, mapIndices, width, height);
        }

        // Game/Main Loop
        void run()
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
                
                // close game after 5s from game finish
                // note that clock is restarted at endGame()
                else if (gameFinished && clock.getElapsedTime().asMilliseconds() >= 5000)
                    break;
                window.draw(timerText);
                window.draw(minesText);
                window.display();
            }
            
        }

    private:
        /**
         * @brief puts mines in random tiles and updating their neighbours counter
         * 
         */
        void generateLevel()
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
                    // find another position if mine already exist
                } while (tiles[mineIndex].m_isMine); 

                tiles[mineIndex].m_isMine = true;
                // Update neighbours' counter
                Tile* neighbours[8];
                auto counter = getNeighbours8(mineIndex, neighbours);
                for (uint16_t i = 0; i < counter; i++)
                    neighbours[i]->m_mineCounter++;
            }
            
            // map tiles to their index on tilemap
            for (uint16_t i = 0; i < width * height; i++)
                mapIndices[i] = tiles[i].getMapIndex();
        }

    
        /**
         * @brief update one tile's state
         * 
         * @param tileIndex1D index in 1Dim tiles array
         * @param state tile's new state
         */
        void updateTile(uint16_t tileIndex1D, const Tile::state& state)
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
        void updateTile(Tile* tilePtr, const Tile::state& state)
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
        uint16_t getNeighbours8(const uint16_t tileIndex1D, Tile** neighbours)
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
         * a hidden neighbour: is a tile whose state is Tile::state::hidden
         * a close neighbour can be [down, left, up, right]
         * @param tileIndex1D index in 1Dim tiles array
         * @param neighbours array of pointers (PLEASE MAKE SURE THAT THIS IS ALLOCATED IN MEMORY)
         * @return uint16_t number of hidden-close neighbours found (max: 4)
         */
        uint16_t getHiddenNeighbours4(const uint16_t tileIndex1D, Tile** neighbours)
        {
            sf::Vector2u tileIndex2D = convertDim1To2(tileIndex1D, width);
            auto& i = tileIndex2D.x;
            auto& j = tileIndex2D.y;
            uint16_t counter = 0;
            if (i != height - 1)
            {
                Tile* neighbour = &tiles[tileIndex1D + width];
                if (neighbour->m_state == Tile::state::hidden || neighbour->m_state == Tile::state::peek)
                    neighbours[counter++] = neighbour; // down
            }

            if (i != 0)
            {
                Tile* neighbour = &tiles[tileIndex1D - width];
                if (neighbour->m_state == Tile::state::hidden || neighbour->m_state == Tile::state::peek)
                    neighbours[counter++] = neighbour; // up
            }

            if (j != width - 1)
            {
                Tile* neighbour = &tiles[tileIndex1D + 1];
                if (neighbour->m_state == Tile::state::hidden || neighbour->m_state == Tile::state::peek)
                    neighbours[counter++] = neighbour; // right
            }
            if (j != 0)
            {
                Tile* neighbour = &tiles[tileIndex1D - 1];
                if (neighbour->m_state == Tile::state::hidden || neighbour->m_state == Tile::state::peek)
                    neighbours[counter++] = neighbour; // left
            }
            return counter;
        }

        void handleEvent(const std::optional<sf::Event>& event)
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
            //         if (tiles[hoveredIndex].m_state == Tile::state::hidden)
            //             tilemap.updateTile(hoveredIndex2D.x, hoveredIndex2D.y, mapIndex::hidden);
            //         // update current hovered tile
            //         hoveredIndex = col + row * width;
            //         Tile temp = tiles[hoveredIndex];
            //         temp.m_state = Tile::state::notHidden;
            //         tilemap.updateTile(row, col, temp.getMapIndex());
            //     }
            // }

            // else if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>())
            // {
            //     if (wasPeeking)
            //     {
            //         Tile* neighborus[8];
            //         auto counter = getNeighbours8(tilePeekedIndex1D, neighborus);
            //         for (uint16_t i = 0; i < counter; i++)
            //         {
            //             if (neighborus[i]->m_state == Tile::state::peek)
            //                 updateTile(neighborus[i], Tile::state::hidden);
            //         }
            //         wasPeeking = false;
            //     }
            // }
            // PEEK ON HOVER END

            else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
            {
                // start timer whenever the user presses on any position
                if (!clock.isRunning())
                    clock.start();
                
                // which button is pressed
                bool left = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
                bool right = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);

                // auto tileIndex2D = tileIndexFromScreenPos(mouse->position);
                auto tileIndex1D = tileIndexFromScreenPos(mouse->position);

                // Both Buttons Clicked (Peeking Neighbours)
                if (left && right)
                {
                    // to peek neighbours of a tile, the tile must be not-hidden
                    if (tiles[tileIndex1D].m_state != Tile::state::notHidden)
                        return;

                    Tile* neighbours[8];
                    auto counter = getNeighbours8(tileIndex1D, neighbours);

                    uint16_t flagCounter = 0;
                    bool flagNotOnMine = false;

                    // peek on hidden neighbours
                    for (uint16_t i = 0; i < counter; i++)
                    {
                        if (neighbours[i]->m_state == Tile::state::hidden)
                            updateTile(neighbours[i], Tile::state::peek);

                        // check neighbours with flags
                        else if (neighbours[i]->m_state == Tile::state::flagged)
                        {
                            flagCounter++;
                            // if user puts a flag on a not-mined tile
                            // there's possibility that user loses
                            if (!(neighbours[i]->m_isMine))
                                flagNotOnMine = true;
                        }
                    }

                    // no. of neighbours with flags >= tile number itself
                    // then user is not peeking, but opening all neighbours
                    if (flagCounter == tiles[tileIndex1D].m_mineCounter)
                    {
                        // a flag was on wrong tile, and the user tries to open
                        // the tile, then user loses.
                        if (flagNotOnMine)
                        {
                            endGame(false);
                            return;
                        }

                        // opening non-mined neighbours
                        // since user guessed the mines by putting flags correct
                        for (uint16_t i = 0; i < counter; i++)
                        {
                            Tile* neighbour = neighbours[i];
                            // if neighbour is empty, unhide all neighbour's neighbours !
                            if (neighbour->m_mineCounter == 0 && !neighbour->m_isMine)
                                unhideEmptyNeighbours(neighbours[i] - tiles);

                            else if (!neighbour->m_isMine)
                                updateTile(neighbours[i], Tile::state::notHidden);
                        }
                    }
                    // PEEK ON HOVER
                    // else
                    // {
                    //     wasPeeking = true;
                    //     tilePeekedIndex1D = tileIndex1D;
                    // }
                    // END OF PEEK ON HOVER
                }

                // Left Button Clicked (opening/unhiding tile)
                else if (left)
                {
                    // ignore clicking on already-openned tiles
                    if (tiles[tileIndex1D].m_state == Tile::state::notHidden)
                        return;
                    // user opens a mine -> Loses
                    if (tiles[tileIndex1D].m_isMine)
                    {
                        updateTile(tileIndex1D, Tile::state::mineClicked);
                        endGame(false);
                    }
                    else
                    {
                        updateTile(tileIndex1D, Tile::state::notHidden);
                        // user opens an empty tile
                        if (tiles[tileIndex1D].m_mineCounter == 0)
                            unhideEmptyNeighbours(tileIndex1D);
                    }
                }

                // Right Button Clicked (setting/unsetting flag)
                else if (right)
                {
                    // ignore right-clicking on openned tile
                    if (tiles[tileIndex1D].m_state == Tile::state::notHidden)
                        return;

                    // unsetting a flag
                    if (tiles[tileIndex1D].m_state == Tile::state::flagged)
                    {
                        updateTile(tileIndex1D, Tile::state::hidden);
                        flags--;
                        minesText.setString(std::to_string(mines - flags));
                        return;
                    }

                    // setting a flag
                    updateTile(tileIndex1D, Tile::state::flagged);
                    flags++;
                    minesText.setString(std::to_string(mines - flags));

                    // if user uses all their flags - endGame
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
        void unhideEmptyNeighbours(uint16_t index1D)
        {
            Tile* neighbours[4];
            uint16_t count = getHiddenNeighbours4(index1D, neighbours);
            
            for (uint16_t i = 0; i < count; ++i)
            {
                // don't open mined neighbours
                if (neighbours[i]->m_isMine)
                    continue;
                
                updateTile(neighbours[i], Tile::state::notHidden);

                // recursively unhide empty neighbours if current
                // negihbour is empty (has no mined neighbour)
                if (neighbours[i]->m_mineCounter == 0)
                    unhideEmptyNeighbours(neighbours[i] - tiles);
            }
        }

        /**
         * @brief check if user win 
         * 
         * @return false if any tile with a flag is not mined or vice versa
         * @return true when all mined tiles are flagged and vice versa
         */
        bool checkWin()
        {
            for (uint16_t i = 0; i < width * height; ++i)
            {
                bool isMined = tiles[i].m_isMine;
                bool isFlagged = tiles[i].m_state == Tile::state::flagged;
                if (isMined && !isFlagged || !isMined && isFlagged)
                    return false;
            }
            return true;
        }

        void endGame(bool userWon)
        {
            // resetting timer and game state
            gameFinished = true;
            clock.restart();

            std::cout << (userWon ? "win" : "lose") << "\n";

            // open all mines to let user know where were the mines
            for (uint16_t i = 0; i < width * height; ++i)
            {
                if (tiles[i].m_state != Tile::state::mineClicked && tiles[i].m_isMine)
                    updateTile(i, Tile::state::notHidden);
            }
        }

        /**
         * @brief returns tileIndex in 1-D array of given position on screen
         * 
         * @param screenPos 
         * @return sf::Vector2u 
         */
        static uint16_t tileIndexFromScreenPos(const sf::Vector2i& screenPos)
        {
            const auto& x = screenPos.x;
            const auto& y = screenPos.y;
            if (x < 0 || y < 0 || x >= width * tileSize || y >= height * tileSize)
                return {};
            sf::Vector2u index2d = sf::Vector2u(y / tileSize, x / tileSize);
            return convertDim2To1{index2d, width};
        }

    private:
        // Static Data
        static const uint16_t width = 9u; // 2-bytes
        static const uint16_t height = 9u; // 2-bytes
        static const uint16_t tileSize = 64u; // 2-bytes
        ////////////////////////////////////////////////

        // Member Fields
        uint16_t mines = 10; // TODO: change number of mines 
        uint16_t flags = 0; 
        sf::RenderWindow window { sf::VideoMode({width * tileSize, height * tileSize}), "Minesweeper" }; // unknown-bytes
        TileMap tilemap;
        Tile tiles[width * height]; // 9 * 9 * 4 = 324-bytes
        uint16_t mapIndices[width * height]; // 9 * 9 * 2 = 162-bytes
        sf::Clock clock;
        sf::Font font = sf::Font("res/fonts/DS-DIGI.TTF");
        sf::Text timerText = sf::Text(font, "00");
        sf::Text minesText = sf::Text(font, "");
        bool gameFinished = false;
        ////////////////////////////////////////////////
    };
};