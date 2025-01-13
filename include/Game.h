// Game.h
///////////////////////////////////////////
#pragma once

#include <SFML/Graphics.hpp>
#include <Tilemap.h>
#include <Tile.h>
#include <algorithm>
#include <iostream>

namespace game
{
    // Convert Dimensions (1D to 2D)
    sf::Vector2u convertDim1To2(uint16_t oneDim, uint16_t width);
    // Convert Dimensions (2D to 1D)
    uint16_t convertDim2To1(const sf::Vector2u& twoDim, uint16_t width);

    class Game
    {
    public:
        // Default Constructor (You have to call init to initialize game)
        Game()
        {
        }

        // Init The Game (should be called before run)
        bool init(const std::filesystem::path& tilesetPath, uint16_t tileSize);
        

        /**
         * @brief Game/Main Loop
         * 
         * @return true if game finishes before player closing window
         * @return false if player closes window
         */
        bool run();
        

    private:
        /**
         * @brief puts mines in random tiles and updating their neighbours counter
         * 
         */
        void generateLevel();


    
        /**
         * @brief update one tile's state
         * 
         * @param tileIndex1D index in 1Dim tiles array
         * @param state tile's new state
         */
        void updateTile(uint16_t tileIndex1D, const TileState& state);

        /**
         * @brief update one tile's state
         * 
         * @param tilePtr  tile pointer
         * @param state tile's new state
         */
        void updateTile(Tile* tilePtr, const TileState& state);


        /**
         * @brief Get array of pointers to neighbours of one tile
         * 
         * @param tileIndex1D index in 1Dim tiles array
         * @param neighbours array of pointers (PLEASE MAKE SURE THAT THIS IS ALLOCATED IN MEMORY)
         * @return uint16_t number of neighbours found (max: 8)
         * Neighbours can be [down left, left, up left, up, up right, right, down right, down]
         */
        uint16_t getNeighbours8(const uint16_t tileIndex1D, Tile** neighbours);


        /**
         * @brief Get array of pointers to hidden-close neighbours of one tile
         * a hidden neighbour: is a tile whose state is TileState::hidden
         * a close neighbour can be [down, left, up, right]
         * @param tileIndex1D index in 1Dim tiles array
         * @param neighbours array of pointers (PLEASE MAKE SURE THAT THIS IS ALLOCATED IN MEMORY)
         * @return uint16_t number of hidden-close neighbours found (max: 4)
         */
        uint16_t getHiddenNeighbours4(const uint16_t tileIndex1D, Tile** neighbours);
        
        void handleEvent(const std::optional<sf::Event>& event);

        /**
         * @brief opens/unhides non-mined neighbours of a given tile
         * 
         * @param index1D index in 1Dim tiles array
         */
        void unhideEmptyNeighbours(uint16_t index1D);


        /**
         * @brief check if player win 
         * 
         * @return false if any tile with a flag is not mined or vice versa
         * @return true when all mined tiles are flagged and vice versa
         */
        bool checkWin();

        void endGame(bool userWon);

        /**
         * @brief returns tileIndex in 1-D array of given position on screen
         * 
         * @param screenPos screen position (according to sfml)
         * @return uint16_t index of tile in 1D array 
         */
        uint16_t tileIndexFromScreenPos(const sf::Vector2i& screenPos);

    private:
        // Static Data
        static const uint16_t width = 9u;
        static const uint16_t height = 9u;
        ////////////////////////////////////////////////

        // Member Fields
        sf::RenderWindow window;
        uint16_t tileSize; // tile size in pixel (e.g. 64 x 64)
        Tile tiles[width * height]; // array of structs representing tile states
        TileMap tilemap; // the board that is drawn

        const uint16_t mines = 10; // Number of Mines in the map
        uint16_t flags = 0; // Number of flags put by player
        uint16_t mapIndices[width * height];

        sf::Clock clock;
        sf::Font font;
        sf::Text timerText {font};
        sf::Text minesText {font};

        bool gameFinished;
        ////////////////////////////////////////////////
    };
};