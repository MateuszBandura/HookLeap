#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include "game/Platform.hpp"
#include "game/Character.hpp"


class Physics
{
public:
    static constexpr float GRAVITY = 980.0f;
    static constexpr float MAX_FALL_SPEED = 600.0f;
    static constexpr float GROUND_FRICTION = 0.85f;
    static constexpr float AIR_FRICTION = 0.95f;
    
    void addPlatform(std::shared_ptr<Platform> platform);
    void clearPlatforms();

    const std::vector<std::shared_ptr<Platform>>& getPlatforms() const;
    
    void applyGravity(sf::Vector2f& velocity, const sf::Time& elapsed);
    void applyFriction(sf::Vector2f& velocity, bool isOnGround);
    
    bool handleCollisions(Character& character, sf::Vector2f& velocity, bool& fellInPit, bool& hitDeadlyPlatform);
    
private:
    std::vector<std::shared_ptr<Platform>> platforms;
    
    bool checkPlatformCollision(const sf::FloatRect& bounds, Platform& platform, sf::Vector2f& velocity, sf::Vector2f& correction);
};
