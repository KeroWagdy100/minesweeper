// Tile.h
#pragma once

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
    
} // namespace game

