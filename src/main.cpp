#include <SFML/Graphics.hpp>
#include <Tilemap.h>
#include <Game.h>

template <short size>
void tilesToMapIndex(game::Tile level[], uint16_t indices[])
{
    for (uint16_t i = 0; i < size; i++)
        indices[i] = level[i].getMapIndex();
}

const uint16_t WIDTH = 9;
const uint16_t HEIGHT = 9;
const uint16_t TILE_SIZE = 64;

int main()
{
    sf::RenderWindow window(sf::VideoMode({WIDTH*TILE_SIZE, HEIGHT*TILE_SIZE}), "Testing My Tilemap");
    game::Tile level[WIDTH*HEIGHT];
    uint16_t mapIndices[WIDTH*HEIGHT];
    game::generateLevel(level, WIDTH, HEIGHT);
    tilesToMapIndex<WIDTH*HEIGHT>(level, mapIndices);
    

    TileMap map;
    if (!map.load("res/png/tilemap-new.png", {TILE_SIZE, TILE_SIZE}, mapIndices, WIDTH, HEIGHT))
        return -1;

    // run the main loop
    while (window.isOpen())
    {
        // handle events
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // draw the map
        window.clear();
        window.draw(map);
        window.display();
    }
}