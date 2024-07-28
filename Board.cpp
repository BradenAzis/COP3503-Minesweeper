#include "Board.h"

Board::Board(int rowCount, int colCount, int numMines) {
    this->rowCount = rowCount;
    this->colCount = colCount;
    this->numMines = numMines;
    numFlags = 0;
    for (int i = 0; i < rowCount; i++) {
        std::vector<Tile> row;
        for (int j = 0; j < colCount; j++) {
            Tile tile;
            tile.x = j;
            tile.y = i;
            row.push_back(tile);
        }
        tiles.push_back(row);
    }
}

void Board::revealAdjacentTiles(Tile *tile) {
    tile->revealTile();
    for (auto &adjacentTile : tile->adjacentTiles) { //FIXME: Most likely issue with diagonal tiles
        if (adjacentTile != nullptr && !adjacentTile->revealed && !adjacentTile->flag && !adjacentTile->mine) {
            if(adjacentTile->numMinesNear == 0) {
                revealAdjacentTiles(adjacentTile);
            }
            else{
                adjacentTile->revealTile();
            }
            adjacentTile->revealed = true;
        }
    }
}

void Board::revealAllMines() {
    for (auto &row : tiles) {
        for (auto &tile : row) {
            if (tile.mine){
                tile.revealed = true;
            }
        }
    }
}

void Board::hideAllMines(){
    for (auto &row : tiles){
        for (auto &tile : row){
            if (tile.mine && !tile.flag){
                tile.hideTile();
            }
            else if (tile.flag){
                tile.tileBackgroundSprite.setTexture(TextureManager::getTexture("tile_hidden"));
            }
        }
    }
}

void Board::drawBoard(sf::RenderWindow &gameWindow) {
    for (int i = 0; i < this->tiles.size(); i++) { // Row (x)
        for (int j = 0; j < this->tiles[i].size(); j++) { // Col (y)
            this->tiles[i][j].tileBackgroundSprite.setPosition(j * 32, i * 32);
            this->tiles[i][j].tileMineSprite.setPosition(j * 32, i * 32);
            this->tiles[i][j].tileTopSprite.setPosition(j* 32, i * 32);
            this->tiles[i][j].tileFlagSprite.setPosition(j * 32, i * 32);
            gameWindow.draw(this->tiles[i][j].tileBackgroundSprite); //FIXME: Has to change position per iteration
            if (!this->tiles[i][j].revealed && this->tiles[i][j].flag){
                gameWindow.draw(this->tiles[i][j].tileFlagSprite);
            }
            else if (this->tiles[i][j].revealed && !this->tiles[i][j].flag){
                if ((this->tiles[i][j].numMinesNear > 0 && !this->tiles[i][j].mine)) { //FIXME: Potential bug if tile becomes revealed after being flagged and now has a number
                    gameWindow.draw(this->tiles[i][j].tileTopSprite);
                }
                else if (this->tiles[i][j].mine){
                    gameWindow.draw(this->tiles[i][j].tileMineSprite);
                }
            }
            else if (this->tiles[i][j].revealed && this->tiles[i][j].flag){ //TODO: Haven't tested, may be edge cases
                gameWindow.draw(this->tiles[i][j].tileFlagSprite);
                if (this->tiles[i][j].mine){
                    gameWindow.draw(this->tiles[i][j].tileMineSprite);
                }
            }
        }
    }
}

void Board::randomizeMines() {
    //Wipes board of mines
    for (int i = 0; i < this->tiles.size(); i++) {
        for (int j = 0; j < this->tiles[i].size(); j++) {
            this->tiles[i][j].mine = false;
            this->tiles[i][j].numMinesNear = 0;
            this->tiles[i][j].revealed = false;
            this->tiles[i][j].flag = false;
            this->tiles[i][j].tileBackgroundSprite.setTexture(TextureManager::getTexture("tile_hidden"));
        }
    }

    //Randomly places mines
    srand(time(NULL));
    int minesPlaced = 0;
    while (minesPlaced < this->numMines) {
        int randRow = rand() % this->rowCount;
        int randCol = rand() % this->colCount;
        if (!this->tiles[randRow][randCol].mine) {
            this->tiles[randRow][randCol].mine = true;
            minesPlaced++;
        }
    }

    setAdjacentTiles();
    checkAllNearMines();
}

