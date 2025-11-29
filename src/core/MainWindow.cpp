#include "core/MainWindow.hpp"

MainWindow::MainWindow(unsigned int width, unsigned int height, const std::string& title)
{
    window.create(sf::VideoMode({width, height}), title);
}

void MainWindow::handleEvents()
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();
    }
}

void MainWindow::update(sf::Time& elapsed)
{
    
}

void MainWindow::render()
{
    window.clear(sf::Color::White);
    window.display();
}

void MainWindow::run()
{
    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        handleEvents();
        update(elapsed);
        render();
    }
}
