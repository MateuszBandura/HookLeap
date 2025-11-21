#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <cmath>

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "HookLeap");
    sf::Clock clock;
    sf::Time elapsed;

    sf::Vector2 vector = {250, 100};


     while (window.isOpen())
    {
        elapsed = clock.restart();

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::White);  
        window.display();
    }
    return 0;
}