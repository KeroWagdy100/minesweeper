#include <SFML/Graphics.hpp>
#include <Tilemap.h>
#include <Game.h>

int main()
{
    game::Game game;
    game.init("res/png/tilemap-new.png");
    game.run();
}