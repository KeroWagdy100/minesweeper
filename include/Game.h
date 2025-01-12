// Game.h
///////////////////////////////////////////
#pragma once

#include <SFML/Graphics.hpp>

namespace game
{
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
            hidden, notHidden, flagged
        };
        short m_mineCounter = 0; // 2-bytes
        bool  m_isMine = 0; // 1-byte
        state m_state = state::notHidden; // 1-byte // TODO: Let Init State = Hidden

        mapIndex getMapIndex()
        {
            if (m_state == state::hidden)
                return mapIndex::hidden;
            else if (m_state == state::flagged)
                return mapIndex::flag;
            else if (m_isMine) // TODO: handle mineClicked case
                return mapIndex::mine;
            else
                return static_cast<mapIndex>(m_mineCounter);
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
                uint16_t i = mineIndex / height;
                uint16_t j = mineIndex % height;
                if (i != height - 1)
                {
                    tiles[mineIndex + width].m_mineCounter++; // down
                    if (j != width - 1)
                        tiles[mineIndex + width + 1].m_mineCounter++; // down-right
                    if (j != 0)
                        tiles[mineIndex + width - 1].m_mineCounter++; // down-left
                }

                if (i != 0)
                {
                    tiles[mineIndex - width].m_mineCounter++; // up
                    if (j != 0)
                        tiles[mineIndex - width - 1].m_mineCounter++; // up-left
                    if (j != width - 1)
                        tiles[mineIndex - width + 1].m_mineCounter++; // up-right
                }

                if (j != width - 1)
                    tiles[mineIndex + 1].m_mineCounter++; // right
                if (j != 0)
                    tiles[mineIndex - 1].m_mineCounter++; // left
            }
        }

        void updateMapIndices()
        {
            for (uint16_t i = 0; i < width * height; i++)
                mapIndices[i] = tiles[i].getMapIndex();
        }
    
        void handleEvent(const std::optional<sf::Event>& event)
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
    private:
        // Static Data
        static const uint16_t width = 9u; // 2-bytes
        static const uint16_t height = 9u; // 2-bytes
        static const uint16_t tileSize = 64u; // 2-bytes
        ////////////////////////////////////////////////

        // Member Fields
        uint16_t mines = (width * height) / 3;
        sf::RenderWindow window { sf::VideoMode({width * tileSize, height * tileSize}), "Minesweeper" }; // unknown-bytes
        TileMap tilemap;
        Tile tiles[width * height]; // 9 * 9 * 4 = 324-bytes
        uint16_t mapIndices[width * height];
        ////////////////////////////////////////////////
    };
};