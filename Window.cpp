#include <iostream>
#include <fstream>
#include <chrono>
#include "Window.h"

map<int, sf::Sprite> parseDigits(sf::Sprite digits){
    map<int, sf::Sprite> digitsMap;

    for(int i = 0; i < 10; i++){
        sf::IntRect rect(i*21,0,21,32);
        digits.setTextureRect(rect);
        sf::Sprite sprite = digits;
        digitsMap.emplace(i, sprite);
    }

    sf::IntRect negRect(10*21, 0, 21, 32);
    digits.setTextureRect(negRect);
    sf::Sprite negSign = digits;
    digitsMap.emplace(-1, negSign);

    return digitsMap;
}

void setText(sf::Text &text, float x, float y){
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

void Window::readConfig() {
    std::ifstream file("files/config.cfg");
    if (!file.is_open()){
        std::cout << "File not open!" << std::endl;
    }
    std::string line;
    getline(file, line);
    this->colCount = std::stoi(line);
    getline(file, line);
    this->rowCount = std::stoi(line);
    getline(file, line);
    this->mineCount = std::stoi(line);
    file.close();
}

Window::Window() {
    this->readConfig();
}

MainWindow::MainWindow() : Window() {
    sf::RenderWindow mainWindow(sf::VideoMode((this->colCount * 32),
                                              (this->rowCount * 32) + 100), "Welcome Window");

    sf::Font font;
    if (!font.loadFromFile("files/font.ttf")) {
        std::cout << "Can't load font" << std::endl;
    }

    sf::Text welcomeText("WELCOME TO MINESWEEPER!", font, 24);
    welcomeText.setFillColor(sf::Color::White);
    welcomeText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(welcomeText, (float) (mainWindow.getSize().x)/2.0f, ((float)(mainWindow.getSize().y)/2.0f)- 150);

    sf::Text enterNameText("Enter your name:", font, 20);
    enterNameText.setFillColor(sf::Color::White);
    enterNameText.setStyle((sf::Text::Bold));
    setText(enterNameText, (float) (mainWindow.getSize().x)/2.0f, ((float)(mainWindow.getSize().y)/2.0f)- 75);

    std::string nameString = "|";
    sf::Text nameText(nameString, font, 18);
    nameText.setFillColor(sf::Color::Yellow);
    nameText.setStyle(sf::Text::Bold);
    setText(nameText, (float) (mainWindow.getSize().x)/2.0f, ((float)(mainWindow.getSize().y)/2.0f) - 45);

    int charCount = 0;


    while (mainWindow.isOpen()) {
        sf::Event event;
        while (mainWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                mainWindow.close();
            }

            if (event.type == sf::Event::KeyPressed) { //FIXME: Weird edge case where if one char is inputted, and then ":", window closes
                const sf::Keyboard::Key keycode = event.key.code;
                if (keycode >= sf::Keyboard::A && keycode <= sf::Keyboard::Z) { //Checks ASCII between A and Z
                    if(charCount == 0) {
                        char chr = static_cast<char>(keycode - sf::Keyboard::A + 'A');
                        nameString.pop_back();
                        nameString.push_back(chr);
                        nameString.append("|");
                        charCount++;
                    }
                    else if(charCount == 10);
                    else{
                        char chr = static_cast<char>(keycode - sf::Keyboard::A + 'a');
                        nameString.pop_back();
                        nameString.push_back(chr);
                        nameString.append("|");
                        charCount++;
                    }
                }
                else if(event.key.code == sf::Keyboard::BackSpace){
                    if(nameString != "|") {
                        nameString.pop_back();
                        nameString.pop_back();
                        nameString.append("|");
                        charCount--;
                    }
                }

                nameText.setString(nameString);
                setText(nameText, (float) (mainWindow.getSize().x)/2.0f,
                        ((float)(mainWindow.getSize().y)/2.0f)-45);
            }

            if (event.key.code == sf::Keyboard::Enter) {
                if (charCount >= 1) {
                    nameString.pop_back();
                    mainWindow.close();
                    GameWindow();
                }
            }

        }

        mainWindow.clear(sf::Color::Blue);
        mainWindow.draw(welcomeText);
        mainWindow.draw(enterNameText);
        mainWindow.draw(nameText);
        mainWindow.display();
    }
}

