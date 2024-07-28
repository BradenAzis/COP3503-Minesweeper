#include "Tile.h"
#include "TextureManager.h"

TextureManager textureManager;

Tile::Tile() {
    int x = 0;
    int y = 0;

    rightTile = nullptr;
    leftTile = nullptr;
    aboveTile = nullptr;
    belowTile = nullptr;

    //May need these diagonals for numMinesNear. Not necessary for recursive reveal//
    topLeftTile = nullptr;
    topRightTile = nullptr;
    botLeftTile = nullptr;
    botRightTile = nullptr;

    mine = false;
    flag = false;
    revealed = false;
    numMinesNear = 0;
//    adjacentTiles = {topLeftTile, aboveTile, topRightTile,
//                     leftTile,               rightTile,
//                     botLeftTile, belowTile, botRightTile};
    tileBackgroundSprite.setTexture(TextureManager::getTexture("tile_hidden"));

    tileMineTexture = TextureManager::getTexture("mine");
    tileMineSprite.setTexture(tileMineTexture);

    tileFlagTexture = TextureManager::getTexture("flag");
    tileFlagSprite.setTexture(tileFlagTexture);
}

void Tile::revealTile() {
    if (!this->mine) {
        if (this->numMinesNear > 0) {
            std::string numMinesString = std::to_string(this->numMinesNear);
            this->tileTopSprite.setTexture(TextureManager::getTexture("number_" + numMinesString));
        }
        this->tileBackgroundSprite.setTexture(TextureManager::getTexture("tile_revealed"));
        this->revealed = true;
    }
}

void Tile::hideTile() { // For debug button
    tileBackgroundSprite.setTexture(TextureManager::getTexture("tile_hidden"));
    this->revealed = false;
}

void Tile::flagTile(){
    this->flag = !this->flag;
}

void Tile::checkNearMines() {
    int numMines = 0;
    for (auto &adjacentTile: this->adjacentTiles) {
//        std::cout << "Checking tile: " << this->x << ", " << this->y << std::endl;
        if (adjacentTile != nullptr) { //FIXME: Not currently being accessed as adjacentTile is nullptr
//            std::cout << "Checking adjacent tile: " << adjacentTile->x << ", " << adjacentTile->y << std::endl;
            if (adjacentTile->mine) {
                numMines++;
            }
//            std::cout<< "Num mines: " << numMines << std::endl;
        }
        this->numMinesNear = numMines;
    }
}