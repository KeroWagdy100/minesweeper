#include <SFML/Graphics.hpp>
#include <Tilemap.h>


template <size_t size>
void randomizeLevel(std::array<uint16_t, size>& level, const uint16_t max, const uint16_t min)
{
    for (uint16_t i = 0; i < size; i++)
        level[i] = (rand() % max + min + 1);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({16*32, 8*32}), "Testing Tilemap");
    // define the level with an array of tile indices
    constexpr std::array<uint16_t, 16 * 8> level = {
        0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3,
        0, 1, 0, 0, 2, 0, 3, 3, 3, 0, 1, 1, 1, 0, 0, 0,
        0, 1, 1, 0, 3, 3, 3, 0, 0, 0, 1, 1, 1, 2, 0, 0,
        0, 0, 1, 0, 3, 0, 2, 2, 0, 0, 1, 1, 1, 1, 2, 0,
        2, 0, 1, 0, 3, 0, 2, 2, 2, 0, 1, 1, 1, 1, 1, 1,
        0, 0, 1, 0, 3, 2, 2, 2, 0, 0, 0, 0, 1, 1, 1, 1,
    };
    // std::array<uint16_t, 16 * 8> level;
    // randomizeLevel(level, 3, 0);
    

    TileMap map;
    if (!map.load("res/sfml-test-tileset.png", {32, 32}, level.data(), 16, 8))
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