GameWindow::GameWindow() {
    sf::RenderWindow gameWindow(sf::VideoMode((this->colCount * 32),
                                              (this->rowCount * 32) + 100), "Game Window");
    auto startTime = chrono::high_resolution_clock::now();
    auto pauseTime = chrono::high_resolution_clock::now();
    auto elapsedPauseTime = chrono::duration_cast<chrono::seconds>(
            chrono::high_resolution_clock::now() - pauseTime).count();

    // For timer and flag counter
    sf::Texture &digitsText = TextureManager::getTexture("digits");
    sf::Sprite digits;
    digits.setTexture(digitsText);
    map<int, sf::Sprite> digitsMap = parseDigits(digits);

    sf::Sprite happyFaceSprite;
    happyFaceSprite.setTexture(TextureManager::getTexture("face_happy"));
    happyFaceSprite.setPosition((float) (((colCount / 2.0f) * 32) - 32), (float) (32 * (rowCount + 0.5f)));

    sf::Sprite debugButtonSprite;
    debugButtonSprite.setTexture(TextureManager::getTexture("debug"));
    debugButtonSprite.setPosition((float) ((colCount * 32) - 304), (float) (32 * (rowCount + 0.5f)));
    bool debugOn = false;

    sf::Sprite pauseSprite;
    pauseSprite.setTexture(TextureManager::getTexture("pause"));
    pauseSprite.setPosition((float) ((colCount * 32) - 240), (float) (32 * (rowCount + 0.5f)));
    bool pauseOn = false;

    sf::Sprite leaderboardSprite;
    leaderboardSprite.setTexture(TextureManager::getTexture("leaderboard"));
    leaderboardSprite.setPosition((float) ((colCount * 32) - 176), (float) (32 * (rowCount + 0.5f)));
    bool leaderOpen = false;


    bool gameWin = false;
    bool gameLose = false;


    Board board(rowCount, colCount, mineCount);
    board.randomizeMines();
    board.checkAllNearMines();

    while (gameWindow.isOpen()) {
        gameWindow.clear(sf::Color::White);
        sf::Event event;
        while (gameWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                gameWindow.close();
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mouse;
                mouse = sf::Mouse::getPosition(gameWindow);

                // Restart Game
                if (happyFaceSprite.getGlobalBounds().contains(gameWindow.mapPixelToCoords(mouse))) {
                    happyFaceSprite.setTexture(TextureManager::getTexture("face_happy"));
                    pauseSprite.setTexture(TextureManager::getTexture("pause"));
                    debugOn = false;
                    pauseOn = false;
                    leaderOpen = false;
                    board.numFlags = 0;
                    board.hideAllMines();
                    board.randomizeMines();
                    board.checkAllNearMines();
                    board.removeAllFlags();
                    board.hideAllMines();
                    gameWin = false;
                    gameLose = false;
                    startTime = chrono::high_resolution_clock::now();
                }

                // Pause Button
                if (pauseSprite.getGlobalBounds().contains(gameWindow.mapPixelToCoords(mouse)) && !gameWin && !gameLose) { //FIXME: Able to be interacted with even on gameWin
                    if (!leaderOpen) {
                        if (!pauseOn) {
                            pauseSprite.setTexture(TextureManager::getTexture("play"));
                            pauseTime = chrono::high_resolution_clock::now();
                        } else {
                            pauseSprite.setTexture(TextureManager::getTexture("pause"));
                            auto unPausedTime = chrono::high_resolution_clock::now();
                            elapsedPauseTime += (chrono::duration_cast<chrono::seconds>(
                                    unPausedTime - pauseTime)).count();
                        }
                        pauseOn = !pauseOn;
                    }
                }

                // Debug Button
                if (debugButtonSprite.getGlobalBounds().contains(gameWindow.mapPixelToCoords(mouse))
                && !pauseOn && !leaderOpen && !gameWin && !gameLose){
                    debugOn = !debugOn;
                }

                //Leaderboard Button
                if (leaderboardSprite.getGlobalBounds().contains(gameWindow.mapPixelToCoords(mouse))){
                    leaderOpen = true;
                    if (!pauseOn){
                        pauseTime = chrono::high_resolution_clock::now();
                        LeaderWindow();
                        auto unPausedTime = chrono::high_resolution_clock::now();
                        elapsedPauseTime += (chrono::duration_cast<chrono::seconds>(
                                unPausedTime - pauseTime)).count();
                        leaderOpen = false;
                    }
                    else{
                        LeaderWindow();
                        leaderOpen = false;
                    }

                    //TODO: Block interaction with game, proper implementation if debug mode is on
                }

                //Tiles
                if (mouse.x < colCount * 32 && mouse.y < rowCount * 32 && !pauseOn && !gameWin && !gameLose) {
                    int x = mouse.x / 32;
                    int y = mouse.y / 32;
                    Tile *tile = &board.tiles[y][x];
                    if (!tile->revealed && !tile->flag) { //FIXME: Adjacent Tiles not properly being revealed (issue with revealAdjacentTiles)
                        tile->revealTile();
                        if (tile->mine) {
                            gameLose = true;
                        } else if (tile->numMinesNear == 0) {
                            board.revealAdjacentTiles(tile);
                        }
                    }
                }


            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                sf::Vector2i mouse;
                mouse = sf::Mouse::getPosition(gameWindow);

                //Flag
                if (mouse.x < colCount * 32 && mouse.y < rowCount * 32 && !pauseOn && !gameWin && !gameLose) { //FIXME: Flag still able to be put on the recursively revealed tiles (issue with revealAdjacentTiles)
                    int x = mouse.x / 32;
                    int y = mouse.y / 32;
                    Tile *tile = &board.tiles[y][x];
                    if (!tile->revealed) {
                        tile->flagTile();
                        if (tile->flag) {
                            board.numFlags++;
                        } else {
                            board.numFlags--;
                        }
                    }
                }
            }
        }

        // Checks for win
        gameWin = board.checkForWin();


        auto gameDuration = std::chrono::duration_cast<std::chrono::seconds>(
                chrono::high_resolution_clock::now() - startTime);
        int totalTime = gameDuration.count();

        int minutes;
        int seconds;

        if (!pauseOn) {
            totalTime = totalTime - elapsedPauseTime;
            minutes = totalTime / 60;
            seconds = totalTime % 60;
        }

        int minutes0 = minutes / 10 % 10; //minutes index 0
        int minutes1 = minutes % 10; // minutes index 1
        int seconds0 = seconds / 10 % 10; // seconds index 0
        int seconds1 = seconds % 10; // seconds index 1


        digitsMap[minutes0].setPosition((colCount * 32) - 97, 32 * (rowCount + 0.5f) + 16);
        gameWindow.draw(digitsMap[minutes0]);

        digitsMap[minutes1].setPosition((colCount * 32) - 76, 32 * (rowCount + 0.5f) + 16);
        gameWindow.draw(digitsMap[minutes1]);

        digitsMap[seconds0].setPosition((colCount * 32) - 54, 32 * (rowCount + 0.5f) + 16);
        gameWindow.draw(digitsMap[seconds0]);

        digitsMap[seconds1].setPosition((colCount * 32) - 33, 32 * (rowCount + 0.5f) + 16);
        gameWindow.draw(digitsMap[seconds1]);


        int counter = board.numMines - board.numFlags;
        if (counter < 0){
            digitsMap[-1].setPosition(12, 32 * (rowCount + 0.5f) + 16);
            gameWindow.draw(digitsMap[-1]);
            counter = abs(counter);
        }
        int counter0 = counter / 100 % 10;
        int counter1 = counter / 10 % 10;
        int counter2 = counter % 10;

        digitsMap[counter0].setPosition(33, 32 * (rowCount + 0.5f) + 16);
        gameWindow.draw(digitsMap[counter0]);

        digitsMap[counter1].setPosition(54, 32 * (rowCount + 0.5f) + 16);
        gameWindow.draw(digitsMap[counter1]);

        digitsMap[counter2].setPosition(75, 32 * (rowCount + 0.5f) + 16);
        gameWindow.draw(digitsMap[counter2]);

        if (gameWin || gameLose) {
            if (gameWin) {
                happyFaceSprite.setTexture(TextureManager::getTexture("face_win"));
            } else if (gameLose) {
                board.revealAllMines();
                happyFaceSprite.setTexture(TextureManager::getTexture("face_lose"));
            }
        }

        if (!pauseOn) {
            board.drawBoard(gameWindow);
        }
        else{
            board.drawPausedBoard(gameWindow);
        }
        if (debugOn && !pauseOn) {
            board.drawDebugBoard(gameWindow);
        }
        gameWindow.draw(happyFaceSprite);
        gameWindow.draw(debugButtonSprite);
        gameWindow.draw(pauseSprite);
        gameWindow.draw(leaderboardSprite);
        gameWindow.display();
    }
}

