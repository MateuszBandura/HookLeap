#include "game/Pickup.hpp"

Pickup::Pickup(const sf::Texture& texture)
    : AnimatedSprite(texture), collected(false)
{
}

void Pickup::collect()
{
    collected = true;
}

bool Pickup::isCollected() const
{
    return collected;
}
