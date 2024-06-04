# Minesweeper

A clone of Minesweeper made in C with SDL2.

## Building for Linux

### Prerequisites

Before building, ensure you have the following libraries installed:

- `SDL2`
- `SDL2_ttf`
- `SDL2_image`

### Build

To build Minesweeper for Linux from source, follow these steps:

1. Clone the Minesweeper repository from GitHub:

    ```shell
    git clone https://github.com/uwiwiow/minesweeper
    cd minesweeper/
    ```

2. Create a build directory for MinGW:

    ```shell
    mkdir build
    cd build
    ```

3. Run CMake to configure the build:

    ```shell
    cmake ..
    ```

4. Build the project using make:

    ```shell
    make
    ```
   
This will compile the source code and generate the executable.

### Running the Game

Once the build process is complete, you can run the Minesweeper game directly from the source build directory:
    
```shell
./minesweeper
```

## Building for Windows with MinGW from Linux

### Prerequisites

Before building, ensure you have the following libraries installed:

- `mingw-w64-toolchain`
- `mingw-w64-sdl2`
- `mingw-w64-sdl2_ttf`
- `mingw-w64-sdl2_image`

### Build

1. Clone the Minesweeper repository from GitHub:

    ```shell
    git clone https://github.com/uwiwiow/minesweeper
    cd minesweeper/
    ```

2. Create a build directory for MinGW:

    ```shell
    mkdir build-mingw
    cd build-mingw
    ```

3. Run CMake with the MinGW toolchain file:

    ```shell
    x86_64-w64-mingw32-cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-mingw.cmake ..
    ```
    
    To make a static build add `-DSTATIC_BUILD=ON`:

    ```shell
    x86_64-w64-mingw32-cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-mingw.cmake -DSTATIC_BUILD=ON ..
    ```

4. Once CMake configuration is done, build the project:
    
    ```shell
    make
    ```
    This will generate the minesweeper.exe executable.

### Running the Game

Once the build process is complete, you can run the Minesweeper game directly from the directory:

```shell
.\minesweeper.exe
```

## Run from Binary Distribution

### Linux

1. Download and extract the binary distribution archive:

    ```shell
    tar -xzvf minesweeper_linux_x86_64.tar.gz
    cd minesweeper-*
    ```

2. Run the game:

    ```shell
    ./run.sh
    ```

### Windows

1. Download and extract the binary distribution archive:

    ```shell
    Expand-Archive .\minesweeper_windows_x86_64.zip
    cd .\minesweeper-*
    ```

2. Run the game:

    ```shell
    .\minesweeper.exe
    ```

## Gameplay

### General

There are almost 99 mines (due to duplicated positions).

Every time you start a game, you will open a blank tile first. From there, you will be opening the adjacent tiles.

### Controls

- WASD for movement
- O to restart the game
- P to uncover all the tiles
- K to open a tile
- L to flag or question mark a tile
- 1-5 to show debug information

### Win condition

Open all the tiles without hitting a mine.

### Debug keys

The in-game debug keys and their explanations:

#### key 1
Displays the (x, y) coordinates of the cursor.

#### key 2
Displays (Type, Amount):

* Type represents the type of tile:
    * BLANK_TILE: A blank tile
    * NUMBER: A tile with a number representing the number of mines around it
    * MINE: A mine
    * M-EXP: A mine that the player opened, only appears when it's game over

* Amount represents the number of mines around the tile where the cursor is located.

#### key 3
Displays (Mark, Visible):

* Mark represents the player's assessment of the tile:
    * CLEAR: A regular tile
    * FLAG: A flagged tile
    * QSTN: A questioned tile

* Visible is a boolean value indicating if the current tile is visible in normal mode. (Uncovering all the tiles with key P doesn't affect this value.)

#### key 4
Displays (State):

* State represents the current state of the game:
    * START: The player hasn't opened any tile (uncovering all the tiles with key P doesn't affect this)
    * PLAYING: The player has opened a tile (if the game is restarted and all the tiles were uncovered, this feature will be disabled automatically)
    * WIN: The player has opened all the safe tiles, uncovering all the mines
    * LOSE: The player has opened a tile with a mine, uncovering all the tiles and the tile with the mine opened is colored red

#### key 5
Toggles the debug information background between black and transparent.

### Customize 
Almost all the settings are set in the Status struct.
```c
// default settings
Status status = {
.TILE = 40, // The size of each tile
.W_TILES = 32, // Number of tiles in width
.H_TILES = 16, // Number of tiles in height
.BOMBS = 99, // Number of bombs in the game (might not be accurate)
.STATE = START, // Default state when starting the game (typically set to START)
.VISIBLE_TILES = 0 // Number of starting visible tiles (used to calculate win condition; typically left as 0)
.FIRST_CELL = BLANK_TILE, // Default first tile to open (BLANK_TILE / NUMBER / MINE / ANY)
.MAX_ITERATIONS = 10000 // Maximum iterations for generating the board before automatic termination (-1 for unlimited)
};
```
The width and height of the window are calculated based on the values given in the status.
