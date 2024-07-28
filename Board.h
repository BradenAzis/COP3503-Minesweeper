#pragma once
#include <iostream>
#include "Window.h"
#include <vector>
#include <ctime>
#include <fstream>
#include <SFML/Graphics.hpp>

struct Board{
    std::vector<std::vector<Tile>> tiles;
    int rowCount;
    int colCount;
    int numMines;
    int numFlags;
    bool gameWin;



    Board(int rowCount, int colCount, int numMines);
    void revealAdjacentTiles(Tile* tile); //Separate from Tile::revealTile()
    void revealAllMines();
    void hideAllMines();
    void drawBoard(sf::RenderWindow &gameWindow);
    void randomizeMines();
    void removeAllFlags();
    void checkAllNearMines();
    void drawPausedBoard(sf::RenderWindow &gameWindow);
    void drawDebugBoard(sf::RenderWindow &gameWindow);
    bool checkForWin();
    void setAdjacentTiles();
};