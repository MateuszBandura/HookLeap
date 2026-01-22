#include "game/Hook.hpp"
#include <cmath>

Hook::Hook()
    : state(HookState::Inactive), hookPosition(0, 0), attachPoint(0, 0),
      shootDirection(0, 0), ropeLength(0), attachTime(0)
{
}

void Hook::shoot(const sf::Vector2f& startPos, const sf::Vector2f& direction)
{
    state = HookState::Shooting;
    hookPosition = startPos;
    
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0)
    {
        shootDirection = sf::Vector2f(direction.x / length, direction.y / length);
    }
    else
    {
        shootDirection = sf::Vector2f(0, -1);
    }
}

void Hook::update(const sf::Time& elapsed, const sf::Vector2f& playerPos)
{
    if (state == HookState::Shooting)
    {
        hookPosition += shootDirection * HOOK_SPEED * elapsed.asSeconds();
        
        float distance = std::sqrt(
            std::pow(hookPosition.x - playerPos.x, 2) +
            std::pow(hookPosition.y - playerPos.y, 2)
        );
        
        if (distance > MAX_HOOK_RANGE)
        {
            release();
        }
    }
    else if (state == HookState::Attached)
    {
        attachTime += elapsed.asSeconds();
        
        if (attachTime >= HOOK_DURATION)
        {
            release();
        }
    }
}

bool Hook::checkPlatformCollision(std::shared_ptr<Platform> platform, const sf::Vector2f& playerPos)
{
    if (state != HookState::Shooting)
        return false;
    
    if (platform->getType() != PlatformType::Floating)
        return false;
    
    if (platform->getBounds().position.y >= playerPos.y)
        return false;
    
    sf::FloatRect platformBounds = platform->getBounds();
    
    if (hookPosition.x >= platformBounds.position.x &&
        hookPosition.x <= platformBounds.position.x + platformBounds.size.x &&
        hookPosition.y >= platformBounds.position.y &&
        hookPosition.y <= platformBounds.position.y + platformBounds.size.y)
    {
        attach(sf::Vector2f(
            hookPosition.x,
            platformBounds.position.y + platformBounds.size.y
        ));
        return true;
    }
    
    return false;
}

void Hook::attach(const sf::Vector2f& point)
{
    state = HookState::Attached;
    attachPoint = point;
    attachTime = 0;
}

void Hook::release()
{
    state = HookState::Inactive;
    ropeLength = 0;
    attachTime = 0;
}

bool Hook::shouldBreak(const sf::Vector2f& playerPos) const
{
    if (state != HookState::Attached)
        return false;
    
    sf::Vector2f ropeVec = playerPos - attachPoint;
    float angle = std::atan2(ropeVec.x, ropeVec.y) * 180.0f / 3.14159f;
    angle = std::abs(angle);
    
    if (angle > MAX_ROPE_ANGLE)
        return true;
    
    float currentLength = std::sqrt(ropeVec.x * ropeVec.x + ropeVec.y * ropeVec.y);
    
    if (currentLength > MAX_ROPE_LENGTH * 1.2f)
        return true;
    
    return false;
}

void Hook::draw(sf::RenderWindow& window) const
{
    if (state == HookState::Shooting)
    {
        sf::CircleShape hookCircle(5);
        hookCircle.setPosition({hookPosition.x - 5, hookPosition.y - 5});
        hookCircle.setFillColor(sf::Color(150, 150, 150));
        window.draw(hookCircle);
    }
    else if (state == HookState::Attached)
    {
        sf::CircleShape attachCircle(3);
        attachCircle.setPosition({attachPoint.x - 3, attachPoint.y - 3});
        attachCircle.setFillColor(sf::Color::Red);
        window.draw(attachCircle);
    }
}
