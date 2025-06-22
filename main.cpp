#include <SFML/Graphics.hpp>
#include <string>

bool LIVE = true;
bool GAME = false;

const int windowWidth = 1400;
const int windowHeight = 800;

const int cellSize = 10;

const int cols = windowWidth / cellSize;
const int rows = windowHeight / cellSize;

void send_to_shader(sf::Texture &texture, bool(*grid)[cols], int rows, int cols);
void updateGameLogic(const sf::Keyboard::Key &Key, bool(*grid)[cols], bool(*gameGrid)[cols], sf::Vector2i& pointPlayer);
bool shouldPerformAction(sf::Clock& clock, float interval);

int WinMain() {
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Live game", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(30);


    sf::VertexArray quad(sf::TrianglesStrip, 4);
        quad[0].position = sf::Vector2f(0, 0);
        quad[1].position = sf::Vector2f(windowWidth, 0);
        quad[2].position = sf::Vector2f(0, windowHeight);
        quad[3].position = sf::Vector2f(windowWidth, windowHeight);


    sf::Shader shader;
    if(!shader.loadFromFile("main_shader.fs", sf::Shader::Fragment)){
        return -1;
    }

    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        return -1;
    }
    
    sf::Text FPS("FPS: 30", font, 20);
    FPS.setPosition(10, 10);
    FPS.setStyle(sf::Text::Bold); 

    sf::Text Mode("Mode: " + std::string(( !LIVE && !GAME ) ? "EDIT" : ( LIVE && !GAME ) ? "LIFE" : "GAME"), font, 20);
    Mode.setPosition(10, 40);
    Mode.setStyle(sf::Text::Bold); 


    bool grid[rows][cols] = {false};
    bool bufferGrid[rows][cols] = {false};

    bool gameGrid[rows][cols] = {false};
    sf::Vector2i pointPlayer(0,0);

    sf::Clock clock;
    
    sf::Texture texture;
    texture.create(cols, rows);
    send_to_shader(texture, grid, rows, cols);

    shader.setUniform("gridTexture", texture);

    sf::Texture texture1;
    texture1.create(cols, rows);
    send_to_shader(texture1, gameGrid, rows, cols);
    shader.setUniform("gridGameTexture", texture1);

    shader.setUniform("resolution", sf::Glsl::Vec2(windowWidth,windowHeight));
    shader.setUniform("cellSize", cellSize);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyReleased){
                if(event.key.code == sf::Keyboard::Q && GAME == false){
                    LIVE = !LIVE;
                }
                if(event.key.code == sf::Keyboard::E && LIVE == true){
                    GAME = !GAME;
                }



                if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num9) {
                    int framerate = (event.key.code - sf::Keyboard::Num1 + 1) * 5;
                    FPS.setString("FPS: " + std::to_string(framerate));
                    window.setFramerateLimit(framerate);
                }else if (event.key.code == sf::Keyboard::Num0) {
                    window.setFramerateLimit(0);
                    FPS.setString("FPS: -");
                }
                

            }
            if (event.type == sf::Event::MouseButtonPressed && GAME == false) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    int col = mousePos.x / cellSize;
                    int row = mousePos.y / cellSize;

                    if (col >= 0 && col < cols && row >= 0 && row < rows) {
                        grid[row][col] = !grid[row][col];
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonPressed && GAME == true) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    int col = mousePos.x / cellSize;
                    int row = mousePos.y / cellSize;

                    if (col >= 0 && col < cols && row >= 0 && row < rows && grid[row][col] == false) {
                        gameGrid[row][col] = !gameGrid[row][col];
                        gameGrid[pointPlayer.x][pointPlayer.y] = false;
                        pointPlayer = sf::Vector2(row, col);
                    }
                }
            }
        }


        if(shouldPerformAction( clock, 0.2f) && GAME == true){
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::W) ){
                updateGameLogic(sf::Keyboard::W, grid, gameGrid, pointPlayer);
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::S) ){
                updateGameLogic(sf::Keyboard::S, grid, gameGrid, pointPlayer);
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) ){
                updateGameLogic(sf::Keyboard::A, grid, gameGrid, pointPlayer);
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
                updateGameLogic(sf::Keyboard::D, grid, gameGrid, pointPlayer);
            }
        }
        std::copy(&grid[0][0], &grid[0][0] + rows * cols, &bufferGrid[0][0]);

        if (LIVE && !GAME) {
            for (int y = 0; y < rows; ++y) {
                for (int x = 0; x < cols; ++x) {
                    unsigned short count = 0;
                    for(int dy = -1; dy <= 1; ++dy){
                        for(int dx = -1; dx <= 1; ++dx){
                            if (dy == 0 && dx == 0) continue;
                            int ny = y + dy;
                            int nx = x + dx;
                            if(ny >= 0 && ny < rows && nx >= 0 && nx < cols){
                                if (true == bufferGrid[ny][nx]){
                                    ++count;
                                }
                            }
                        }
                    }
                    if (false == bufferGrid[y][x] && count == 3){
                        grid[y][x] = true;
                    }
                    else if ( true == bufferGrid[y][x] && (count < 2 || count > 3)){
                        grid[y][x] = false;
                    }
                }
            }
        }
        send_to_shader(texture, grid, rows, cols);
        shader.setUniform("gridTexture", texture);

        send_to_shader(texture1, gameGrid, rows, cols);
        shader.setUniform("gridGameTexture", texture1);

        window.clear(sf::Color(206, 206, 206));
        window.draw(quad, &shader);
        Mode.setString("Mode: " + std::string(( !LIVE && !GAME ) ? "EDIT" : ( LIVE && !GAME ) ? "LIFE" : "GAME"));
        window.draw(FPS);
        window.draw(Mode);

        window.display();
    }
    return 1;
}
void updateGameLogic(const sf::Keyboard::Key &Key, bool(*grid)[cols], bool(*gameGrid)[cols], sf::Vector2i &pointPlayer){
    if(Key == sf::Keyboard::D && grid[pointPlayer.x][pointPlayer.y + 1] == false){
        gameGrid[pointPlayer.x][pointPlayer.y + 1] = true;
        gameGrid[pointPlayer.x][pointPlayer.y] = false;
        pointPlayer = sf::Vector2i(pointPlayer.x, pointPlayer.y + 1);
    }
    else if(Key == sf::Keyboard::A && grid[pointPlayer.x][pointPlayer.y - 1] == false){
        gameGrid[pointPlayer.x][pointPlayer.y - 1] = true;
        gameGrid[pointPlayer.x][pointPlayer.y] = false;
        pointPlayer = sf::Vector2i(pointPlayer.x, pointPlayer.y - 1);
    }
    else if(Key == sf::Keyboard::W && grid[pointPlayer.x - 1][pointPlayer.y] == false){
        gameGrid[pointPlayer.x  - 1][pointPlayer.y] = true;
        gameGrid[pointPlayer.x][pointPlayer.y] = false;
        pointPlayer = sf::Vector2i(pointPlayer.x - 1, pointPlayer.y);
    }
    else if(Key == sf::Keyboard::S && grid[pointPlayer.x + 1][pointPlayer.y] == false){
        gameGrid[pointPlayer.x  + 1][pointPlayer.y] = true;
        gameGrid[pointPlayer.x][pointPlayer.y] = false;
        pointPlayer = sf::Vector2i(pointPlayer.x + 1, pointPlayer.y);
    }
}

void send_to_shader(sf::Texture &texture, bool(*grid)[cols], int rows, int cols){
     std::vector<sf::Uint8> data(rows * cols * 4);
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            int index = (y * cols + x) * 4; 
            bool isAlive = grid[y][x];
            data[index + 0] = isAlive ? 255 : 0; // R
            data[index + 1] = isAlive ? 255 : 0; // G
            data[index + 2] = isAlive ? 255 : 0; // B
            data[index + 3] = 255;               // A
        }
    }
    texture.update(data.data());
}

bool shouldPerformAction(sf::Clock& clock, float interval) {
    if (clock.getElapsedTime().asSeconds() >= interval) {
        clock.restart();
        return true;
    }
    return false;
}
