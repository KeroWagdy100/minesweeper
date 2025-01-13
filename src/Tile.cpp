// Tile.cpp
#include <Tile.h>

namespace game
{
    mapIndex Tile::getMapIndex() const {
        return getMapIndex(*this);
    }

    mapIndex Tile::getMapIndex(const Tile& tile) // static
    {
        const auto& state = tile.m_state;
        const auto& isMine = tile.m_isMine;
        const auto& mineCounter = tile.m_mineCounter;

        if (state == TileState::hidden)
            return mapIndex::hidden;

        else if (state == TileState::flagged)
            return mapIndex::flag;

        else if (state == TileState::peek)
            return mapIndex::empty;

        else if (state == TileState::mineClicked && isMine)
            return mapIndex::mineClicked;

        else if (isMine)
            return mapIndex::mine;
        else
            return static_cast<mapIndex>(mineCounter);
    }
};