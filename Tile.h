#pragma once
#include <fstream>
#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "TextureManager.h"

 struct Tile {
    int x;
    int y;

    Tile* rightTile;
    Tile* leftTile;
    Tile* aboveTile;
    Tile* belowTile;

    //May need these diagonals for numMinesNear. Not necessary for recursive reveal//
    Tile* topLeftTile;
    Tile* topRightTile;
    Tile* botLeftTile;
    Tile* botRightTile;

    bool mine;
    bool flag;
    bool revealed;
    int numMinesNear;
    std::vector<Tile*> adjacentTiles;

    sf::Sprite tileBackgroundSprite;

    sf::Texture& tileMineTexture = TextureManager::getTexture("mine");
    sf::Sprite tileMineSprite;

    sf::Texture& tileFlagTexture = TextureManager::getTexture("flag"); //Flags or numbers
    sf::Sprite tileFlagSprite;

    sf::Sprite tileTopSprite;



    Tile();
    void revealTile();
    void hideTile();
    void flagTile();
    void checkNearMines();




};

