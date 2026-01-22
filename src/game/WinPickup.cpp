#include "game/WinPickup.hpp"

WinPickup::WinPickup(const sf::Texture& texture)
    : Pickup(texture)
{
    // Setup win pickup animation (assuming 8 frames)
    for (int i = 0; i < 6; ++i)
    {
        addAnimationFrame(sf::IntRect({i * 16, 0}, {16, 16}));
    }
    setFps(10);
}

void WinPickup::collect()
{
    Pickup::collect();
    // Triggers win screen in MainWindow
}
