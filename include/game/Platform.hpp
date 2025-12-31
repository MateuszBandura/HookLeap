#pragma once
#include <SFML/Graphics.hpp>

enum class PlatformType
{
    Ground,
    Floating,
    DeathPit
};

class Platform : public sf::Sprite
{
public:
    Platform(const sf::Texture& texture, PlatformType type = PlatformType::Floating);
    
    void setSize(float width, float height);
    sf::FloatRect getBounds() const;
    
    PlatformType getType() const;
    void setType(PlatformType type);
    
    bool isDeadly() const;  // Returns true if this platform kills on contact

private:
    PlatformType platformType;
    sf::Vector2f size;
};