LeaderWindow::LeaderWindow() {
    closed = false;
    sf::RenderWindow leaderWindow(sf::VideoMode((this->colCount * 16) + 50,
                                                (this->rowCount * 16)), "Leaderboard Window");
    sf::Font font;
    if (!font.loadFromFile("files/font.ttf")) {
        std::cout << "Can't load font" << std::endl;
    }

    readBestTimes();

    sf::Text leaderboardText("LEADERBOARD", font, 20);
    leaderboardText.setFillColor(sf::Color::White);
    leaderboardText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(leaderboardText, (float) (leaderWindow.getSize().x)/2.0f, (float) (leaderWindow.getSize().y/2.0f - 120));

    sf::Text leaderboardNames(leaderboardString, font, 18);
    leaderboardNames.setFillColor(sf::Color::White);
    leaderboardNames.setStyle(sf::Text::Bold);
    setText(leaderboardNames, (float) (leaderWindow.getSize().x)/2.0f, (float) (leaderWindow.getSize().y/2.0f + 20));

    //TODO: Read from file and display leaderboard

    while (leaderWindow.isOpen()) {
        sf::Event event;
        while (leaderWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                closed = true;
                leaderWindow.close();
            }
        }
        leaderWindow.clear(sf::Color::Blue);
        leaderWindow.draw(leaderboardText);
        leaderWindow.draw(leaderboardNames);
        leaderWindow.display();
    }
}

