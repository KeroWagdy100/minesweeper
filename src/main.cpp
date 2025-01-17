#include <SFML/Graphics.hpp>
#include <Tilemap.h>
#include <Game.h>

int main()
{
    bool playAgain = false;
    short playOn64;
    game::Game game;
    do
    {
        std::cout << "Play on 32x32 or 64x64?\nyou can change choose dimensions when game starts the next time\n"
                    << "Enter [0] for 32x32 | [1] for 64x64 | [else] to exit game\n";
        std::cin >> playOn64;
        if (playOn64 == 1)
            game.init("res/png/tilemap-new-64.png", 64u);
        else if (playOn64 == 0)
            game.init("res/png/tilemap-new-32.png", 32u);
        else
            break;

        if (!game.run())
            break;

    } while (true);
    return 0;
}