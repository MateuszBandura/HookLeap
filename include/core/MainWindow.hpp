#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "game/Player.hpp"
#include "core/Physics.hpp"
#include "game/AnimatedSprite.hpp"
#include "game/Platform.hpp"

class MainWindow 
{
public:
    MainWindow(unsigned int width = 800, unsigned int height = 600, const std::string& title = "HookLeap");
    void run();
private:
    sf::RenderWindow window;
    sf::View camera;
    sf::Clock clock;
    std::unique_ptr<Player> player;
    sf::Texture characterTexture;
    Physics physics;
    std::vector<std::shared_ptr<Platform>> platforms;
    sf::Texture platformTexture;

    void init();
    void handleEvents();
    void update(sf::Time& elapsed);
    void render();
    void setupMap();
    void updateCamera();
};