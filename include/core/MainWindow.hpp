#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class MainWindow 
{
public:
    MainWindow(unsigned int width = 800, unsigned int height = 600, const std::string& title = "HookLeap");
    void run();
private:
    sf::RenderWindow window;
    sf::Clock clock;

    void handleEvents();
    void update(sf::Time& elapsed);
    void render();
};