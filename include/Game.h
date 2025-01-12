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

    
    void generateLevel(game::Tile tiles[], uint16_t width, uint16_t height)
    {
        srand(time(0));
        uint16_t size = width * height;
        uint16_t nMines = size / 3;
        for (uint16_t mine = 0; mine < nMines; ++mine)
        {
            uint16_t mineIndex;
            do
            {
                mineIndex = rand() % size;
            } while (tiles[mineIndex].m_isMine);
            tiles[mineIndex].m_isMine = true;
            uint16_t i = mineIndex / height;
            uint16_t j = mineIndex % height;
            if (i != height - 1)
                tiles[mineIndex + width].m_mineCounter++;
            if (j != width - 1)
                tiles[mineIndex + 1].m_mineCounter++;
            if (i != height - 1 && j != width - 1)
                tiles[mineIndex + width + 1].m_mineCounter++;
            if (i != 0)
                tiles[mineIndex - width].m_mineCounter++;
            if (j != 0)
                tiles[mineIndex - 1].m_mineCounter++;
            if (i != 0 && j != 0)
                tiles[mineIndex - width - 1].m_mineCounter++;
        }
    }

};