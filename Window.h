#pragma once
#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "Tile.h"
#include "TextureManager.h"
#include "Board.h"
#include <fstream>
#include <sstream>

class Window {
protected:
    int rowCount = 0;
    int colCount = 0;
    int mineCount = 0;
public:
    Window();
    void readConfig();

};

class MainWindow : public Window{
public:
    MainWindow();

};

class GameWindow : public Window{
public:
    GameWindow();
    void drawTimer(sf::RenderWindow &gameWindow, int minutes, int seconds);
};

class LeaderWindow : public Window{
public:
    bool closed;
    LeaderWindow();
    string leaderboardString;
    void compareBestTimes(int score);
    void writeBestTime(int score);
    void readBestTimes();
};