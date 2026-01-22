#include "game/Coin.hpp"

Coin::Coin(const sf::Texture& texture)
    : Pickup(texture)
{
    // Setup coin animation (assuming 8 frames in a row)
    for (int i = 0; i < 12; ++i)
    {
        addAnimationFrame(sf::IntRect({i * 32, 0}, {32, 32}));
    }
    setFps(10);
}

void Coin::collect()
{
    Pickup::collect();
    // Coin disappears when collected
}
