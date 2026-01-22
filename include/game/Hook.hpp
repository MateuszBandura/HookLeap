#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "game/Platform.hpp"

enum class HookState
{
    Inactive,
    Shooting,
    Attached,
    Breaking
};

class Hook
{
public:
    Hook();
    
    void shoot(const sf::Vector2f& startPos, const sf::Vector2f& direction);
    void update(const sf::Time& elapsed, const sf::Vector2f& playerPos);
    bool checkPlatformCollision(std::shared_ptr<Platform> platform, const sf::Vector2f& playerPos);
    void attach(const sf::Vector2f& attachPoint);
    void release();
    bool shouldBreak(const sf::Vector2f& playerPos) const;
    
    HookState getState() const { return state; }
    sf::Vector2f getAttachPoint() const { return attachPoint; }
    sf::Vector2f getHookPosition() const { return hookPosition; }
    float getRopeLength() const { return ropeLength; }
    bool isAttached() const { return state == HookState::Attached; }
    
    void draw(sf::RenderWindow& window) const;
    
    static constexpr float HOOK_SPEED = 800.0f;
    static constexpr float MAX_ROPE_LENGTH = 400.0f;
    static constexpr float MAX_HOOK_RANGE = 500.0f;
    static constexpr float HOOK_DURATION = 3.0f;
    static constexpr float MAX_ROPE_ANGLE = 160.0f;
    
private:
    HookState state;
    sf::Vector2f hookPosition;
    sf::Vector2f attachPoint;
    sf::Vector2f shootDirection;
    float ropeLength;
    float attachTime;
};
