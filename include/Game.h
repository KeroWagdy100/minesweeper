// Game.h
///////////////////////////////////////////
#pragma once

#include <SFML/Graphics.hpp>
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
    sf::Vector2u convertD12(uint16_t oneDim, uint16_t width)
    {
        return {oneDim / width, oneDim % width};
    }
    // Convert Dimensions (2D to 1D)
    template <class T>
    T convertD21(const sf::Vector2<T>& twoDim, uint16_t width)
    {
        return twoDim.y + twoDim.x * width;
    }
    enum mapIndex {
        empty = 0, one, two, three, four, five, six, seven, eight,
        hidden, flag, mine, mineClicked
    };

    // should be 4-bytes
    struct Tile 
    {
    public:
        enum class state : char // 1-byte
        {
            hidden, notHidden, flagged, peek, mineClicked
        };
        short m_mineCounter = 0; // 2-bytes
        bool  m_isMine = 0; // 1-byte
        state m_state = state::hidden; // 1-byte // TODO: Let Init State = Hidden

        mapIndex getMapIndex() const {
            return getMapIndex(*this);
        }
        static mapIndex getMapIndex(const Tile& tile)
        {
            const auto& st = tile.m_state;
            const auto& isMine = tile.m_isMine;
            const auto& mineCounter = tile.m_mineCounter;
            if (st == state::hidden)
                return mapIndex::hidden;

            else if (st == state::flagged)
                return mapIndex::flag;

            else if (st == state::peek)
                return mapIndex::empty;

            else if (st == state::mineClicked && isMine)
                return mapIndex::mineClicked;

            else if (isMine)
                return mapIndex::mine;

            else
                return static_cast<mapIndex>(mineCounter);
        }
    };

    

    class Game
    {
    public:
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
            // Generate new level
            generateLevel();
            // map level to tileset indices
            updateMapIndices();
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
        void generateLevel()
        {
            srand(time(0));
            uint16_t size = width * height;

            for (uint16_t mine = 0; mine < mines; ++mine)
            {
                uint16_t mineIndex;
                do
                {
                    mineIndex = rand() % size;
                } while (tiles[mineIndex].m_isMine);
                tiles[mineIndex].m_isMine = true;

                // Update neighbours' counter
                Tile* neighbours[8];
                auto counter = getNeighbours8(mineIndex, neighbours);
                for (uint16_t i = 0; i < counter; i++)
                    neighbours[i]->m_mineCounter++;

            }
        }

        void updateMapIndices()
        {
            for (uint16_t i = 0; i < width * height; i++)
                mapIndices[i] = tiles[i].getMapIndex();
        }
    
        void updateTile(uint16_t tileIndex1D, const Tile::state& state)
        {
            tiles[tileIndex1D].m_state = state;
            auto tileIndex2D = convertD12(tileIndex1D, width);
            tilemap.updateTile(tileIndex2D.x, tileIndex2D.y, tiles[tileIndex1D].getMapIndex());
        }
        void updateTiles(const std::vector<uint16_t>& cols, uint16_t row, const Tile::state& state)
        {
            for (uint16_t i = 0; i < cols.size(); ++i)
            {
                uint16_t tileIndex1D = convertD21(sf::Vector2u(row, cols.at(i)), width);
                updateTile(tileIndex1D, state);
            }
        }

        void updateTile(Tile* tilePtr, const Tile::state& state)
        {
            if (tilePtr == nullptr)
                return;
            tilePtr->m_state = state;
            uint16_t tileIndex1D = tilePtr - tiles;
            auto tileIndex2D = convertD12(tileIndex1D, width);
            tilemap.updateTile(tileIndex2D.x, tileIndex2D.y, tiles[tileIndex1D].getMapIndex());
        }


        // Fills given array of pointers to all negihbours and returns its size
        // (Max Number Of Neighbours is 8)
        // Neighbours are [down left, left, up left, up, up right, right, down right, down]
        uint16_t getNeighbours8(const uint16_t tileIndex1D, Tile** neighbours)
        {
            sf::Vector2u tileIndex2D = convertD12(tileIndex1D, width);
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

        // Fills given array of pointers to close hidden negihbours only and returns its size
        // (Max Number Of Neighbours is 4)
        // Close Neighbours are [left, right, up, down]
        uint16_t getHiddenNeighbours4(const uint16_t tileIndex1D, Tile** neighbours)
        {
            sf::Vector2u tileIndex2D = convertD12(tileIndex1D, width);
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

            // else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>())
            // {

            //     // PEEK ON HOVER // Only For Debugging purposes
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
            //         auto hoveredIndex2D = convertD12(hoveredIndex, width);
            //         if (tiles[hoveredIndex].m_state == Tile::state::hidden)
            //             tilemap.updateTile(hoveredIndex2D.x, hoveredIndex2D.y, mapIndex::hidden);
            //         // update current hovered tile
            //         hoveredIndex = col + row * width;
            //         Tile temp = tiles[hoveredIndex];
            //         temp.m_state = Tile::state::notHidden;
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
                        if (neighborus[i]->m_state == Tile::state::peek)
                            updateTile(neighborus[i], Tile::state::hidden);
                    }
                    wasPeeking = false;
                }
            }
            else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (!clock.isRunning())
                    clock.start();
                bool left = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
                bool right = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
                // Both Buttons Clicked (Peeking Neighbours)
                auto tileIndex1D = convertD21(tileIndexFromScreenPos(mouse->position), width);
                auto tileIndex2D = tileIndexFromScreenPos(mouse->position);
                if (left && right)
                {
                    if (tiles[tileIndex1D].m_state != Tile::state::notHidden)
                        return;
                    Tile* neighbours[8];
                    auto counter = getNeighbours8(tileIndex1D, neighbours);
                    uint16_t flagCounter = 0;
                    bool flagNotOnMine = false;

                    for (uint16_t i = 0; i < counter; i++)
                    {
                        if (neighbours[i]->m_state == Tile::state::hidden)
                            updateTile(neighbours[i], Tile::state::peek);
                        else if (neighbours[i]->m_state == Tile::state::flagged)
                        {
                            flagCounter++;
                            if (!(neighbours[i]->m_isMine))
                                flagNotOnMine = true;
                        }
                    }
                    if (flagCounter == tiles[tileIndex1D].m_mineCounter)
                    {
                        if (flagNotOnMine)
                            endGame(false);
                        else
                        {
                            for (uint16_t i = 0; i < counter; i++)
                            {
                                Tile* neighbour = neighbours[i];
                                if (neighbour->m_mineCounter == 0 && !neighbour->m_isMine)
                                    unhideEmptyNeighbours(neighbours[i] - tiles);
                                else if (!neighbour->m_isMine)
                                    updateTile(neighbours[i], Tile::state::notHidden);
                            }
                        }
                    }
                    else
                    {
                        wasPeeking = true;
                        tilePeekedIndex1D = tileIndex1D;
                    }
                }
                else if (left)
                {
                    if (tiles[tileIndex1D].m_isMine)
                    {
                        updateTile(tileIndex1D, Tile::state::mineClicked);
                        endGame(false);
                    }
                    else
                    {
                        updateTile(tileIndex1D, Tile::state::notHidden);
                        if (tiles[tileIndex1D].m_mineCounter == 0)
                            unhideEmptyNeighbours(tileIndex1D);

                    }
                }
                else if (right)
                {
                    if (tiles[tileIndex1D].m_state == Tile::state::hidden)
                    {
                        updateTile(tileIndex1D, Tile::state::flagged);
                        flags++;
                        minesText.setString(std::to_string(mines - flags));
                        if (flags == mines)
                            endGame(true);
                    }
                    else if (tiles[tileIndex1D].m_state == Tile::state::flagged)
                    {
                        updateTile(tileIndex1D, Tile::state::hidden);
                        flags--;
                        minesText.setString(std::to_string(mines - flags));
                    }
                    
                }
                

            }
        }


        void unhideEmptyNeighbours(uint16_t index1D)
        {
            Tile* neighbours[4];
            
            uint16_t count = getHiddenNeighbours4(index1D, neighbours);
            for (uint16_t i = 0; i < count; ++i)
            {
                if (!(neighbours[i]->m_isMine))
                {
                    // unhide neighbour if not mine
                    updateTile(neighbours[i], Tile::state::notHidden);

                    // recursively unhide empty negihbours if current
                    // negihbour is empty
                    if (neighbours[i]->m_mineCounter == 0)
                        unhideEmptyNeighbours(neighbours[i] - tiles);
                }
            }
        }

        void endGame(bool userWon)
        {
            gameFinished = true;
            clock.restart();
            if (userWon)
            {
                // check win
                for (uint16_t i = 0; i < width * height; ++i)
                    if (tiles[i].m_state == Tile::state::flagged && !tiles[i].m_isMine)
                        userWon = false;
            }
            std::cout << (userWon ? "win" : "lose");
            for (uint16_t i = 0; i < width * height; ++i)
            {
                if (tiles[i].m_state != Tile::state::mineClicked && tiles[i].m_isMine)
                    updateTile(i, Tile::state::notHidden);
            }
        }

        static sf::Vector2u tileIndexFromScreenPos(const sf::Vector2i& mousePos)
        {
            if (mousePos.x < 0 || mousePos.y < 0)
                return {};
            return {mousePos.y / tileSize, mousePos.x / tileSize};
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