void LeaderWindow::writeBestTime(int score) { //TODO: Implement
    //Rewrites leaderboard.txt so that passed in score is in appropriate ranking

}

void LeaderWindow::readBestTimes(){
std::ifstream file("files/leaderboard.txt");
    if (!file.is_open()){
        std::cout << "File not open!" << std::endl;
    }
    std::string line;
    int currRankNum = 0;
    while(getline(file, line)){
        currRankNum++;
        istringstream stream(line);
        int minutesTime;
        string minutesString;
        getline(stream, minutesString, ':');
        minutesTime = std::stoi(minutesString);
        int secondsTime;
        string secondsString;
        getline(stream, secondsString, ',');
        secondsTime = std::stoi(secondsString);

        leaderboardString.append(std::to_string(currRankNum));
        leaderboardString.append("\t");
        leaderboardString.append(minutesString);
        leaderboardString.append(":");
        leaderboardString.append(secondsString);
        leaderboardString.append("\t");

        string nameString;
        getline(stream, nameString,',');

        leaderboardString.append(nameString);
        leaderboardString.append("\n\n");
    }
}

void LeaderWindow::compareBestTimes(int score) { // Should properly compare and write to file
    std::ifstream file("files/leaderboard.txt");
    if (!file.is_open()){
        std::cout << "File not open!" << std::endl;
    }
    std::string line;
    int currRankNum = 0;
    while(getline(file, line)){
        currRankNum++;
        istringstream stream(line);
        int minutesTime;
        string minutesString;
        getline(stream, minutesString, ':');
        minutesTime = std::stoi(minutesString);
        int secondsTime;
        string secondsString;
        getline(stream, secondsString, ',');
        secondsTime = std::stoi(secondsString);

        if (score < (minutesTime * 60) + secondsTime){
            writeBestTime(score);
            break;
        }
    }

}

void GameWindow::drawTimer(sf::RenderWindow &gameWindow, int minutes, int seconds){
    sf::Texture &digitsText = TextureManager::getTexture("digits");
    sf::Sprite digits;
    digits.setTexture(digitsText);
    map<int, sf::Sprite> digitsMap = parseDigits(digits);


    int minutes0 = minutes / 10 % 10; //minutes index 0
    int minutes1 = minutes % 10; // minutes index 1
    int seconds0 = seconds / 10 % 10; // seconds index 0
    int seconds1 = seconds % 10; // seconds index 1


    digitsMap[minutes0].setPosition((colCount * 32) - 97, 32 * (rowCount + 0.5f) + 16);
    gameWindow.draw(digitsMap[minutes0]);

    digitsMap[minutes1].setPosition((colCount * 32) - 76, 32 * (rowCount + 0.5f) + 16);
    gameWindow.draw(digitsMap[minutes1]);

    digitsMap[seconds0].setPosition((colCount * 32) - 54, 32 * (rowCount + 0.5f) + 16);
    gameWindow.draw(digitsMap[seconds0]);

    digitsMap[seconds1].setPosition((colCount * 32) - 33, 32 * (rowCount + 0.5f) + 16);
    gameWindow.draw(digitsMap[seconds1]);
}




