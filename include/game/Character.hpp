#pragma once
#include "game/AnimatedSprite.hpp"

class Character : public AnimatedSprite
{
public:
    Character(const sf::Texture& texture, float maxHealth = 100.0f);

    void damage(float amount);
    void heal(float amount);
    void moveCharacter(const sf::Time& elapsed);
    bool isAlive() const;
    
    float getHealth() const;
    float getMaxHealth() const;
    sf::Vector2f getVelocity() const;

    void setMaxHealth(float maxHealth_);
    void setVelocity(const sf::Vector2f& velocity_);

    // Hitbox methods
    void setHitbox(const sf::FloatRect& hitbox_);
    void setHitbox(float offsetX, float offsetY, float width, float height);
    sf::FloatRect getHitbox() const;
    sf::FloatRect getGlobalHitbox() const;  // Hitbox in world coordinates

protected:
    float health;
    float maxHealth;
    sf::Vector2f velocity;
    sf::FloatRect hitbox;
    bool hasCustomHitbox;

};