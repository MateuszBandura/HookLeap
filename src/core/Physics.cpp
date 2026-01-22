#include "core/Physics.hpp"

void Physics::addPlatform(std::shared_ptr<Platform> platform)
{
    platforms.push_back(platform);
}

void Physics::clearPlatforms()
{
    platforms.clear();
}

const std::vector<std::shared_ptr<Platform>>& Physics::getPlatforms() const
{
    return platforms;
}

void Physics::applyGravity(sf::Vector2f& velocity, const sf::Time& elapsed)
{
    velocity.y += GRAVITY * elapsed.asSeconds();
    
    if (velocity.y > MAX_FALL_SPEED)
        velocity.y = MAX_FALL_SPEED;
}

void Physics::applyFriction(sf::Vector2f& velocity, bool isOnGround)
{
    if (isOnGround)
        velocity.x *= GROUND_FRICTION;
    else
        velocity.x *= AIR_FRICTION;
    
    // Stop very small movements
    if (std::abs(velocity.x) < 0.1f)
        velocity.x = 0;
}

bool Physics::checkPlatformCollision(const sf::FloatRect& bounds, Platform& platform, sf::Vector2f& velocity, sf::Vector2f& correction)
{
    sf::FloatRect platformBounds = platform.getBounds();
    
    if (!bounds.findIntersection(platformBounds))
        return false;
    
    auto intersection = bounds.findIntersection(platformBounds).value();
    
    if (intersection.size.x > intersection.size.y)
    {
        // Vertical collision (top or bottom)
        if (bounds.position.y < platformBounds.position.y)
        {
            // Landing on top of platform
            correction.y = -intersection.size.y;
            velocity.y = 0;
            return true; // On ground
        }
        else
        {
            // Hit bottom of platform
            correction.y = intersection.size.y;
            velocity.y = 0;
        }
    }
    else
    {
        // Horizontal collision (left or right)
        if (bounds.position.x < platformBounds.position.x)
        {
            // Hit right side of platform
            correction.x = -intersection.size.x;
        }
        else
        {
            // Hit left side of platform
            correction.x = intersection.size.x;
        }
        velocity.x = 0;
    }
    
    return false;
}

bool Physics::handleCollisions(Character& character, sf::Vector2f& velocity, bool& fellInPit, bool& hitDeadlyPlatform)
{
    sf::FloatRect bounds = character.getGlobalHitbox();
    bool isOnGround = false;
    fellInPit = false;
    hitDeadlyPlatform = false;
    
    // Check for falling below all platforms (death pit)
    float lowestPlatform = -1000000.0f;
    for (const auto& platform : platforms)
    {
        sf::FloatRect platformBounds = platform->getBounds();
        if (platformBounds.position.y + platformBounds.size.y > lowestPlatform)
            lowestPlatform = platformBounds.position.y + platformBounds.size.y;
    }
    
    if (bounds.position.y > lowestPlatform + 200.0f)
    {
        fellInPit = true;
        return false;
    }
    
    // Check collisions with all platforms
    sf::Vector2f totalCorrection(0, 0);
    
    for (auto& platform : platforms)
    {
        sf::Vector2f correction(0, 0);
        bool groundContact = checkPlatformCollision(bounds, *platform, velocity, correction);
        
        totalCorrection += correction;
        
        if (groundContact)
        {
            isOnGround = true;
            
            // Check if platform is deadly
            if (platform->isDeadly())
                hitDeadlyPlatform = true;
        }
    }
    
    // Apply position correction
    if (totalCorrection.x != 0 || totalCorrection.y != 0)
    {
        character.setPosition(character.getPosition() + totalCorrection);
    }
    
    return isOnGround;
}
