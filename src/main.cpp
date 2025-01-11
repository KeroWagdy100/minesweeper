#include <SFML/Graphics.hpp>
#include <Tilemap.h>


template <size_t size>
void randomizeLevel(uint16_t level[], const uint16_t max, const uint16_t min)
{
    srand(time(0));
    for (uint16_t i = 0; i < size; i++)
        level[i] = (rand() % max + min + 1);
}

const uint16_t WIDTH = 9;
const uint16_t HEIGHT = 9;
const uint16_t TILE_SIZE = 64;

int main()
{
    sf::RenderWindow window(sf::VideoMode({WIDTH*TILE_SIZE, HEIGHT*TILE_SIZE}), "Testing My Tilemap");
    uint16_t level[WIDTH * HEIGHT];
    randomizeLevel<WIDTH*HEIGHT>(level, 8, 0);
    

    TileMap map;
    if (!map.load("res/png/tilemap.png", {TILE_SIZE, TILE_SIZE}, level, WIDTH, HEIGHT))
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