void Board::removeAllFlags() {
    for (auto &row : tiles){
        for (auto &tile : row){
            tile.flag = false;
        }
    }
}

void Board::checkAllNearMines() {
    for (auto &row : tiles){
        for (auto &tile : row){
            tile.checkNearMines();
        }
    }
}

void Board::drawPausedBoard(sf::RenderWindow &gameWindow) {
    for (int i = 0 ; i < tiles.size(); i++){
        for (int j = 0; j < tiles[i].size(); j++){
            sf::Sprite tempSprite;
            sf::Texture &tempTexture = TextureManager::getTexture("tile_revealed");
            tempSprite.setTexture(tempTexture);
            tempSprite.setPosition(j * 32, i * 32);
            gameWindow.draw(tempSprite);
        }
    }
}

bool Board::checkForWin() {
    int numRevealed = 0;
    for (auto &row : tiles){
        for (auto &tile : row){
            if (tile.revealed){
                numRevealed++;
            }
        }
    }
    if (numRevealed == (this->rowCount * this->colCount) - this->numMines){
        gameWin = true;
        for (auto &row : tiles){
            for (auto &tile : row){
                if (tile.mine){
                    tile.flag = true;
                }
            }
        }
    }

    else if (numFlags == this->numMines){
        int numCorrectFlags = 0;
        for (auto &row : tiles){
            for (auto &tile : row){
                if (tile.mine && tile.flag){
                    numCorrectFlags++;
                }
            }
        }
        if (numCorrectFlags == this->numMines){
            gameWin = true;
        }
        else{
            gameWin = false;
        }
    }
    else{
        gameWin = false;
    }
    return gameWin;


}

void Board::drawDebugBoard(sf::RenderWindow &gameWindow) {
    for (auto &row : tiles){
        for (auto &tile : row){
            if (tile.mine){
                gameWindow.draw(tile.tileMineSprite);
            }
        }
    }
}

void Board::setAdjacentTiles() {
    for (int i = 0; i < this->tiles.size(); i++) { // Row (y)
        for (int j = 0; j < this->tiles[i].size(); j++) { // Col (x)
            if (i > 0) {
                if (j > 0) {
                    this->tiles[i][j].topLeftTile = &this->tiles[i - 1][j - 1];
                }
                this->tiles[i][j].aboveTile = &this->tiles[i - 1][j];
                if (j < this->colCount - 1) {
                    this->tiles[i][j].topRightTile = &this->tiles[i - 1][j + 1];
                }
            }

            if (j > 0) {
                this->tiles[i][j].leftTile = &this->tiles[i][j - 1];
            }

            if (j < this->colCount - 1) {
                this->tiles[i][j].rightTile = &this->tiles[i][j + 1];
            }

            if (i < this->rowCount - 1) {
                if (j > 0) {
                    this->tiles[i][j].botLeftTile = &this->tiles[i + 1][j - 1];
                }
                this->tiles[i][j].belowTile = &this->tiles[i + 1][j];
                if (j < this->colCount - 1) {
                    this->tiles[i][j].botRightTile = &this->tiles[i + 1][j + 1];
                }
            }
            this->tiles[i][j].adjacentTiles = {this->tiles[i][j].topLeftTile, this->tiles[i][j].aboveTile,
                                               this->tiles[i][j].topRightTile,
                                               this->tiles[i][j].leftTile, this->tiles[i][j].rightTile,
                                               this->tiles[i][j].botLeftTile, this->tiles[i][j].belowTile,
                                               this->tiles[i][j].botRightTile};
        }
    }
}
