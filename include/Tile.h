// Tile.h
#pragma once

namespace game
{

    /**
     * @brief type of each tile in the tileset sorted by their indices.
     * 
     */
    enum mapIndex {
        empty = 0, one, two, three, four, five, six, seven, eight,
        hidden, flag, mine, mineClicked
    };

    // State of each tile
    enum class TileState : char // 1-byte
    {
        hidden, notHidden, flagged, peek, mineClicked
    };

    // should be 4-bytes
    struct Tile 
    {
    public:
        short m_mineCounter = 0; // 2-bytes
        bool  m_isMine = 0; // 1-byte
        TileState m_state = TileState::hidden; // 1-byte

        mapIndex getMapIndex() const {
            return getMapIndex(*this);
        }

        static mapIndex getMapIndex(const Tile& tile)
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
    
} // namespace game