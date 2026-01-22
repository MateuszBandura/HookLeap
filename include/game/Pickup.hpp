#pragma once
#include "game/AnimatedSprite.hpp"

class Pickup : public AnimatedSprite
{
public:
    Pickup(const sf::Texture& texture);
    virtual ~Pickup() = default;
    
    virtual void collect();
    bool isCollected() const;
    virtual bool shouldRemainVisible() const { return false; }
    
protected:
    bool collected;
};
