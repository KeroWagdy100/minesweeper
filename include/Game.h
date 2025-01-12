// Game.h
///////////////////////////////////////////
#pragma once

#include <SFML/Graphics.hpp>

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
                auto counter = getNeighbours(mineIndex, neighbours);
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

        void updateTile(Tile* tilePtr, const Tile::state& state)
        {
            if (tilePtr == nullptr)
                return;
            tilePtr->m_state = state;
            uint16_t tileIndex1D = tilePtr - tiles;
            auto tileIndex2D = convertD12(tileIndex1D, width);
            tilemap.updateTile(tileIndex2D.x, tileIndex2D.y, tiles[tileIndex1D].getMapIndex());
        }

        // Fills given array of pointers and returns its size
        uint16_t getNeighbours(const uint16_t tileIndex1D, Tile** neighbours)
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

        void handleEvent(const std::optional<sf::Event>& event)
        {
            static bool wasPeeking = false;
            static uint16_t tilePeekedIndex1D = 0;
            // last hovered tile
            static uint16_t hoveredIndex = 0;

            if (event->is<sf::Event::Closed>())
                window.close();

            else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>())
            {

                // PEEK ON HOVER
                uint16_t col, row;
                {
                    auto tileIndex2D = tileIndexFromScreenPos(mouse->position);
                    col = tileIndex2D.y;
                    row = tileIndex2D.x;
                }
                // hover on different tile
                if (hoveredIndex != col + row * width)
                {
                    // hide prev hovered tile 
                    auto hoveredIndex2D = convertD12(hoveredIndex, width);
                    if (tiles[hoveredIndex].m_state == Tile::state::hidden)
                        tilemap.updateTile(hoveredIndex2D.x, hoveredIndex2D.y, mapIndex::hidden);
                    // update current hovered tile
                    hoveredIndex = col + row * width;
                    Tile temp = tiles[hoveredIndex];
                    temp.m_state = Tile::state::notHidden;
                    tilemap.updateTile(row, col, temp.getMapIndex());
                }
            }

            else if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (wasPeeking)
                {
                    Tile* neighborus[8];
                    auto counter = getNeighbours(tilePeekedIndex1D, neighborus);
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
                bool left = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
                bool right = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
                // Both Buttons Clicked (Peeking Neighbours)
                auto tileIndex1D = convertD21(tileIndexFromScreenPos(mouse->position), width);
                auto tileIndex2D = tileIndexFromScreenPos(mouse->position);
                if (left && right)
                {
                    Tile* neighbours[8];
                    auto counter = getNeighbours(tileIndex1D, neighbours);

                    for (uint16_t i = 0; i < counter; i++)
                    {
                        if (neighbours[i]->m_state == Tile::state::hidden)
                            updateTile(neighbours[i], Tile::state::peek);
                    }
                    wasPeeking = true;
                    tilePeekedIndex1D = tileIndex1D;
                }
                else if (left)
                {
                    if (tiles[tileIndex1D].m_isMine)
                    {
                        updateTile(tileIndex1D, Tile::state::mineClicked);
                        endGame();
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
                        updateTile(tileIndex1D, Tile::state::flagged);
                }
                

            }
        }

        void unhideEmptyNeighbours(uint16_t index1D)
        {
            auto index2D = convertD12(index1D, width);
            auto& row = index2D.x;
            auto& col = index2D.y;
            // Row of Clicked Empty
            for (; col < width; ++col)
            {
                index1D = convertD21(index2D, width);
                if (tiles[index1D].m_mineCounter != 0)
                    break;
                updateTile(index1D, Tile::state::notHidden);
            }
            if (col > 0)
                col--;
            for (; col >= 0; --col)
            {
                index1D = convertD21(index2D, width);
                if (tiles[index1D].m_mineCounter != 0)
                    break;
                updateTile(index1D, Tile::state::notHidden);
                if (col == 0)
                    break;
            }

            // Upper rows
            for (; row >= 0; row--)
            {
                // if ()
                if (row == 0)
                    break;
            }

        }

        void endGame()
        {
            std::cout << "Game Finished\n";
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
        // uint16_t mines = (width * height) / 5; // TODO: change it 
        uint16_t mines = 10; // TODO: change it 
        sf::RenderWindow window { sf::VideoMode({width * tileSize, height * tileSize}), "Minesweeper" }; // unknown-bytes
        TileMap tilemap;
        Tile tiles[width * height]; // 9 * 9 * 4 = 324-bytes
        uint16_t mapIndices[width * height]; // 9 * 9 * 2 = 162-bytes
        ////////////////////////////////////////////////
    };
};