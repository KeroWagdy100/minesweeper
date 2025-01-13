#include <SFML/Graphics.hpp>
#include <Tilemap.h>
#include <Game.h>

int main()
{
    bool playAgain = false;
    do
    {
        game::Game game;
        game.init("res/png/tilemap-new.png");
        game.run();

        std::cout << "Play Again ? (1 for Yes | 0 for No)\n";
        std::cin >> playAgain;
    } while (playAgain);
}