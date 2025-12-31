#include "game/Platform.hpp"

Platform::Platform(const sf::Texture& texture, PlatformType type)
    : sf::Sprite(texture), platformType(type), size(0, 0)
{
}

void Platform::setSize(float width, float height)
{
    size = sf::Vector2f(width, height);
    
    // Calculate scale to fit the desired size
    sf::Vector2f textureSize = static_cast<sf::Vector2f>(getTexture().getSize());
    if (textureSize.x > 0 && textureSize.y > 0)
    {
        setScale({width / textureSize.x, height / textureSize.y});
    }
}

sf::FloatRect Platform::getBounds() const
{
    if (size.x > 0 && size.y > 0)
    {
        // Use custom size if set
        return sf::FloatRect(getPosition(), size);
    }
    else
    {
        // Use sprite bounds
        return getGlobalBounds();
    }
}

PlatformType Platform::getType() const
{
    return platformType;
}

void Platform::setType(PlatformType type)
{
    platformType = type;
}

bool Platform::isDeadly() const
{
    return platformType == PlatformType::DeathPit;
}