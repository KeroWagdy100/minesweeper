# Minesweeper (Simple C++ & SFML Project)

[Minesweeper](https://en.wikipedia.org/wiki/Minesweeper_(video_game)) is a logic puzzle video game genre generally played on personal computers. The game features a grid of clickable tiles, with hidden "mines" (depicted as naval mines in the original game) scattered throughout the board. The objective is to clear the board without detonating any mines, with help from clues about the number of neighboring mines in each field.

![Game Icon png](res/png/mine-icon-256.png)
![Game Screenshot](res/jpg/game-screenshot.jpg)

# CMake SFML Project Template

This repository is based on CMake SFML project [template](https://github.com/SFML/cmake-sfml-project).
All CMake script files are written by default in the template. Just knew how to use a github repo template, used the template, built the project using CMake and started coding!

# How to install

Game is not published yet, but you can always clone the repo or donwload it right away and build the project yourself and start playing.

- [CMake](https://cmake.org/download/) V3.28.0 Required.
- [SFML](https://www.sfml-dev.org/download/) V3.0.0 Required (CMake fetches it in the script, you don't have to install it yourself).

Once you have game files on your system:

- Open your favourite code editor or any IDE and figure out how to use CMake on it. Most popular IDEs support CMake projects with very little effort on your part.

   - [VS Code](https://code.visualstudio.com) via the [CMake extension](https://code.visualstudio.com/docs/cpp/cmake-linux)
   - [Visual Studio](https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-170)
   - [CLion](https://www.jetbrains.com/clion/features/cmake-support.html)
   - [Qt Creator](https://doc.qt.io/qtcreator/creator-project-cmake.html)

- Use CMake to build the game. Using CMake from the command line is straightforward as well.
   Be sure to run these commands in the root directory of the project you just created.

   ```
   cmake -B build
   cmake --build build
   ```

- After the build, you can start the game by openning the [.exe](https://en.wikipedia.org/wiki/Executable) found in 
   ```
   gameDir/build/bin/main.exe
   ```

Here are some useful resources if you want to learn more about CMake:

- [Official CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/)
- [How to Use CMake Without the Agonizing Pain - Part 1](https://alexreinking.com/blog/how-to-use-cmake-without-the-agonizing-pain-part-1.html)
- [How to Use CMake Without the Agonizing Pain - Part 2](https://alexreinking.com/blog/how-to-use-cmake-without-the-agonizing-pain-part-2.html)
- [Better CMake YouTube series by Jefferon Amstutz](https://www.youtube.com/playlist?list=PL8i3OhJb4FNV10aIZ8oF0AA46HgA2ed8g)

## How to play

I don't want to write the rules myself because the game is already very popluar and I haven't changed anything in the logic, so here's what [minesweeper.online](https://minesweeper.online/) writes as rules:

"
Minesweeper rules are very simple. The board is divided into cells, with mines randomly distributed. To win, you need to open all the cells. The number on a cell shows the number of mines adjacent to it. Using this information, you can determine cells that are safe, and cells that contain mines. Cells suspected of being mines can be marked with a flag using the right mouse button.
"


## License

### CMake and SFML License
The source code is dual licensed under Public Domain and MIT -- choose whichever you prefer.

### My License
I don't know, just whataver you want
