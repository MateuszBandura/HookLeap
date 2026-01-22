#include "game/Character.hpp"

Character::Character(const sf::Texture& texture, float maxHealth)
    : AnimatedSprite(texture), maxHealth(maxHealth), health(maxHealth), velocity(0.0f, 0.0f), hitbox({0, 0}, {0, 0}), hasCustomHitbox(false)
{
}

void Character::damage(float amount)
{
    if (amount < 0) return; 
    
    health -= amount;
    if (health < 0)
        health = 0;
}

void Character::heal(float amount)
{
    if (amount < 0) return;
    
    health += amount;
    if (health > maxHealth)
        health = maxHealth;
}

void Character::moveCharacter(const sf::Time& elapsed)
{
    sf::Vector2f movement = velocity * elapsed.asSeconds();
    move(movement);
}

bool Character::isAlive() const
{
    return health > 0;
}

float Character::getHealth() const
{
    return health;
}

float Character::getMaxHealth() const
{
    return maxHealth;
}

void Character::setMaxHealth(float maxHealth_)
{
    maxHealth = maxHealth_;
    if (health > maxHealth)
        health = maxHealth;
}

sf::Vector2f Character::getVelocity() const
{
    return velocity;
}

void Character::setVelocity(const sf::Vector2f& velocity_)
{
    velocity = velocity_;
}

	
void Character::setHitbox(const sf::FloatRect& hitbox_)
{
    hitbox = hitbox_;
    hasCustomHitbox = true;
}
void Character::setHitbox(float offsetX, float offsetY, float width, float height)
{
    hitbox = sf::FloatRect({offsetX, offsetY}, {width, height});
    hasCustomHitbox = true;
}
sf::FloatRect Character::getHitbox() const
{
    return hitbox;
}
sf::FloatRect Character::getGlobalHitbox() const
{
    if (!hasCustomHitbox)
        return getGlobalBounds();
    
    sf::Vector2f pos = getPosition();
    return sf::FloatRect(
        {pos.x + hitbox.position.x,
        pos.y + hitbox.position.y},
        {hitbox.size.x,
        hitbox.size.y}
    );
}
